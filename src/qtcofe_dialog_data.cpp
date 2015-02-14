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
                                 DataModel       *dm,
                                 Qt::WindowFlags f )
                  : QDialog ( parent,f )  {
  dataModel   = dm;
  preferences = dataModel->getPreferences();
  session     = dataModel->getSession    ();
  setStyleSheet  ( preferences->getFontSizeStyleSheet(1.0) );
  setWindowTitle ( tr("Data Inspector") );
  makeInspectorLayout ();
  makeInspectorTable  ( projectTree,jobNode );
}

qtCOFE::DataDialog::~DataDialog()  {}

void qtCOFE::DataDialog::makeInspectorLayout()  {
QVBoxLayout *vbox;
QHBoxLayout *hbox;
QLabel      *lbl;

  vbox = new QVBoxLayout();

  lbl = new QLabel("Data Inspector<br>&nbsp;");
  lbl->setStyleSheet (
      QString("font-weight: bold; font-style: italic; font-size: %1pt;")
            .arg(6*dataModel->getPreferences()->getFontPointSize()/5) );
  vbox->addWidget ( lbl,0 );

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


Q_DECLARE_METATYPE ( qtCOFE::Job* )

void qtCOFE::DataDialog::makeInspectorTable (
                                        ProjectTree     *projectTree,
                                        QTreeWidgetItem *jobNode )  {
QStringList              dtypes;
QList<QTreeWidgetItem *> nodes;
const DataType          *dataType;
Job                     *job;
QString                  metadata;
int                      row,k;

  dataTable->setColumnCount ( 5  );
  dataTable->setRowCount    ( 10 );
  dataTable->setAlternatingRowColors ( true );

  dataTable->setHorzHeader ( 0,"File"        );
  dataTable->setHorzHeader ( 1,"Type"        );
  dataTable->setHorzHeader ( 2,"Metadata"    );
  dataTable->setHorzHeader ( 3,"Description" );
  dataTable->setHorzHeader ( 4,"Job"         );

  dataTable->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  dataTable->setSelectionMode     ( QAbstractItemView::SingleSelection );

  projectTree->addProjectedData ( jobNode,dtypes,nodes );

  row = 0;
  for (int i=0;i<dtypes.count();i++)  {
    job      = nodes[i]->data ( 0,Qt::UserRole ).value<Job*>();
    dataType = dataModel->getDataType ( dtypes.at(i) );
    if (job)  {
      if (job->metadata.count()>0)  {
        k = job->dtypes.indexOf ( dtypes.at(i) );
        if (k>=0)  {
          QList<Metadata *> mlist = job->metadata.at(k);
          if (dataTable->rowCount()<=row+mlist.count())
            dataTable->setRowCount ( row+1+mlist.count() );
          foreach (Metadata *m,mlist)  {
            dataTable->setVertHeader ( row,QString("%1").arg(row+1) );
            dataTable->setTableItem  ( row,0,m->fname,
               Qt::AlignLeft | Qt::AlignVCenter )->setIcon (
                   QIcon(QString(qtCOFE_icon_base) + dataType->icon) );
            dataTable->setTableItem  ( row,1,dataType->name,
                                   Qt::AlignRight | Qt::AlignVCenter );
            if (m->columns.count()>0)  {
              metadata = "Columns:";
              foreach (QString c,m->columns)
                metadata.append ( "\n" + c );
            } else
              metadata = " ";
            dataTable->setTableItem  ( row,2,metadata,
                                        Qt::AlignLeft | Qt::AlignVCenter );
            dataTable->setTableItem  ( row,3,m->desc,
                                        Qt::AlignLeft | Qt::AlignVCenter );
            dataTable->setTableItem  ( row,4,job->name,
                                        Qt::AlignLeft | Qt::AlignVCenter );
            row++;
          }
        }
      } else  {
        if (dataTable->rowCount()<=row)
          dataTable->setRowCount ( row+1 );
        dataTable->setVertHeader ( row,QString("%1").arg(row+1) );
        dataTable->setTableItem  ( row,0,"promise",
             Qt::AlignLeft | Qt::AlignVCenter )->setIcon (
                   QIcon(QString(qtCOFE_icon_base) + dataType->icon) );
        dataTable->setTableItem  ( row,1,dataType->name,
                                    Qt::AlignRight | Qt::AlignVCenter );
        dataTable->setTableItem  ( row,2," ",
                                    Qt::AlignLeft | Qt::AlignVCenter );
        dataTable->setTableItem  ( row,3,dataType->desc,
                                    Qt::AlignLeft | Qt::AlignVCenter );
        dataTable->setTableItem  ( row,4,job->name,
                                    Qt::AlignLeft | Qt::AlignVCenter );
        row++;
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

