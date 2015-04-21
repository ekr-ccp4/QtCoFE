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
#include <QLabel>
#include <QProgressBar>

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

QToolButton *makeButton ( const QString icon,
                          const QString toolTip,
                          const QString & btnStyle,
                          const int     btnSize )  {
QToolButton *btn = new QToolButton();
  btn->setIcon        ( QIcon(icon) );
  btn->setIconSize    ( QSize(btnSize,btnSize) );
  btn->setStyleSheet  ( btnStyle );
  btn->setToolTip     ( toolTip  );
  return btn;
}

void qtCOFE::ProjectTree::makeLayout()  {
QVBoxLayout *vbox = new QVBoxLayout();
QHBoxLayout *hbox = new QHBoxLayout();
QString      bs = preferences->getToolButtonStyle();
int          bz = preferences->getToolButtonSize ();

  jobTree = new qtx::Tree();
  jobTree->setColumnCount ( 1 );
  jobTree->setSelectionBehavior ( QAbstractItemView::SelectRows );
  jobTree->setSelectionMode     ( QAbstractItemView::SingleSelection );
  jobTree->setHeaderLabel       ( "Project tree" );
  jobTree->setAlternatingRowColors ( true );
  jobTree->setStyle ( new QWindowsStyle() );
  vbox->addWidget ( jobTree,100 );

  add_btn  = makeButton ( qtCOFE_Plus_icon ,"Add job"       ,bs,bz );
  del_btn  = makeButton ( qtCOFE_Minus_icon,"Delete job"    ,bs,bz );
  prm_btn  = makeButton ( qtCOFE_Param_icon,"Job parameters",bs,bz );
  run_btn  = makeButton ( qtCOFE_Run_icon  ,"Run job"       ,bs,bz );
  view_btn = makeButton ( qtCOFE_View_icon ,"View results"  ,bs,bz );
  data_btn = makeButton ( qtCOFE_Data_icon,"Data inspector" ,bs,bz );

  hbox->addWidget  ( add_btn ,1 );
  hbox->addWidget  ( del_btn ,1 );
  hbox->addStretch ( 100 );
  hbox->addWidget  ( prm_btn ,1 );
  hbox->addWidget  ( run_btn ,1 );
  hbox->addWidget  ( view_btn,1 );
  hbox->addStretch ( 100 );
  hbox->addWidget  ( data_btn,1 );

  hbox->setContentsMargins ( 0,0,0,0 );
  hbox->setSpacing ( 4 );

  vbox->addLayout ( hbox,0 );
  vbox->setSpacing ( 4 );

  setLayout ( vbox );

  connect ( add_btn ,SIGNAL(clicked()),this,SLOT(addBtnClicked())  );
  connect ( del_btn ,SIGNAL(clicked()),this,SLOT(delBtnClicked())  );
  connect ( prm_btn ,SIGNAL(clicked()),this,SLOT(prmBtnClicked())  );
  connect ( run_btn ,SIGNAL(clicked()),this,SLOT(runBtnClicked())  );
  connect ( view_btn,SIGNAL(clicked()),this,SLOT(viewBtnClicked()) );
  connect ( data_btn,SIGNAL(clicked()),this,SLOT(dataBtnClicked()) );

  connect ( jobTree,
      SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
      this,SLOT(crJobChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );

}

Q_DECLARE_METATYPE ( qtCOFE::Job* )
Q_DECLARE_METATYPE ( QLabel* )
Q_DECLARE_METATYPE ( QProgressBar* )

void qtCOFE::ProjectTree::addJob ( const QJsonObject   & obj,
                                   QTreeWidgetItem     *item,
                                   QTreeWidgetItem *& crItem,
                                   int              & maxID )  {
Job             *job   = new Job(obj,dataModel);
QTreeWidgetItem *item1 = jobTree->addTreeItem ( item,"" );
QHBoxLayout     *hbox  = new QHBoxLayout();
QWidget         *w     = new QWidget();
QProgressBar    *pbar;
QLabel          *lbl;
int              charHeight;

  item1->setData ( 0,Qt::UserRole,QVariant::fromValue<Job*>(job) );
  item1->setExpanded ( job->expanded );

  if (!job->icon.isEmpty())  {
    lbl = new QLabel("");
    charHeight = (preferences->getToolButtonSize() +
                  preferences->getFontHeight()) / 2;
    lbl->setPixmap ( QPixmap(QString(qtCOFE_icon_base)+job->icon)
                     .scaledToHeight(charHeight,Qt::SmoothTransformation) );
    lbl->setStyleSheet("QLabel{background-color: white; color: back;}");
    hbox->addWidget ( lbl,0 );
    item1->setData ( 0,Qt::UserRole+1,QVariant::fromValue<QLabel*>(lbl) );
  }
  lbl = new QLabel(" " + job->name + " " );
  lbl->setStyleSheet("QLabel{background-color: transparent; color: back;}");
  hbox->addWidget ( lbl,0 );
  item1->setData ( 0,Qt::UserRole+2,QVariant::fromValue<QLabel*>(lbl) );

  pbar = new QProgressBar();
  pbar->setRange(0,0);
  charHeight = preferences->getFontHeight();
  pbar->setMinimumSize(10*charHeight,charHeight);
  pbar->setMaximumSize(10*charHeight,charHeight);
  hbox->addWidget(pbar,0);
  item1->setData ( 0,Qt::UserRole+3,
                     QVariant::fromValue<QProgressBar*>(pbar) );

  hbox->addStretch ( 100 );
  hbox->setSpacing ( 0   );
  hbox->setContentsMargins ( 0,1,0,1 );
  w->setLayout ( hbox );
  jobTree->setItemWidget ( item1,0,w);
  job->setProgressBar ( pbar );

//  if (!job->icon.isEmpty())
//    item1->setIcon ( 0,QIcon(QString(qtCOFE_icon_base)+job->icon) );
  item1->setToolTip   ( 0,job->desc     );
  item1->setTextColor ( 0,QColor(0,0,0) );

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


bool qtCOFE::ProjectTree::updateJob ( const QJsonObject     & obj,
                                 QVector<QTreeWidgetItem *> & items,
                                 QVector<int>               & job_ids
                                    )  {
Job *job = new Job(obj,dataModel);
Job *jobn;
int  n = job_ids.indexOf ( job->id );
bool updated;

  if (n<0)  {
    delete job;
    return false;
  }

  jobn = items[n]->data ( 0,Qt::UserRole ).value<Job*>();
  if (!jobn)  {
    delete job;
    return false;
  }

  items[n]->setData ( 0,Qt::UserRole,QVariant::fromValue<Job*>(job) );
  job->copyTreeData ( jobn );
  job->showStatus();

  delete jobn;
  updated = true;

  if (obj.contains("jobs"))  {
    QJsonArray jobs = obj.value("jobs").toArray();
    int nc = jobs.count();
    for (int i=0;(i<nc) && updated;i++)
      updated = updateJob ( jobs[i].toObject(),items,job_ids );
  }

  return updated;

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
//    del_btn->setEnabled ( session->jobID>0 );
//    add_btn->setEnabled ( true );
  } else  {
    session->jobID = 0;
//    add_btn->setEnabled ( false );
//    del_btn->setEnabled ( false );
  }
  enableControls();

}

void qtCOFE::ProjectTree::updateTree ( QJsonObject & prjData )  {
QTreeWidgetItemIterator    it(jobTree);
QVector<QTreeWidgetItem *> items;
QVector<int>               job_ids;
Job                       *job;
int                        njobs;
bool                       updated;

  if (prjData.keys().contains("jobs",Qt::CaseInsensitive))  {

    QJsonArray jobs = prjData.value("jobs").toArray();
    njobs = jobs.count();

    while (*it) {
      items.append ( *it );
      job = (*it)->data ( 0,Qt::UserRole ).value<Job*>();
      if (job)  job_ids.append ( job->id );
          else  job_ids.append ( -1      );
      ++it;
    }

    updated = (items.count()>0);
    for (int i=0;(i<njobs) && updated;i++)
      updated = updateJob ( jobs[i].toObject(),items,job_ids );
    if (!updated)
          makeTree ( prjData );
    else  enableControls();

  } else
    makeTree ( prjData );

}

void qtCOFE::ProjectTree::clearTree()  {
  jobTree->clear();
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


qtCOFE::Job *qtCOFE::ProjectTree::currentJob()  {
QTreeWidgetItem *item = jobTree->currentItem();
Job             *job;
  if (!item)  return NULL;
  job = item->data ( 0,Qt::UserRole ).value<Job*>();
  return job;
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

QTreeWidgetItem *qtCOFE::ProjectTree::currentNode()  {
  if (jobTree)  return jobTree->currentItem();
  return NULL;
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

void qtCOFE::ProjectTree::prmBtnClicked()  {
  QMessageBox::information ( this,"Not implemented","Not implemented" );
}

void qtCOFE::ProjectTree::runBtnClicked()  {
  emit run_job ( currentJobId() );
//  QMessageBox::information ( this,"Not implemented","Not implemented" );
}

void qtCOFE::ProjectTree::viewBtnClicked()  {
  emit view_report ( currentJobId() );
//  QMessageBox::information ( this,"Not implemented","Not implemented" );
}

void qtCOFE::ProjectTree::dataBtnClicked()  {
  emit view_job_data ( currentJobId() );
}

void qtCOFE::ProjectTree::enableControls()  {
Job *job = currentJob();
bool runnable,running,done,root,children;

  if (job)  {
    done     = (job->status>=qtCOFE_JOB_Done);
    running  = (qtCOFE_JOB_Starting<=job->status) &&
               (job->status<qtCOFE_JOB_Done);
    root     = (job->id<=0);
    children = (jobTree->currentItem()->childCount()>0);
    runnable = (!root) && (job->type!=qtCOFE_TASK_DataImport) &&
               (job->type!=qtCOFE_TASK_Disambiguator);

    add_btn ->setEnabled ( done || root );
    del_btn ->setEnabled ( (!root) && (!running) );
    prm_btn ->setEnabled ( runnable     );
    run_btn ->setEnabled ( runnable && (!running) &&
                           ((!done) || (!children)) );
    view_btn->setEnabled ( running || done );
    data_btn->setEnabled ( !root );

  } else  {
    add_btn ->setEnabled ( false );
    del_btn ->setEnabled ( false );
    prm_btn ->setEnabled ( false );
    run_btn ->setEnabled ( false );
    view_btn->setEnabled ( false );
    data_btn->setEnabled ( false );
  }

}


void qtCOFE::ProjectTree::crJobChanged ( QTreeWidgetItem * current,
                                         QTreeWidgetItem * previous )  {
QLabel *lbl;
int     jid = currentJobId();

  if (previous)  {
    lbl = previous->data ( 0,Qt::UserRole+1 ).value<QLabel*>();
    lbl->setStyleSheet ( "QLabel{background-color: white; color: black;}" );
    lbl = previous->data ( 0,Qt::UserRole+2 ).value<QLabel*>();
    lbl->setStyleSheet ( "QLabel{background-color: transparent; color: black;}" );
  }
  if (current)  {
    lbl = current->data ( 0,Qt::UserRole+1 ).value<QLabel*>();
    lbl->setStyleSheet ( "QLabel{background-color: white; color: white;}" );
    lbl = current->data ( 0,Qt::UserRole+2 ).value<QLabel*>();
    lbl->setStyleSheet ( "QLabel{background-color: transparent; color: white;}" );
  }

  if (jid>=0)
    session->jobID = jid;

  enableControls();

  if (jid>=0)
    emit crjob_changed ( jid );

}

void qtCOFE::ProjectTree::getProjectedData (
                          QList<QList<JobData *> >         & projData,
                          QList<QList<QTreeWidgetItem *> > & nodes )  {
// projData[i][*] corresponds to a particular data type, to be found by
// querying projData[i][0]->type. 2nd index counts up the tree branch
// from current node. nodes[i][j] stores QTreeWidgetItem corresponding
// to projData[i][j].
  projData.clear();
  nodes   .clear();
  addProjectedData ( jobTree->currentItem(),projData,nodes );
}

void qtCOFE::ProjectTree::addProjectedData (
                          QTreeWidgetItem                  * node,
                          QList<QList<JobData *> >         & projData,
                          QList<QList<QTreeWidgetItem *> > & nodes )  {
// projData[i][*] corresponds to a particular data type, to be found by
// querying projData[i][0]->type. 2nd index counts up the tree branch
// from 'node'. nodes[i][j] stores QTreeWidgetItem corresponding to
// projData[i][j].
int i,k;
  if (node)  {
    Job *job = node->data ( 0,Qt::UserRole ).value<Job*>();
    if (job)  {
      foreach (JobData *jd,job->outData)
        if ((jd->type!="dtype_any") && (jd->type!="dtype_dummy"))  {
          k = -1;
          for (i=0;(i<projData.count()) && (k<0);i++)
            if (projData[i][0]->type==jd->type)  {
              k = i;
              projData[i].append ( jd   );
              nodes   [i].append ( node );
            }
          if (k<0)  {
            projData.append ( QList<JobData *>()         << jd   );
            nodes   .append ( QList<QTreeWidgetItem *>() << node );
          }
        }
    }
    addProjectedData ( node->parent(),projData,nodes );
  }
}
