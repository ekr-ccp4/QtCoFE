// $Id: qtcofe_dialog_data.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_data <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE data inspector dialog
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QDialog>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

#include "qtx/qtx_tree.h"

#include "qtcofe_dialog_data.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_project_tree.h"
#include "qtcofe_job.h"
#include "qtcofe_preferences.h"
#include "qtcofe_defs.h"

qtCOFE::DataDialog::DataDialog ( QWidget         *parent,
                                 ProjectTree     *projectTree,
                                 QTreeWidgetItem *jobNode,
                                 QString          taskType,
                                 DataModel       *dm,
                                 QString          winTitle,
                                 QString          prompt,
                                 Qt::WindowFlags  f )
                  : QDialog ( parent,f )  {
  dataModel   = dm;
  preferences = dataModel->getPreferences();
  session     = dataModel->getSession    ();
  setStyleSheet  ( preferences->getFontSizeStyleSheet(1.0) );
  setWindowTitle ( winTitle );
  makeLayout     ();
  prompt_lbl->setText ( prompt + "<br>&nbsp;" );
  makeDataTree   ( projectTree,jobNode,taskType );
}

qtCOFE::DataDialog::~DataDialog()  {}

void qtCOFE::DataDialog::makeLayout()  {
QVBoxLayout *vbox;
QHBoxLayout *hbox;

  vbox = new QVBoxLayout();

  prompt_lbl = new QLabel("Data Inspector<br>&nbsp;");
  prompt_lbl->setStyleSheet (
      QString("font-weight: bold; font-style: italic; font-size: %1pt;")
            .arg(6*dataModel->getPreferences()->getFontPointSize()/5) );
  vbox->addWidget ( prompt_lbl,0 );

  dataTree = new qtx::Tree();
  vbox->addWidget ( dataTree,100 );

  hbox = new QHBoxLayout();
  close_btn = new QPushButton ( "Close" );
  close_btn->setIcon ( QIcon(qtCOFE_Close_icon) );
  hbox->addStretch ( 100 );
  hbox->addWidget  ( close_btn,0 );
  vbox->addLayout  ( hbox,0 );

  setLayout ( vbox );
  connect ( close_btn,SIGNAL(clicked()), this,SLOT(close()) );

}

#define  _align_left   (Qt::AlignLeft  | Qt::AlignVCenter)
#define  _align_right  (Qt::AlignRight | Qt::AlignVCenter)

QTreeWidgetItem * qtCOFE::DataDialog::makeSection (
                                          const QString name,
                                          const QString icon,
                                          const QString statusIcon )  {
QTreeWidgetItem *item = dataTree->addTreeItem ( "" );
  item->setFirstColumnSpanned ( true );
  item = dataTree->addTreeItem ( name );
  if (!icon.isEmpty())
    item->setIcon ( 0,QIcon(QString(qtCOFE_icon_base) + icon) );
  item->setText ( 1," " );
  if (!statusIcon.isEmpty())
    item->setIcon ( 1,QIcon(statusIcon) );
  item->setExpanded ( true );
  return item;
}


QTreeWidgetItem * qtCOFE::DataDialog::makeRow (
                                           QTreeWidgetItem * item,
                                           QStringList     & fields,
                                           QString           jobType,
                                           bool              checked,
                                           bool              checkbox,
                                           int               nData
                                              )  {
QTreeWidgetItem *item1;
QVBoxLayout     *vbox;
QCheckBox       *cbx;
QWidget         *w;
const Task      *task;
int              fsize = preferences->getFontPixelSize();

  fields.removeAll ( "" );
  item1 = new QTreeWidgetItem ( item,fields );

  if (fields[0].startsWith("<"))  {
    QFont font = item1->font(0);
    font.setItalic ( true );
    item1->setFont ( 0,font );
  }

  if (checkbox)  {
    vbox = new QVBoxLayout();
    for (int i=0;i<nData;i++)  {
      cbx = new QCheckBox();
      vbox->addWidget  ( cbx,0    );
      cbx->setChecked  ( checked  );
//      cbx->setDisabled ( disabled );
    }
    vbox->setContentsMargins ( fsize/2,0,0,0 );
    vbox->setSpacing         ( fsize/3 );
    w = new QWidget();
    w->setLayout ( vbox );
    dataTree->setItemWidget ( item1,1,w );
  } else if (checked)  {
      item1->setIcon ( 1,QIcon(qtCOFE_Ok_icon) );
    item1->parent()->setExpanded ( true );
  }

  for (int i=0;i<item1->columnCount();i++)
    item1->setTextAlignment ( i,_align_left );

  task = dataModel->getTask ( jobType );
  item1->setIcon ( item1->columnCount()-1,
                   QIcon(QString(qtCOFE_icon_base) + task->icon) );

  return item1;

}

Q_DECLARE_METATYPE ( qtCOFE::Job* )

void qtCOFE::DataDialog::makeDataTree ( ProjectTree     *projectTree,
                                        QTreeWidgetItem *jobNode,
                                        QString          taskType )  {
QList<QList<JobData *> >         projData;
QList<QList<QTreeWidgetItem *> > nodes;
QList<TaskData *>                taskData;
QList<JobData *>                 prjData;
QTreeWidgetItem                 *item;
const DataType                  *dataType;
Job                             *job;
QString                          fname,desc,jname; //,iconPath;
int                              k,brushNo;

  if (!taskType.isEmpty())  {
    const Task *task = dataModel->getTask ( taskType );
    if (task)  taskData = task->inpData;
  }

  dataTree->setHeaderLabels ( QStringList()  <<
                              "File"         <<
                              "Status"       <<
                              "Description"  <<
                              "Job"
                            );
  dataTree->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  dataTree->setSelectionMode     ( QAbstractItemView::SingleSelection );
  dataTree->setAlternatingRowColors ( true );

  // dtypes  === projData
  // dtypes0 === taskData
  projectTree->addProjectedData ( jobNode,projData,nodes );
  for (int i=0;i<projData.count();i++)
    prjData.append ( projData[i][0] );

  brushNo = 0;

  job = jobNode->data ( 0,Qt::UserRole ).value<Job*>();
  for (int i=0;i<taskData.count();i++)
    if (indexOf(taskData.at(i)->type,prjData)<0)  {
      dataType = dataModel->getDataType ( taskData.at(i)->type );
      item = makeSection ( dataType->name,dataType->icon,
                           qtCOFE_Cancel_icon );
      item->setText ( 2,dataType->desc );
      /*
      makeRow ( item,
                QStringList() <<
                  "<< not in promise >>" <<
                  " "                    <<
                  dataType->desc         <<
                  job->name,
                job->type,
                false,
                false,
                1
              );
      */
    }

//  if (!taskData.isEmpty())  iconPath = qtCOFE_Ok_icon;
//                      else  iconPath = "";

  for (int i=0;i<projData.count();i++)
    if ((taskData.isEmpty()) ||
        (indexOf(prjData.at(i)->type,taskData)>=0))  {
      dataType = dataModel->getDataType ( projData[i][0]->type );
      item     = makeSection ( dataType->name,dataType->icon,"" );
      for (int j=0;j<projData[i].count();j++)  {
        job = nodes[i][j]->data ( 0,Qt::UserRole ).value<Job*>();
        if (job)  {
          k = job->indexOf ( prjData.at(i)->type );
          if (k>=0)  {
            if (job->outData.at(k)->metadata.isEmpty())  {
              item = makeRow ( item,
                               QStringList() <<
                                 "<< in promise >>" <<
                                 " "                <<
                                 dataType->desc     <<
                                 job->name,
                               job->type,
                               (j==0),
                               false,
                               1
                             );
            } else  {
              jname = job->name;
              fname.clear();
              desc .clear();
              foreach (Metadata *m,job->outData[k]->metadata)  {
                if (!fname.isEmpty())  {
                  fname.append ( "\n" );
                  desc .append ( "\n" );
                }
                fname.append ( m->fname );
                desc .append ( m->desc  );
                if (m->columns.count()>0)  {
                  fname.append ( "/" );
                  foreach (QString c,m->columns)
                    fname.append ( c + ":" );
                  fname.resize ( fname.size()-1 );
                }
              }
              item = makeRow ( item,
                               QStringList() <<
                                 fname       <<
                                 " "         <<
                                 desc        <<
                                 jname,
                               job->type,
                               (j==0),
                               false,
                               job->outData[k]->metadata.count()
                             );
            }
          }
        }
      }
    }

}

void qtCOFE::DataDialog::resizeToData()  {
  dataTree->setFullWidth();
  QSize s = dataTree->size();
  s.setWidth  ( qMin(s.width (),700) );
  s.setHeight ( qMin(s.height(),400) );
  resize ( s );
}
