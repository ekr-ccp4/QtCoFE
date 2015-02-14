// $Id: qtcofe_dialog_import.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_import <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE data import dialog
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
//#include <QApplication>

//#include <QTimer>
//#include <QTableWidgetItem>

//#include "qjson/QJsonDocument.h"
//#include "qjson/QJsonParseError.h"
#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"
#include "qtx/qtx_table.h"

#include "qtcofe_dialog_import.h"
#include "qtcofe_server.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_defs.h"

qtCOFE::DataImportDialog::DataImportDialog ( QWidget        *parent,
                                             int             jobID,
                                             DataModel      *dm,
                                             Qt::WindowFlags f )
                        : QDialog ( parent,f )  {
  JobID       = jobID;
  dataModel   = dm;
  preferences = dataModel->getPreferences();
  session     = dataModel->getSession    ();
  setStyleSheet ( preferences->getFontSizeStyleSheet(1.0) );
  setWindowTitle ( tr("Data import") );
  makeLayout();
}

qtCOFE::DataImportDialog::~DataImportDialog()  {}


void qtCOFE::DataImportDialog::makeLayout()  {
QVBoxLayout *vbox;
QGridLayout *gbox;
QLabel      *lbl;
QString      btnStyle = preferences->getToolButtonStyle();
int          btnSize  = preferences->getToolButtonSize ();

  vbox = new QVBoxLayout();

  lbl = new QLabel("Imported data<br>&nbsp;");
  lbl->setStyleSheet (
      QString("font-weight: bold; font-style: italic; font-size: %1pt;")
          .arg(6*dataModel->getPreferences()->getFontPointSize()/5) );
  vbox->addWidget ( lbl,0 );

  importTable = new qtx::Table();
  vbox->addWidget ( importTable,100 );

  gbox    = new QGridLayout();
  add_btn = new QToolButton();
  add_btn->setIcon       ( QIcon(qtCOFE_Plus_icon) );
  add_btn->setIconSize   ( QSize(btnSize,btnSize) );
  add_btn->setStyleSheet ( btnStyle );
  gbox->addWidget        ( add_btn,0,0,1,1,Qt::AlignTop );
  del_btn = new QToolButton();
  del_btn->setIcon       ( QIcon(qtCOFE_Minus_icon) );
  del_btn->setIconSize   ( QSize(btnSize,btnSize) );
  del_btn->setStyleSheet ( btnStyle );
  gbox->addWidget        ( del_btn,0,1,1,1,Qt::AlignTop );
  gbox->addWidget        ( new QLabel(" "),0,2,1,1 );

  connect ( add_btn,SIGNAL(clicked()), this,SLOT(addClicked()) );
  connect ( del_btn,SIGNAL(clicked()), this,SLOT(delClicked()) );

  gbox->addWidget        ( new QLabel(" "),1,1,1,3 );
  close_btn = new QPushButton ( "Close" );
  close_btn->setIcon     ( QIcon(qtCOFE_Close_icon) );
  gbox->addWidget        ( close_btn,0,3,2,1,Qt::AlignBottom );
  connect ( close_btn,SIGNAL(clicked()), this,SLOT(close()) );

  gbox->setColumnStretch ( 0,0 );
  gbox->setColumnStretch ( 1,0 );
  gbox->setColumnStretch ( 2,100 );
  gbox->setColumnStretch ( 3,0 );

  gbox->setContentsMargins ( 0,0,0,0 );
  gbox->setVerticalSpacing ( 0 );

  vbox->addLayout  ( gbox,0 );
  vbox->setSpacing ( 0 );

  setLayout ( vbox );

//  QTimer::singleShot ( 0,this,SLOT(makeEmptyTable()) );
  makeEmptyTable();

}

void qtCOFE::DataImportDialog::makeEmptyTable()  {

  importTable->setColumnCount ( 4  );
  importTable->setRowCount    ( 10 );
  importTable->setAlternatingRowColors ( true );

  importTable->setHorzHeader ( 0,"File"        );
  importTable->setHorzHeader ( 1,"Type"        );
  importTable->setHorzHeader ( 2,"Metadata"    );
  importTable->setHorzHeader ( 3,"Description" );

  importTable->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  importTable->setSelectionMode     ( QAbstractItemView::SingleSelection );
  for (int i=0;i<importTable->rowCount();i++)  {
    importTable->setVertHeader ( i,"    " );
    for (int j=0;j<importTable->columnCount();j++)
      importTable->setTableItem  ( i,j," " );
  }

  importTable->setColumnWidth ( 0,importTable->columnWidth(0)+30 );
  importTable->setColumnWidth ( 1,importTable->columnWidth(1)+20 );
  importTable->setColumnWidth ( 2,importTable->columnWidth(2)+10 );
  importTable->setColumnWidth ( 3,importTable->columnWidth(3)+80 );
  importTable->setFullSize ( false,false );
  importTable->setRowCount ( 0 );

}

void qtCOFE::DataImportDialog::makeImportTable (
                                     const QJsonObject & jobData )  {
QJsonArray        data = jobData.value("data").toArray();
QTableWidgetItem *item;
//QJsonParseError   e;
QString           iconPath;
QString           metadata;
int               row  = 0;
int               row0 = qMax(data.count(),10);

  importTable->setRowCount ( row0 );

  for (int i=0;i<importTable->rowCount();i++)  {
    importTable->setVertHeader ( i,"    " );
    for (int j=0;j<importTable->columnCount();j++)
      importTable->setTableItem  ( i,j," " );
  }

  for (int i=0;i<data.count();i++)  {
    QJsonArray dlist = data[data.count()-i-1].toArray();
    for (int j=0;j<dlist.count();j++)  {
//      QJsonObject jd = QJsonDocument::fromJson (
//                           dlist[j].toString().toAscii(),&e ).object();
      QJsonObject jd = dlist[j].toObject();
      if (row>=row0)  importTable->setRowCount ( row+1 );
      importTable->setVertHeader ( row,QString("%1").arg(row+1) );
      item = importTable->setTableItem  ( row,0,
                                   jd.value("file").toString(),
                                   Qt::AlignLeft | Qt::AlignVCenter );
      iconPath = jd.value("icon").toString();
      if (!iconPath.isEmpty())
        item->setIcon ( QIcon(QString(qtCOFE_icon_base)+iconPath) );
      importTable->setTableItem  ( row,1,jd.value("name").toString(),
                                   Qt::AlignRight | Qt::AlignVCenter );
      if (!jd.keys().contains("columns",Qt::CaseInsensitive))
        metadata = " ";
      else  {
        metadata = "Columns:";
        QJsonArray clist = jd.value("columns").toArray();
        for (int k=0;k<clist.count();k++)
          metadata.append ( " \n" + clist[k].toString() );
      }
//      QLabel *lbl = new QLabel ( metadata );
//      importTable->setCellWidget ( row,2,lbl );
      importTable->setTableItem  ( row,2,metadata,
                                   Qt::AlignLeft | Qt::AlignVCenter );
      importTable->setTableItem  ( row,3,jd.value("desc").toString(),
                                   Qt::AlignLeft | Qt::AlignVCenter );
      row++;
    }
  }
  importTable->resizeCells ();
  importTable->setFullSize ( false,false );
  importTable->setRowCount ( row );

  adjustSize();

}

int qtCOFE::DataImportDialog::importCount()  {
  if (importTable)  return importTable->rowCount();
  return 0;
}

void qtCOFE::DataImportDialog::importFile ( const QString & fpath )  {

Server      server(preferences,session,this);
QJsonObject jsonData;
QJsonObject jsonReply;
SERVER_RC   rc;

  jsonData.insert ( "job_id"   ,JobID );
  jsonData.insert ( "file_path",fpath );
  rc = server.call ( qtCOFE_SERVER_ACT_ImportFile,jsonData,jsonReply );

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
      if (!jsonReply.keys().contains("imports",Qt::CaseInsensitive))
        message.append ( "<p>List of imports could not be obtained." );
      QMessageBox::information ( this,"Error",message );
    }
    makeImportTable ( jsonReply.value("job").toObject() );
  }

}


void qtCOFE::DataImportDialog::addClicked()  {
QStringList files = QFileDialog::getOpenFileNames ( this,
                                                   "Import File(s)" );
  if (!files.isEmpty())  {
    foreach (QString f,files)
      importFile ( f );
  }

}

void qtCOFE::DataImportDialog::delClicked()  {

}
