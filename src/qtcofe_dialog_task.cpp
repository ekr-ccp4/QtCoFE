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
#include "qtcofe_dialog_data.h"
#include "qtcofe_project_tree.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_job.h"
#include "qtcofe_defs.h"

qtCOFE::TaskDialog::TaskDialog (
                           QWidget                        * parent,
                           DataModel                      * dm,
                           const QList<QList<JobData *> > & projData,
                           ProjectTree                    * jTree,
                           Qt::WindowFlags                  f )
                   : QDialog ( parent,f )  {
  dataModel    = dm;
  jobTree      = jTree;
  signalMapper = new QSignalMapper ( this );
  setStyleSheet ( dataModel->getPreferences()->
                                          getFontSizeStyleSheet(1.0) );
  setWindowTitle ( "Tasks" );
  makeLayout ( projData );
//  setFixedSize(size());
  fixSize();
}

qtCOFE::TaskDialog::~TaskDialog()  {}


#define _common_btn_style "border-radius:6px; color: black; border:2px "


void qtCOFE::TaskDialog::makeLayout (
                         const QList<QList<JobData *> > & projData )  {
Job          job;
QVBoxLayout *vbox;
QHBoxLayout *hbox;
QGridLayout *gbox;
QWidget     *w;
QLabel      *lbl;
int          btnSize   = 4*dataModel->getPreferences()->getFontPixelSize();
int          rowHeight = 7*dataModel->getPreferences()->getFontPixelSize();
QString      btnUnsuitable = _common_btn_style" solid #FF0000;";
QString      btnAmbiguous  = _common_btn_style" solid #FFAA00;";
QString      btnSuitable   = "color: black;";
QString      lblStyle = QString ( "font-size: %1pt;" ).arg (
                 9*dataModel->getPreferences()->getFontPointSize()/10);
JobData::SUITABILITY dataKey;
int          r,nc,c;

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
        job.copy ( dataModel->tasks.at(j) );
        if (job.section==section->id)  {
          QToolButton *btn = new QToolButton();
          btn->setToolTip ( job.desc );
          btn->setIcon    ( QIcon(QString(qtCOFE_icon_base)+job.icon) );
          btn->setIconSize ( QSize(btnSize-6,btnSize-6) );
          dataKey = job.isSuitable ( projData );
          switch (dataKey)  {
            case JobData::Unsuitable:
               btn->setStyleSheet ( btn->styleSheet() + btnUnsuitable );
               break;
            case JobData::Ambiguous:
               btn->setStyleSheet ( btn->styleSheet() + btnAmbiguous );
               break;
            default: ;
               btn->setStyleSheet ( btn->styleSheet() + btnSuitable );
          }
          btn->setMinimumSize ( QSize(btnSize,btnSize) );
          btn->setMaximumSize ( QSize(btnSize,btnSize) );
          buttonMap[job.type] = dataKey;
          lbl  = new QLabel  ( job.name );
          lbl->setStyleSheet ( lbl->styleSheet() + lblStyle );
          lbl->setAlignment  ( Qt::AlignHCenter );
          lbl->setWordWrap   ( true            );
          vbox = new QVBoxLayout();
          vbox->addWidget ( btn,0,Qt::AlignHCenter );
          vbox->addWidget ( lbl,0,Qt::AlignHCenter );
          if (c>=nc)  {
            gbox->setRowMinimumHeight ( r,rowHeight );
            r++; c = 0;
          }
          gbox->addLayout ( vbox,r,c++,1,1 );
          connect ( btn,SIGNAL(clicked()), signalMapper,SLOT(map()) );
          signalMapper->setMapping ( btn,job.type );
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
      gbox->setRowMinimumHeight ( r,rowHeight );
      r++;
    }
  }

  w = new QWidget;
  w->setLayout ( gbox );
  w->setContentsMargins ( 0,0,0,0 );
  gbox->setContentsMargins ( 0,0,0,0 );
  w->adjustSize();

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

//  if (getSelTaskKey()==JobData::Unsuitable)  {
  if (getSelTaskKey()!=JobData::Suitable)  {
    DataDialog *ddlg = new DataDialog ( this,
                jobTree,jobTree->currentNode(),
                type,dataModel,"Data Summary",
                "Data Summary for task '" +
                    dataModel->taskName(type) +
                       "'" );
    ddlg->resizeToData();
    ddlg->exec();
    delete ddlg;
  } else
    accept();

}
