// $Id: qtcofe_dialog_task.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_task <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE task dialog class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QDialog>
#include <QScrollArea>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QScrollBar>
#include <QIcon>
#include <QMessageBox>

#include "qtcofe_dialog_task.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_defs.h"

qtCOFE::TaskDialog::TaskDialog ( QWidget           * parent,
                                 DataModel         * dm,
                                 const QStringList & dtypes,
                                 Qt::WindowFlags     f )
                   : QDialog ( parent,f )  {
  dataModel    = dm;
  setStyleSheet ( dataModel->getPreferences()->getFontSizeStyleSheet(1.0) );
  signalMapper = new QSignalMapper ( this );
  setWindowTitle ( "Tasks" );
  makeLayout ( dtypes );
//  setFixedSize(size());
  fixSize();
}

qtCOFE::TaskDialog::~TaskDialog()  {}

void qtCOFE::TaskDialog::makeLayout ( const QStringList & dtypes )  {
QVBoxLayout *vbox;
QHBoxLayout *hbox;
QGridLayout *gbox;
QWidget     *w;
QLabel      *lbl;
int          btnSize  = 3*dataModel->getPreferences()->getFontPointSize();
//QString      btnStyle = "padding:0px;margin:0px;border:0px;";
QString      btnStyle = "border:2px solid #FF0000;border-radius:6px;";
QString      lblStyle = QString ( "font-size: %1pt;" )
             .arg(9*dataModel->getPreferences()->getFontPointSize()/10);
int          r,nc,c,dkey;

  buttonMap.clear();

  gbox = new QGridLayout();

  nc = 12;
  r  = 0;
  for (int i=0;i<dataModel->sections.count();i++)  {
    TaskSection *section = dataModel->sections.at(i);
    if (section->order>=0)  {
      lbl = new QLabel ( "<i>" + section->name + "</i>" );
      lbl->setFrameStyle ( QFrame::Raised | QFrame::Panel );
      lbl->setLineWidth  ( 2 );
      gbox->addWidget ( lbl,r++,0,1,nc );
      c = 0;
      for (int j=0;j<dataModel->tasks.count();j++)  {
        Task *task = dataModel->tasks.at(j);
        if (task->section==section->id)  {
          QToolButton *btn = new QToolButton();
          btn->setIcon ( QIcon(QString(qtCOFE_icon_base)+task->icon) );
          btn->setIconSize   ( QSize(btnSize,btnSize) );
          dkey = task->hasInput ( dtypes );
          if (!dkey)
            btn->setStyleSheet ( btn->styleSheet() + btnStyle );
          buttonMap[task->type] = dkey;
          btn->setToolTip    ( task->desc );
          lbl  = new QLabel  ( task->name );
          lbl->setStyleSheet ( lblStyle   );
          vbox = new QVBoxLayout();
          vbox->addWidget ( btn,0,Qt::AlignHCenter );
          vbox->addWidget ( lbl,0,Qt::AlignHCenter );
          if (c>=nc)  {
            r++; c = 0;
          }
          gbox->addLayout ( vbox,r,c++,1,1 );
          connect ( btn,SIGNAL(clicked()), signalMapper,SLOT(map()) );
          signalMapper->setMapping ( btn,task->type );
        }
      }
      while (c<nc)  {
        w = new QWidget();
        w->setMinimumSize ( btnSize,btnSize );
        w->setMaximumSize ( btnSize,btnSize );
        vbox = new QVBoxLayout();
        vbox->addWidget ( w );
        gbox->addLayout ( vbox,r,c++,1,1 );
      }
      r++;
    }
  }

  w = new QWidget;
  w->setLayout ( gbox );
  w->setContentsMargins ( 0,0,0,0 );
  gbox->setContentsMargins ( 0,0,0,0 );

  scrollArea = new QScrollArea();
  scrollArea->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
  scrollArea->setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  );
  scrollArea->setWidgetResizable ( false );
  scrollArea->setContentsMargins ( 0,0,0,0 );
  scrollArea->horizontalScrollBar()->setEnabled ( false );
  scrollArea->setWidget ( w );

  vbox = new QVBoxLayout();
  lbl = new QLabel("Choose New Task");
  lbl->setStyleSheet (
    QString("font-weight: bold; font-style: italic; font-size: %1pt;")
           .arg(6*dataModel->getPreferences()->getFontPointSize()/5) );
  vbox->addWidget ( lbl,0 );
  vbox->addWidget ( scrollArea,100 );

  cancel_btn = new QPushButton("Cancel");
  cancel_btn->setIcon ( QIcon(qtCOFE_Cancel_icon) );
  hbox = new QHBoxLayout();
  hbox->addStretch ( 100 );
  hbox->addWidget ( cancel_btn,0 );

  vbox->addLayout ( hbox,0 );

  setLayout ( vbox );

  connect ( signalMapper,SIGNAL(mapped(const QString &)),
            this,SLOT(taskSelected(const QString &)) );

  connect ( cancel_btn,SIGNAL(clicked()),this,SLOT(reject()) );

}

void qtCOFE::TaskDialog::fixSize()  {

  scrollArea->setFixedWidth  ( scrollArea->widget()->width() +
                               scrollArea->verticalScrollBar()->width() );
  scrollArea->setFixedHeight ( 3*scrollArea->widget()->width()/5 );

}


void qtCOFE::TaskDialog::taskSelected ( const QString & type )  {
  selTaskType = type;
  accept();
}
