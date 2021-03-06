// $Id: qtcofe_page_project.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_project <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::ProjectPage
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

//#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QUrl>

#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"

#include "qtcofe_page_project.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_project_tree.h"
#include "qtcofe_dialog_data.h"
#include "qtcofe_dialog_import.h"
#include "qtcofe_dialog_task.h"
#include "qtcofe_preferences.h"
#include "qtcofe_jsrview.h"
#include "qtcofe_session.h"
#include "qtcofe_server.h"
#include "qtcofe_srvdefs.h"


qtCOFE::ProjectPage::ProjectPage ( DataModel      *dm,
                                   QSettings      *stn,
                                   QWidget        *parent,
                                   Qt::WindowFlags flags )
                   : Page(dm,stn,parent,flags)  {
  timer = new QTimer(this);
  timer->setSingleShot ( true );
  connect ( timer,SIGNAL(timeout()),this,SLOT(refreshProject()) );
}


qtCOFE::ProjectPage::~ProjectPage()  {
  if (!currentProject.isEmpty())
    save_project_state();
}


void qtCOFE::ProjectPage::makeLayout()  {
QVBoxLayout *vbox = new QVBoxLayout();

  jobTree   = new ProjectTree(dataModel);
//  hsplitter = new QSplitter();
//  hsplitter->addWidget ( jobTree );
//  hsplitter->addWidget ( new QWidget() );

//  vbox->addWidget ( hsplitter );
  vbox->addWidget ( jobTree );
  vbox->setContentsMargins ( 0,0,0,0 );
  setLayout ( vbox );

  connect ( jobTree,SIGNAL(add_new_job(int)),
            this,SLOT(addJob(int)) );
  connect ( jobTree,SIGNAL(del_job(int,int)),
            this,SLOT(delJob(int,int)) );
  connect ( jobTree,SIGNAL(run_job(int)),
            this,SLOT(runJob(int)) );
  connect ( jobTree,SIGNAL(view_report(int)),
            this,SLOT(viewReport(int)) );
  connect ( jobTree,SIGNAL(crjob_changed(int)),
            this,SLOT(switchJob(int)) );
  connect ( jobTree,SIGNAL(view_job_data(int)),
            this,SLOT(dataInspector(int)) );

}

void qtCOFE::ProjectPage::loadProject()  {
QJsonObject jsonData;
  timer->stop();
  if ((!currentProject.isEmpty()) &&
      (session->projectPath!=currentProject))
    save_project_state();
  currentProject = session->projectPath;
  jobTree->clearTree();
  project_query ( jsonData,qtCOFE_SERVER_ACT_GetListOfJobs );
  timer->start ( preferences->refreshPeriod );
}

void qtCOFE::ProjectPage::refreshProject()  {
QJsonObject jsonData;
  project_query ( jsonData,qtCOFE_SERVER_ACT_GetListOfJobs );
  timer->start ( preferences->refreshPeriod );
}


void qtCOFE::ProjectPage::project_query ( QJsonObject & jsonData,
                                          QString action )  {

  if (!jobTree)  return;

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( action,jsonData,jsonReply );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive))  {
    QMessageBox::information ( this,"JSON Reply",
      "Result key is missed. This is a program error, please "
      "report to developers" );
  } else  {
    if (jsonReply.value("result").toString()!="OK")  {
      QString message = "Operation failed, result code is <b>" +
              jsonReply.value("result").toString() + "</b>:<p><i>" +
              jsonReply.value("message").toString() + "</i>.";
      if (!jsonReply.keys().contains("jobs",Qt::CaseInsensitive))
        message.append ( "<p>List of jobs could not be read." );
      QMessageBox::information ( this,"Error",message );
    }
    if (action==qtCOFE_SERVER_ACT_GetReportURI)  {
      jsonData.insert ( "report_uri",jsonReply.value("report_uri") );
    } else if (action!=qtCOFE_SERVER_ACT_SetData)  {
      if ((action==qtCOFE_SERVER_ACT_GetListOfJobs) ||
          (action==qtCOFE_SERVER_ACT_RunJob))
            jobTree->updateTree ( jsonReply );
      else  jobTree->makeTree   ( jsonReply );
    }
  }

}


void qtCOFE::ProjectPage::addJob ( int jobID )  {
TaskDialog                      *tdlg;
QList<QList<JobData *> >         jobData;
QList<QList<QTreeWidgetItem *> > nodes;
QString                          taskType;
int                              job_id    = jobID;
bool                             createJob = true;

  timer->stop();  // prevent interference

  jobTree->getProjectedData ( jobData,nodes );
  tdlg = new TaskDialog ( this,dataModel,jobData,jobTree );

  tdlg->show();
  tdlg->setFixedSize ( tdlg->size() );
  if (tdlg->exec()==QDialog::Accepted)  {
    taskType = tdlg->getSelTaskType();
    if (tdlg->getSelTaskKey()==JobData::Ambiguous)  {
      QJsonObject *jsonData = jobTree->getTreeData();
      jsonData->insert ( "task_type",QString(qtCOFE_TASK_DataSelection) );
      jsonData->insert ( "parent"   ,job_id );
      project_query ( *jsonData,qtCOFE_SERVER_ACT_AddJob );
      delete jsonData;
      DataDialog *ddlg = new DataDialog ( this,
                  jobTree,jobTree->currentNode(),
                  taskType,dataModel,"Data Selection",
                  "Select Data for task '" +
                      dataModel->taskName(taskType) +
                         "'" );
      ddlg->resizeToData();
      if (ddlg->exec()==QDialog::Accepted)  {
        job_id = jobTree->currentJobId();
        jsonData = new QJsonObject();
        jsonData->insert ( "job_id",job_id );
        QJsonArray *a = ddlg->getSelections();
        jsonData->insert ( "data_list",*a );
        project_query ( *jsonData,qtCOFE_SERVER_ACT_SetData );
        delete jsonData;
        delete a;
      } else  {
        jobTree->deleteCurrentJob();
        createJob = false;
      }
      delete ddlg;
    }
    if (createJob)  {
      QJsonObject *jsonData = jobTree->getTreeData();
      jsonData->insert ( "parent"   ,job_id    );
      jsonData->insert ( "task_type",taskType );
      project_query ( *jsonData,qtCOFE_SERVER_ACT_AddJob    );
      delete jsonData;
      if (taskType==qtCOFE_TASK_DataImport)  {
        DataImportDialog *didlg = new DataImportDialog ( this,
                                   jobTree->currentJobId(),dataModel );
        didlg->exec();
        if (didlg->importCount()<=0)
          jobTree->deleteCurrentJob();
        else  {
          QJsonObject jsd;
          project_query ( jsd,qtCOFE_SERVER_ACT_GetListOfJobs );
          jsonData = jobTree->getTreeData();
          job_id = jobTree->currentJobId();
          jsonData->insert ( "job" ,job_id );
          jsonData->insert ( "next",job_id );
          project_query ( *jsonData,qtCOFE_SERVER_ACT_RunJob );
          delete jsonData;
        }
      }
    }
  }

  delete tdlg;

  timer->start ( preferences->refreshPeriod );

}


void qtCOFE::ProjectPage::delJob ( int jobID, int nextCrJobID )  {
  timer->stop();  // prevent interference
  QJsonObject *jsonData = jobTree->getTreeData();
  jsonData->insert ( "job" ,jobID );
  jsonData->insert ( "next",nextCrJobID );
  project_query    ( *jsonData,qtCOFE_SERVER_ACT_DelJob );
  delete jsonData;
  timer->start ( preferences->refreshPeriod );
}

void qtCOFE::ProjectPage::runJob ( int jobID )  {
  timer->stop();  // prevent interference
  QJsonObject *jsonData = new QJsonObject(); // = jobTree->getTreeData();
  jsonData->insert ( "job" ,jobID );
  jsonData->insert ( "next",jobID );
  project_query    ( *jsonData,qtCOFE_SERVER_ACT_RunJob );
  delete jsonData;
  timer->start ( preferences->refreshPeriod );
}

void qtCOFE::ProjectPage::viewReport ( int jobID )  {
QJsonObject *jsonData = new QJsonObject(); // = jobTree->getTreeData();
JSRView     *jsrview;

  timer->stop();  // prevent interference

  jsonData->insert ( "job" ,jobID );
  jsonData->insert ( "next",jobID );
  project_query    ( *jsonData,qtCOFE_SERVER_ACT_GetReportURI );

  jsrview = new JSRView ( preferences,settings,this );
  jsrview->loadPage ( QUrl(jsonData->value("report_uri").toString()) );
  jsrview->show();

  delete jsonData;

  timer->start ( preferences->refreshPeriod );

}

void qtCOFE::ProjectPage::switchJob ( int jobID )  {
}

void qtCOFE::ProjectPage::save_project_state()  {
}

void qtCOFE::ProjectPage::dataInspector ( int jobID )  {
  timer->stop();  // prevent interference
  DataDialog *ddlg = new DataDialog ( this,jobTree,
                                      jobTree->currentNode(),
                                      "",dataModel,"Data Inspector",
                                      "Data Inspector" );
  ddlg->resizeToData();
  ddlg->exec();
  delete ddlg;
  timer->start ( preferences->refreshPeriod );
}
