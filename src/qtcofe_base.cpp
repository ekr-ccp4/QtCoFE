// $Id: qtcofe_base.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_base <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Base
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QSettings>
#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QTimer>

#include "qtcofe_base.h"
#include "qtcofe_preferences.h"
#include "qtcofe_session.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_defs.h"

qtCOFE::Base::Base ( QWidget *parent, Qt::WindowFlags flags )
            : QMainWindow(parent,flags)  {

  setObjectName ( "qtCOFE_Base" );

  setUnifiedTitleAndToolBarOnMac(true);

  setWindowTitle ( tr(qtCOFE_app_spec)    );
  setWindowIcon  ( QIcon(qtCOFE_app_icon) );

  setAcceptDrops ( true );

  prjListAct     = NULL;
  accountAct     = NULL;
  unlockAct      = NULL;
  preferencesAct = NULL;
  helpAct        = NULL;
  exitAct        = NULL;
  aboutAct       = NULL;
  aboutQtAct     = NULL;

  fileMenu       = NULL;
  helpMenu       = NULL;

  preferences = new qtCOFE::Preferences();
  session     = new qtCOFE::Session();
  settings    = new QSettings ( qtCOFE_Company,qtCOFE_Application );
  dataModel   = new DataModel ( preferences,session );

  readSettings();

//  setStyleSheet ( QString ( "font-size: %1pt;")
//                      .arg(6+preferences->getFontPointSize()/2) );
  setStyleSheet ( preferences->getFontSizeStyleSheet(1.0) );

  createActions  ();
  createMenus    ();
  createToolBar  ();
  createStatusBar();

  connect ( preferences,SIGNAL(applyPreferences()),
            this,SLOT(preferencesChanged()) );

  QTimer::singleShot ( 10,this,SLOT(readModel()) );

}

qtCOFE::Base::~Base() {

  if (settings)    delete settings;
  if (preferences) delete preferences;

}

void qtCOFE::Base::readModel() {
int rc = dataModel->readModel();

  if (rc)  {
    QMessageBox::critical ( this,"No data model",
               QString("Data Model could not be retrieved (%1)<p>"
                       "<b><i>-- STOP</i></b>").arg(rc) );
    QTimer::singleShot ( 100,this,SLOT(close()) );
  }

}

QSize qtCOFE::Base::minimumSizeHint() const  {
  return QSize(200,150);
}

QSize qtCOFE::Base::sizeHint() const  {
  return QSize(700,800);
}


void qtCOFE::Base::readSettings()  {

  restoreState ( settings->value(qtCOFE_SET_WinState).toByteArray() );
  resize ( settings->value(qtCOFE_SET_WinSize,QSize(700,800)).toSize() );
  move   ( settings->value(qtCOFE_SET_WinPos,QPoint(100,50)).toPoint() );

  preferences->readSettings ( settings );
  session    ->readSettings ( settings );

}

void qtCOFE::Base::writeSettings()  {

  settings->setValue ( qtCOFE_SET_WinState ,saveState()   );
  settings->setValue ( qtCOFE_SET_WinSize  ,size     ()   );
  settings->setValue ( qtCOFE_SET_WinPos   ,pos      ()   );

  preferences->writeSettings ( settings );
  session    ->writeSettings ( settings );

  settings->sync();

}

void qtCOFE::Base::closeEvent ( QCloseEvent *event )  {
//  if (maybeSave()) {
  if (!session->keepLogged)
    session->loggedIn = false;
  writeSettings();
  QMainWindow::closeEvent ( event );
//    event->accept();
//  } else {
//    event->ignore();
//  }
}


void qtCOFE::Base::createAction (
              QAction      *& action,
              const char    * iconPath,
              const char    * menuName,
              const char    * toolbarName,
              QKeySequence::StandardKey hotKey,
              const char    * statusTip,
              const QObject * receiver,
              const char    * method,
              Qt::ConnectionType connect_type
                                  )  {
  action = new QAction ( this );

  if (iconPath)
    action->setIcon ( QIcon(iconPath) );
  if (menuName)    action->setText      ( tr(menuName)    );
  if (toolbarName) action->setIconText  ( tr(toolbarName) );
  if (statusTip)   action->setStatusTip ( tr(statusTip)   );
  action->setShortcuts ( hotKey );

  if (receiver)
    connect ( action,SIGNAL(triggered()),
              receiver,method,connect_type );

}

void qtCOFE::Base::createAction (
              QAction      *& action,
              const char    * iconPath,
              const char    * menuName,
              const char    * toolbarName,
              const char    * hotKey,
              const char    * statusTip,
              const QObject * receiver,
              const char    * method,
              Qt::ConnectionType connect_type
                                  )  {
  action = new QAction ( this );

  if (iconPath)
    action->setIcon ( QIcon(iconPath) );
  if (menuName)    action->setText      ( tr(menuName)    );
  if (toolbarName) action->setIconText  ( tr(toolbarName) );
  if (statusTip)   action->setStatusTip ( tr(statusTip)   );
  if (hotKey)      action->setShortcut  ( tr(hotKey)      );

  if (receiver)
    connect ( action,SIGNAL(triggered()),
              receiver,method,connect_type );

}

void qtCOFE::Base::createActions()  {

  createAction ( prjListAct,qtCOFE_prjlist_icon,"My pro&jects",
                 "My projects","Shift+Alt+J",
                 "Manage list of projects",
                 this,SLOT(projectList()) );

  createAction ( accountAct,qtCOFE_account_icon,"My &account",
                 "My account","Shift+Alt+A",
                 "Edit user data",
                 this,SLOT(account()) );

  createAction ( unlockAct,qtCOFE_unlock_icon,"&Unlock",
                 "Unlock","Shift+Alt+U",
                 "Unlock project when it is locked in error",
                 this,SLOT(unlock()) );

  createAction ( preferencesAct,qtCOFE_configure_icon,"&Preferences",
                 "Preferences",QKeySequence::Preferences,
                 "Edit the application's configuration",
                 this,SLOT(configure()) );

  createAction ( helpAct,qtCOFE_help_icon,"Help",
                 "Help","Shift+Alt+H","Help",
                 this,SLOT(help()) );

  createAction ( exitAct,qtCOFE_exit_icon,"E&xit","Exit",
                 QKeySequence::Quit,"Exit the application",
                 this,SLOT(close()) );

  createAction ( logoutAct,qtCOFE_logout_icon,"Logo&ut","Logout",
                 "Shift+Alt+U","Logout current user",
                 this,SLOT(logout()) );
  logoutAct->setVisible ( false );

  createAction ( aboutAct,NULL,"&About","About qtRView",
                 NULL,"Show the application's About box",
                 this,SLOT(about()) );

  createAction ( aboutQtAct,NULL,"About &Qt",NULL,
                 NULL,"Show the Qt library's About box",
                 qApp,SLOT(aboutQt()) );

}

void qtCOFE::Base::createMenus()  {

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction    ( prjListAct     );
  fileMenu->addAction    ( accountAct     );
  fileMenu->addAction    ( unlockAct      );
  fileMenu->addAction    ( preferencesAct );
  fileMenu->addSeparator ();
  fileMenu->addAction    ( logoutAct );
  fileMenu->addAction    ( exitAct   );

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction ( aboutAct   );
  helpMenu->addAction ( aboutQtAct );
  helpMenu->addSeparator();
  helpMenu->addAction ( helpAct    );

}

void qtCOFE::Base::createToolBar()  {
QWidget *w;

  toolBar = addToolBar        ( tr("Main") );
  toolBar->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
  toolBar->setObjectName      ( "qtCOFE_Toolbar" );

  toolBar->addAction ( prjListAct     );
  toolBar->addAction ( accountAct     );
  toolBar->addAction ( unlockAct      );
  toolBar->addAction ( preferencesAct );
  toolBar->addAction ( helpAct        );

  w = new QWidget();
  w->setSizePolicy ( QSizePolicy::Expanding,QSizePolicy::Minimum );
  toolBar->addWidget ( w );
  toolBar->addAction ( logoutAct      );
  toolBar->addAction ( exitAct        );
/*

  QMenu *menu = new QMenu();
  QAction *testAction = new QAction("test menu item", this);
  menu->addAction(testAction);

  QToolButton* toolButton = new QToolButton();
  toolButton->setMenu(menu);
  toolButton->setPopupMode(QToolButton::InstantPopup);

  QWidgetAction* toolButtonAction = new QWidgetAction(this);
  toolButtonAction->setDefaultWidget(toolButton);

  toolBar->addAction(toolButtonAction);

*/


  // this makes sure that button tooltips do show (????)
  toolBar->setStyleSheet ( toolBar->styleSheet() + "color: black;" );

  toolBar->show();

}

void qtCOFE::Base::createStatusBar()  {
  statusBar()->showMessage(tr("Ready"));
}


void qtCOFE::Base::about()  {
QMessageBox msgBox;

  msgBox.setIconPixmap ( QPixmap(qtCOFE_app_icon).
                         scaledToHeight(128,Qt::SmoothTransformation) );
  msgBox.setWindowTitle     ( "About QtCoFE" );
  msgBox.setText            ( qtCOFE_app_spec    );

  msgBox.setInformativeText (
         "CCP4 <b>C</b>l<b>o</b>ud <b>F</b>ront <b>E</b>nd.<br><br>"
         "<b>Developers:</b>"// Dr. Andrey Lebedev and "
//         "Dr. Eugene Krissinel, CCP4, STFC-RAL, UK.<br><br>"
         "<b>QtCoFE</b> is part of CCP4 Software Suite. Using it is "
         "subject to CCP4 license for academic and commercial users, "
         "obtainable from "
         "<a href=\"http://www.ccp4.ac.uk/ccp4license.php\">"
         "http://www.ccp4.ac.uk/ccp4license.php</a>."
                            );

  msgBox.setStandardButtons ( QMessageBox::Ok );
  msgBox.setDefaultButton   ( QMessageBox::Ok );
  msgBox.exec();

}


void qtCOFE::Base::configure()  {
  preferences->actualize();
  preferences->exec();
}


void qtCOFE::Base::help()  {
  QMessageBox::information ( this,"Help","Not implemented" );
}


void qtCOFE::Base::logout()  {
  session->loggedIn = false;
  login();
}

void qtCOFE::Base::preferencesChanged()  {

  if (preferences->isFontChanged())  {
    setStyleSheet ( preferences->getFontSizeStyleSheet(1.0) );

//    setStyleSheet ( QString ( "font-size: %1pt;" )
//                        .arg(6+preferences->getFontPointSize()/2) );
//    refresh_strong();
  }

}
