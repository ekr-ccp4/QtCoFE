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
                                          const QString statusIcon,
                                          const QString prompt )  {
QString          title = "<b>" + name + "</b>";
QTreeWidgetItem *item  = dataTree->addTreeItem ( "" );
  item->setFirstColumnSpanned ( true );
  item = dataTree->addTreeItem ( "" );
  if (!prompt.isEmpty())  {
    item->setFirstColumnSpanned ( true );
    title.append ( "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                   "<font size='-1'><i>(" + prompt +
                   ")</i></font>" );
  }
  dataTree->setItemWidget( item,0,new QLabel(title) );
  if (!icon.isEmpty())
    item->setIcon ( 0,QIcon(QString(qtCOFE_icon_base) + icon) );
  if (!statusIcon.isEmpty())  {
    item->setText ( 1," " );
    item->setIcon ( 1,QIcon(statusIcon) );
  }
  item->setExpanded ( true );
  return item;
}


QTreeWidgetItem * qtCOFE::DataDialog::makeRow (
                                           QTreeWidgetItem * item,
                                           QStringList     & fields,
                                           QString           jobType,
                                           bool              checked,
                                           bool              checkbox,
                                           int               nData,
                                           int               nChecked
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
      vbox->addWidget  ( cbx,0      );
      cbx->setChecked  ( i<nChecked );
//      cbx->setDisabled ( disabled );
    }
    vbox->setContentsMargins ( fsize/2,0,0,0 );
    vbox->setSpacing         ( fsize/3 );
    w = new QWidget();
    w->setLayout ( vbox );
    dataTree->setItemWidget ( item1,1,w );
    item1->parent()->setExpanded ( true );
  } else if (checked)  {
    if (nChecked==nData)
         item1->setIcon ( 1,QIcon(qtCOFE_Ok_icon)  );
    else item1->setIcon ( 1,QIcon(qtCOFE_Ok_Amber_icon)  );
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
QTreeWidgetItem                 *item;
const DataType                  *dataType;
Job                             *nodeJob;
Job                              job;
JobData::SUITABILITY             suitable;
QString                          fname,desc,jname;
int                              k,idata,nData,nChecked;
bool                             disambiguate;
bool                             missing = false;

  if (!taskType.isEmpty())
    job.copy ( dataModel->getTask(taskType) );

  dataTree->setHeaderLabels ( QStringList()  <<
                              "File"         <<
                              "Status"       <<
                              "Description"  <<
                              "Job"
                            );
  dataTree->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  dataTree->setSelectionMode     ( QAbstractItemView::SingleSelection );
  dataTree->setAlternatingRowColors ( true );

  projectTree->addProjectedData ( jobNode,projData,nodes );

  if (taskType.isEmpty())  {  // data inspector

    for (int i=0;i<projData.count();i++)  {
      dataType = dataModel->getDataType ( projData[i][0]->type );
      item     = makeSection ( dataType->name,dataType->icon,"","" );
      for (int j=0;j<projData[i].count();j++)  {
        nodeJob = nodes[i][j]->data ( 0,Qt::UserRole ).value<Job*>();
        if (nodeJob)  {
          k = nodeJob->indexOf ( projData[i][0]->type );
          if (k>=0)  {
            nodeJob->getOutputDataSpecs ( k,jname,fname,desc,nData );
            if (desc.isEmpty())
              desc = dataType->desc;
            item = makeRow ( item,
                             QStringList() <<
                               fname       <<
                               " "         <<
                               desc        <<
                               jname,
                             nodeJob->type,
                             (j==0),
                             false,
                             nData,
                             nData
                           );
          }
        }
      }
    }

  } else  {  // disambiguator

    for (int i=0;i<job.inpData.count();i++)
      if (job.isInputSuitable(i,projData)==JobData::Unsuitable)  {
        dataType = dataModel->getDataType ( job.inpData[i]->type );
        item = makeSection ( dataType->name,dataType->icon,
                             qtCOFE_Cancel_icon,"" );
        item->setText ( 2,dataType->desc );
        missing = true; // will only present stats, no disambiguation
      }

    for (int i=0;i<job.inpData.count();i++)  {
      suitable = job.isInputSuitable(i,projData);
      if (suitable!=JobData::Unsuitable)  {
        idata = indexOf ( job.inpData[i]->type,projData );
        if (idata>=0)  {
          dataType = dataModel->getDataType ( projData[idata][0]->type );
          disambiguate = (!missing) && (suitable==JobData::Ambiguous);
          if (disambiguate)  {
            switch (job.inpData[i]->mode.toAscii())  {
              default:
              case 'A':  desc = QString("Select %1 data item(s)")
                                           .arg(job.inpData[i]->nmode);
                       break;
              case 'G':  desc = QString("Select %1 or more data item(s)")
                                         .arg(job.inpData[i]->nmode+1);
                       break;
              case 'U':  desc = QString("Select up to %1 data item(s)")
                                           .arg(job.inpData[i]->nmode);
            }
          } else if (suitable==JobData::Ambiguous)
            desc = "ambiguous";
          else
            desc.clear();
          item = makeSection ( dataType->name,dataType->icon,"",desc );
          for (int j=0;j<projData[idata].count();j++)  {
            nodeJob = nodes[idata][j]->data ( 0,Qt::UserRole )
                                                        .value<Job*>();
            if (nodeJob)  {
              k = nodeJob->indexOf ( projData[idata][0]->type );
              if (k>=0)  {
                nodeJob->getOutputDataSpecs ( k,jname,fname,desc,nData );
                if (desc.isEmpty())
                  desc = dataType->desc;
                nChecked = job.inpData[i]->guessNData ( nData );
                if (suitable==JobData::Ambiguous)  {
                  if (missing)  nChecked = -1;
                }
                item = makeRow ( item,
                                 QStringList() <<
                                   fname       <<
                                   " "         <<
                                   desc        <<
                                   jname,
                                 nodeJob->type,
                                 (j==0),
                                 disambiguate,
                                 nData,
                                 nChecked
                               );
              }
            }
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
