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

#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include "qjson/QJsonObject.h"

#include "qtcofe_page_project.h"
#include "qtcofe_project_tree.h"
#include "qtcofe_dialog_data.h"
#include "qtcofe_dialog_import.h"
#include "qtcofe_dialog_task.h"
#include "qtcofe_session.h"
#include "qtcofe_server.h"
#include "qtcofe_srvdefs.h"


qtCOFE::ProjectPage::ProjectPage ( DataModel      *dm,
                                   QWidget        *parent,
                                   Qt::WindowFlags flags )
                   : Page(dm,parent,flags)  {
}

qtCOFE::ProjectPage::~ProjectPage()  {
  if (!currentProject.isEmpty())
    save_project_state();
}

void qtCOFE::ProjectPage::makeLayout()  {
QVBoxLayout *vbox = new QVBoxLayout();

  jobTree   = new ProjectTree(dataModel);
  hsplitter = new QSplitter();
  hsplitter->addWidget ( jobTree );
  hsplitter->addWidget ( new QWidget() );

  vbox->addWidget ( hsplitter );
  vbox->setContentsMargins ( 0,0,0,0 );
  setLayout ( vbox );

  connect ( jobTree,SIGNAL(add_new_job(int)),
            this,SLOT(addJob(int)) );
  connect ( jobTree,SIGNAL(del_job(int,int)),
            this,SLOT(delJob(int,int)) );
  connect ( jobTree,SIGNAL(crjob_changed(int)),
            this,SLOT(switchJob(int)) );
  connect ( jobTree,SIGNAL(view_job_data(int)),
            this,SLOT(dataInspector(int)) );

}

void qtCOFE::ProjectPage::loadProject()  {
QJsonObject jsonData;
  if ((!currentProject.isEmpty()) &&
      (session->projectPath!=currentProject))
    save_project_state();
  currentProject = session->projectPath;
  project_query ( jsonData,qtCOFE_SERVER_ACT_GetListOfJobs );
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
    jobTree->makeTree ( jsonReply );
  }

//  jobTree->setFullWidth();

}

void qtCOFE::ProjectPage::addJob ( int jobID )  {
TaskDialog    *tdlg;
QStringList    dtypes;
QList<QTreeWidgetItem *> nodes;

  jobTree->getProjectedData ( dtypes,nodes );
  tdlg = new TaskDialog ( this,dataModel,dtypes );

  tdlg->show();
  tdlg->setFixedSize ( tdlg->size() );
  if (tdlg->exec()==QDialog::Accepted)  {
    QJsonObject *jsonData = jobTree->getTreeData();
    jsonData->insert ( "parent"   ,jobID );
    jsonData->insert ( "task_type",tdlg->getSelTaskType() );
    project_query ( *jsonData,qtCOFE_SERVER_ACT_AddJob    );
    delete jsonData;
    if (tdlg->getSelTaskType()=="task_import")  {
      DataImportDialog *didlg = new DataImportDialog ( this,
                                   jobTree->currentJobId(),dataModel );
      didlg->exec();
      if (didlg->importCount()<=0)
        jobTree->deleteCurrentJob();
      else  {
        QJsonObject jsd;
        project_query ( jsd,qtCOFE_SERVER_ACT_GetListOfJobs );
      }
    }
  }

}

void qtCOFE::ProjectPage::delJob ( int jobID, int nextCrJobID )  {
  QJsonObject *jsonData = jobTree->getTreeData();
  jsonData->insert ( "job" ,jobID );
  jsonData->insert ( "next",nextCrJobID );
  project_query ( *jsonData,qtCOFE_SERVER_ACT_DelJob );
  delete jsonData;
}

void qtCOFE::ProjectPage::switchJob ( int jobID )  {

}

void qtCOFE::ProjectPage::save_project_state()  {

}

void qtCOFE::ProjectPage::dataInspector ( int jobID )  {
DataDialog *ddlg;

  ddlg = new DataDialog ( this,jobTree,jobTree->currentNode(),
                          dataModel );
  ddlg->exec();

//  QMessageBox::information(this,"Not implemented",
//                           "Data Inspector -- not implemented");

}
