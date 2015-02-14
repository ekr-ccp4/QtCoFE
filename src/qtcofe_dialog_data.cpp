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

#include "qtx/qtx_table.h"

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
  makeDataTable  ( projectTree,jobNode,taskType );
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

  dataTable = new qtx::Table();
  vbox->addWidget ( dataTable,100 );

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

void qtCOFE::DataDialog::makeRow ( int   & row,
                                   QString dataIcon,
                                   QString fileName,
                                   QString dataName,
                                   QString signIcon,
                                   QString metadata,
                                   QString desc,
                                   QString jobName
                                 )  {
QTableWidgetItem *item;
int               col = 0;

  if (dataTable->rowCount()<=row)
    dataTable->setRowCount ( row+1 );

  dataTable->setVertHeader ( row,QString("%1").arg(row+1) );
  item = dataTable->setTableItem  ( row,col++,fileName,_align_left );
  item->setIcon ( QIcon(QString(qtCOFE_icon_base) + dataIcon) );
  if (fileName.startsWith("<"))  {
    QFont font = item->font();
    font.setItalic ( true );
    item->setFont ( font );
  }
  dataTable->setTableItem  ( row,col++,dataName,_align_right );
  if (!signIcon.isEmpty())  {
    item = dataTable->setTableItem ( row,col++," ",Qt::AlignCenter );
    if (signIcon!="#")
        item->setIcon ( QIcon(signIcon) );
  }
  dataTable->setTableItem ( row,col++,metadata,_align_left  );
  dataTable->setTableItem ( row,col++,desc    ,_align_left  );
  dataTable->setTableItem ( row,col  ,jobName ,_align_left  );

  row++;

}

Q_DECLARE_METATYPE ( qtCOFE::Job* )

void qtCOFE::DataDialog::makeDataTable ( ProjectTree     *projectTree,
                                         QTreeWidgetItem *jobNode,
                                         QString          taskType )  {
QStringList              dtypes,dtypes0;
QList<QTreeWidgetItem *> nodes;
const DataType          *dataType;
Job                     *job;
QString                  metadata,iconPath;
int                      col,row,k;
bool                     taskData;

  if (!taskType.isEmpty())  {
    const Task *task = dataModel->getTask ( taskType );
    if (task)  dtypes0 = task->input_dtypes;
  }
  taskData = (dtypes0.count()>0);

  if (taskData)  dataTable->setColumnCount ( 6 );
           else  dataTable->setColumnCount ( 5 );
  dataTable->setRowCount    ( 10 );
  dataTable->setAlternatingRowColors ( true );

  col = 0;
  dataTable->setHorzHeader ( col++,"File"        );
  dataTable->setHorzHeader ( col++,"Type"        );
  if (taskData)
    dataTable->setHorzHeader ( col++,"Status"    );
  dataTable->setHorzHeader ( col++,"Metadata"    );
  dataTable->setHorzHeader ( col++,"Description" );
  dataTable->setHorzHeader ( col++,"Job"         );

  dataTable->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  dataTable->setSelectionMode     ( QAbstractItemView::SingleSelection );

  projectTree->addProjectedData ( jobNode,dtypes,nodes );
  row = 0;

  job = jobNode->data ( 0,Qt::UserRole ).value<Job*>();
  for (int i=0;i<dtypes0.count();i++)
    if (!dtypes.contains(dtypes0.at(i)))  {
      dataType = dataModel->getDataType ( dtypes0.at(i) );
      makeRow ( row,dataType->icon,"<< not in promise >>",dataType->name,
                qtCOFE_Cancel_icon," ",dataType->desc,job->name );
    }

  if (taskData)  iconPath = qtCOFE_Ok_icon;
           else  iconPath = "";

  for (int i=0;i<dtypes.count();i++)
    if ((!taskData) || dtypes0.contains (dtypes.at(i)))  {
      job      = nodes[i]->data ( 0,Qt::UserRole ).value<Job*>();
      dataType = dataModel->getDataType ( dtypes.at(i) );
      if (job)  {
        if (job->metadata.count()>0)  {
          k = job->dtypes.indexOf ( dtypes.at(i) );
          if (k>=0)  {
            QList<Metadata *> mlist = job->metadata.at(k);
            foreach (Metadata *m,mlist)  {
              if (m->columns.count()>0)  {
                metadata = "Columns:";
                foreach (QString c,m->columns)
                  metadata.append ( "\n" + c );
              } else
                metadata = " ";
              makeRow ( row,dataType->icon,m->fname,dataType->name,
                        iconPath,metadata,m->desc,job->name );
            }
          }
        } else  {
          makeRow ( row,dataType->icon,"<< in promise >>",dataType->name,
                    iconPath," ",dataType->desc,job->name );
        }
      }
    }

  for (int i=row;i<10;i++)  {
    dataTable->setVertHeader ( i,"    " );
    for (int j=0;j<dataTable->columnCount();j++)
      dataTable->setTableItem  ( i,j," " );
  }

  dataTable->resizeCells ();
  dataTable->setFullSize ( false,false );
  dataTable->setRowCount ( row );
  dataTable->setRowCount ( row );

}

