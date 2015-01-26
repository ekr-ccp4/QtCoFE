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

//#include <QTimer>
//#include <QTableWidgetItem>

#include "qtx/qtx_table.h"

#include "qtcofe_dialog_import.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_defs.h"

qtCOFE::DataImportDialog::DataImportDialog ( QWidget        *parent,
                                             DataModel      *dm,
                                             Qt::WindowFlags f )
                        : QDialog ( parent,f )  {
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

  dataTable = new qtx::Table();
  vbox->addWidget ( dataTable,100 );

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

  dataTable->setColumnCount ( 3  );
  dataTable->setRowCount    ( 10 );
  dataTable->setAlternatingRowColors ( true );

  dataTable->setHorzHeader ( 0,"File"        );
  dataTable->setHorzHeader ( 1,"Type"        );
  dataTable->setHorzHeader ( 2,"Description" );

  dataTable->setSelectionBehavior ( QAbstractItemView::SelectRows      );
  dataTable->setSelectionMode     ( QAbstractItemView::SingleSelection );
  for (int i=0;i<dataTable->rowCount();i++)  {
    dataTable->setVertHeader ( i,"    " );
    for (int j=0;j<dataTable->columnCount();j++)
      dataTable->setTableItem  ( i,j," " );
  }

  dataTable->setColumnWidth ( 0,dataTable->columnWidth(0)+50);
  dataTable->setColumnWidth ( 1,dataTable->columnWidth(1)+30);
  dataTable->setColumnWidth ( 2,dataTable->columnWidth(2)+150);
  dataTable->setFullSize ( false,false );
  dataTable->setRowCount ( 0 );

}

void qtCOFE::DataImportDialog::addClicked()  {
QStringList files = QFileDialog::getOpenFileNames ( this,
                                                   "Import File(s)" );
//  if (!files.isEmpty())  {
//    foreach (QString f,files)
//      s += f + ";  ";
//  }

}

void qtCOFE::DataImportDialog::delClicked()  {

}
