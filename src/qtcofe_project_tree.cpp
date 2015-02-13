// $Id: qtcofe_project_tree.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_project_tree <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::ProjectTree
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QWindowsStyle>
#include <QFontMetrics>
#include <QMessageBox>

#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"
//#include <stdio.h>

#include "qtcofe_project_tree.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_job.h"
#include "qtcofe_session.h"
#include "qtcofe_dialogs.h"
#include "qtcofe_defs.h"
#include "qtx/qtx_tree.h"
//#include "qtcofe_preferences.h"


qtCOFE::ProjectTree::ProjectTree ( DataModel      *dm,
                                   QWidget        *parent,
                                   Qt::WindowFlags flags )
                   : QWidget(parent,flags)  {
  dataModel   = dm;
  preferences = dataModel->getPreferences();
  session     = dataModel->getSession    ();
  makeLayout();
}

qtCOFE::ProjectTree::~ProjectTree() {
}


void qtCOFE::ProjectTree::makeLayout()  {
QVBoxLayout *vbox = new QVBoxLayout();
QHBoxLayout *hbox = new QHBoxLayout();
QString      btnStyle = preferences->getToolButtonStyle();
int          btnSize  = preferences->getToolButtonSize ();

  jobTree = new qtx::Tree();
  jobTree->setColumnCount ( 1 );
  jobTree->setSelectionBehavior ( QAbstractItemView::SelectRows );
  jobTree->setSelectionMode     ( QAbstractItemView::SingleSelection );
  jobTree->setHeaderLabel       ( "Project tree" );
  jobTree->setAlternatingRowColors ( true );
  jobTree->setStyle ( new QWindowsStyle() );
  vbox->addWidget ( jobTree,100 );

  add_btn = new QToolButton();
  add_btn->setIcon        ( QIcon(qtCOFE_Plus_icon) );
  add_btn->setIconSize    ( QSize(btnSize,btnSize) );
  add_btn->setStyleSheet  ( btnStyle );
  hbox->addWidget         ( add_btn,1 );
  del_btn = new QToolButton();
  del_btn->setIcon        ( QIcon(qtCOFE_Minus_icon) );
  del_btn->setIconSize    ( QSize(btnSize,btnSize) );
  del_btn->setStyleSheet  ( btnStyle );
  hbox->addWidget         ( del_btn,1 );
  hbox->addStretch        ( 100 );
  data_btn = new QToolButton();
  data_btn->setIcon       ( QIcon(qtCOFE_Data_icon) );
  data_btn->setIconSize   ( QSize(btnSize,btnSize) );
  data_btn->setStyleSheet ( btnStyle );
  hbox->addWidget         ( data_btn,1 );

  hbox->setContentsMargins ( 0,0,0,0 );
  hbox->setSpacing ( 4 );

  vbox->addLayout ( hbox,0 );
  vbox->setSpacing ( 4 );

  setLayout ( vbox );

  connect ( add_btn ,SIGNAL(clicked()),this,SLOT(addBtnClicked())  );
  connect ( del_btn ,SIGNAL(clicked()),this,SLOT(delBtnClicked())  );
  connect ( data_btn,SIGNAL(clicked()),this,SLOT(dataBtnClicked()) );

  connect ( jobTree,
      SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
      this,SLOT(crJobChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );

}

Q_DECLARE_METATYPE ( qtCOFE::Job* )

void qtCOFE::ProjectTree::addJob ( const QJsonObject   & obj,
                                   QTreeWidgetItem     *item,
                                   QTreeWidgetItem *& crItem,
                                   int              & maxID )  {
Job             *job   = new Job(obj,dataModel);
QTreeWidgetItem *item1 = jobTree->addTreeItem ( item,job->name );

  item1->setData ( 0,Qt::UserRole,QVariant::fromValue<Job*>(job) );
  item1->setExpanded ( job->expanded );

  if (!job->icon.isEmpty())
    item1->setIcon ( 0,QIcon(QString(qtCOFE_icon_base)+job->icon) );
  item1->setToolTip ( 0,job->desc );

  if (job->id==session->jobID)
    crItem = item1;
  else if ((session->jobID<0) && (job->id>maxID))  {
    maxID  = job->id;
    crItem = item1;
  }

  if (obj.contains("jobs"))  {
    QJsonArray jobs = obj.value("jobs").toArray();
    int n = jobs.count();
    for (int i=0;i<n;i++)
      addJob ( jobs[i].toObject(),item1,crItem,maxID );
  }

}


void qtCOFE::ProjectTree::makeTree ( QJsonObject & prjData )  {

  jobTree->clear();
  jobTree->setColumnCount ( 1 );
  jobTree->setSelectionBehavior ( QAbstractItemView::SelectRows );
  jobTree->setSelectionMode     ( QAbstractItemView::SingleSelection );
  jobTree->setHeaderLabels      ( QStringList() << "Project tree" );
  jobTree->setAlternatingRowColors ( true );

  if (prjData.keys().contains("jobs",Qt::CaseInsensitive))  {
    QJsonArray jobs = prjData.value("jobs").toArray();
    int  n = jobs.count();
    if (n>0)  {
      if (prjData.keys().contains("current_job",Qt::CaseInsensitive))
        session->jobID = prjData.value("current_job").toDouble();
      QTreeWidgetItem *crItem = NULL;
      int maxID = -1;
      for (int i=0;i<n;i++)
        addJob ( jobs[i].toObject(),NULL,crItem,maxID );
      if (crItem)  {
        jobTree->setCurrentItem ( crItem );
        if (session->jobID<0)
          session->jobID = maxID;
      }
    } else
      session->jobID = 0;
    del_btn->setEnabled ( session->jobID>0 );
    add_btn->setEnabled ( true );
  } else  {
    session->jobID = 0;
    add_btn->setEnabled ( false );
    del_btn->setEnabled ( false );
  }

}

QJsonObject * qtCOFE::ProjectTree::getTreeData()  {
QJsonObject *jobj   = new QJsonObject();
QJsonArray   closed;
Job         *job;
QTreeWidgetItemIterator it(jobTree);

  while (*it)  {
    if ((*it)->isExpanded())  {
      job = (*it)->data ( 0,Qt::UserRole ).value<Job*>();
      closed.append ( job->id );
    }
    ++it;
  }

  jobj->insert ( "closed_nodes",closed         );
  jobj->insert ( "current_job" ,currentJobId() );

  return jobj;

}


int qtCOFE::ProjectTree::currentJobId()  {
QTreeWidgetItem *item = jobTree->currentItem();
Job             *job;
  if (!item)  return -1;
  job = item->data ( 0,Qt::UserRole ).value<Job*>();
  if (!job)   return -2;
  return job->id;
}

int qtCOFE::ProjectTree::parentJobId()  {
QTreeWidgetItem *item = jobTree->currentItem();
Job             *job;
  if (!item)  return -1;
  item = item->parent();
  if (!item)  return -3;
  job = item->data ( 0,Qt::UserRole ).value<Job*>();
  if (!job)   return -2;
  return job->id;
}

int qtCOFE::ProjectTree::siblingJobId()  {
QTreeWidgetItem *item = jobTree->currentItem();
QTreeWidgetItem *parent;
Job             *job;
int              n;

  if (!item)    return -1;
  parent = item->parent();
  if (!parent)  return -3;

  n = parent->indexOfChild ( item );
  if (n<=0)     return -4;

  job = parent->child(n-1)->data ( 0,Qt::UserRole ).value<Job*>();
  if (!job)     return -2;

  return job->id;

}


void qtCOFE::ProjectTree::addBtnClicked()  {
  emit add_new_job ( currentJobId() );
}

void qtCOFE::ProjectTree::delBtnClicked()  {
qtCOFE::Text2Dialog *t2dlg;
QTreeWidgetItem     *item = jobTree->currentItem();
QString              prompt;
bool                 delete_job;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
                    "Please first select a job in the list." );
    return;
  }

  if (item->childCount()>0)
    prompt = "<div align='right'><i>Please confirm deletion of the "
           "selected job<br><b><u>and all its child jobs"
           "</u></b>:</i></div>";
  else
    prompt = "<div align='right'><i>Please confirm deletion of the "
             "selected job:</i></div>";

  t2dlg = new qtCOFE::Text2Dialog ( this,"Delete job" );
  t2dlg->setPrompt ( prompt );
  t2dlg->setInput1 ( "#","","" );
  t2dlg->setInput2 ( "#","","" );

  t2dlg->setOkButton     ( "Delete",qtCOFE_Ok_icon );
  t2dlg->setCancelButton ( "Cancel",qtCOFE_Cancel_icon );

  delete_job = (t2dlg->exec()==QDialog::Accepted);
  delete t2dlg;

  if (delete_job)
    deleteCurrentJob();

}

void qtCOFE::ProjectTree::deleteCurrentJob()  {
int  nextJobId = siblingJobId();
  if (nextJobId<0)
    nextJobId = parentJobId();
  emit del_job ( currentJobId(),nextJobId );
}

void qtCOFE::ProjectTree::dataBtnClicked()  {
  emit view_job_data ( currentJobId() );
}

void qtCOFE::ProjectTree::crJobChanged ( QTreeWidgetItem * current,
                                         QTreeWidgetItem * previous )  {
UNUSED_ARGUMENT(current);
UNUSED_ARGUMENT(previous);
int jid = currentJobId();

  if (jid>=0)
    session->jobID = jid;

  del_btn->setEnabled ( jid>0 );

  if (jid>=0)
    emit crjob_changed ( jid );

}

void qtCOFE::ProjectTree::getProjectedData ( QStringList & dtypes,
                                QList<QTreeWidgetItem *> & nodes )  {
  dtypes.clear();
  nodes .clear();
  addProjectedData ( jobTree->currentItem(),dtypes,nodes );
}

void qtCOFE::ProjectTree::addProjectedData (
                                  QTreeWidgetItem          * node,
                                  QStringList              & dtypes,
                                  QList<QTreeWidgetItem *> & nodes )  {
  if (node)  {
    Job *job = node->data ( 0,Qt::UserRole ).value<Job*>();
    if (job)  {
      foreach (QString d,job->dtypes)
        if ((d!="dtype_any") && (d!="dtype_dummy") &&
            (!dtypes.contains(d,Qt::CaseInsensitive))) {
          dtypes.append ( d    );
          nodes .append ( node );
        }
    }
    addProjectedData ( node->parent(),dtypes,nodes );
  }
}
