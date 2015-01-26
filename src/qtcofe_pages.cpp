// $Id: qtcofe_pages.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_pages <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Pages
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QStackedWidget>
#include <QAction>

#include "qtcofe_pages.h"
#include "qtcofe_session.h"
#include "qtcofe_page_login.h"
#include "qtcofe_page_newuser.h"
#include "qtcofe_page_account.h"
#include "qtcofe_page_prjlist.h"
#include "qtcofe_page_project.h"
#include "qtcofe_defs.h"


qtCOFE::Pages::Pages ( QWidget        *parent,
                       Qt::WindowFlags flags )
             : Base(parent,flags)  {

  loginPage       = NULL;
  newUserPage     = NULL;
  accountPage     = NULL;
  projectListPage = NULL;
  projectPage     = NULL;

  pages = new QStackedWidget();
  setCentralWidget ( pages );

  connect ( pages,SIGNAL(currentChanged(int)),
            this,SLOT(pageChanged(int)) );

  login();

}

qtCOFE::Pages::~Pages() {
}

void qtCOFE::Pages::login()  {

  if (!loginPage)  {
    loginPage = new LoginPage ( dataModel );
    loginPage->makeLayout();
    pages->addWidget ( loginPage );
    connect ( loginPage,SIGNAL(new_account_requested()),
              this,SLOT(newAccount()),Qt::QueuedConnection );
    connect ( loginPage,SIGNAL(user_logged_in()),
              this,SLOT(projectList()),Qt::QueuedConnection );
  }

  loginPage->setRevertPage ( pages->currentWidget() );

  if (session->keepLogged && session->loggedIn)  {
    loginPage->setVisible ( false );
    if (!loginPage->login())
      loginPage->setVisible ( true );
  } else
    session->loggedIn = false;

  if (!session->loggedIn)  {
    loginPage->setFocus();
    pages->setCurrentWidget ( loginPage );
  }

}

void qtCOFE::Pages::newAccount()  {
  if (!newUserPage)  {
    newUserPage = new NewUserPage ( dataModel );
    newUserPage->makeLayout();
    pages->addWidget ( newUserPage );
    connect ( newUserPage,SIGNAL(back_to_login()),
              this,SLOT(login()),Qt::QueuedConnection );
    newUserPage->setFocus();
  }
  newUserPage->setRevertPage ( pages->currentWidget() );
  pages->setCurrentWidget ( newUserPage );
}

void qtCOFE::Pages::account()  {
  if (!accountPage)  {
    accountPage = new AccountPage ( dataModel );
    accountPage->makeLayout();
    pages->addWidget ( accountPage );
    connect ( accountPage,SIGNAL(revert_page(QWidget*)),
              this,SLOT(revertPage(QWidget*)) );
  }
  accountPage->setRevertPage ( pages->currentWidget() );
  accountPage->fillUserData();
  pages->setCurrentWidget ( accountPage );
}

void qtCOFE::Pages::projectList()  {
  if (!projectListPage)  {
    projectListPage = new ProjectListPage ( dataModel );
    projectListPage->makeLayout();
    pages->addWidget ( projectListPage );
    connect ( projectListPage,SIGNAL(open_project()),
              this,SLOT(project()),Qt::QueuedConnection );
    projectListPage->setFocus();
  }
  projectListPage->setRevertPage ( pages->currentWidget() );
  projectListPage->fillProjectList();
  pages->setCurrentWidget ( projectListPage );
}

void qtCOFE::Pages::project()  {
  if (projectListPage)
    setWindowTitle ( tr(qtCOFE_app_spec) + "  " +
                     projectListPage->getCrProjectPath() );
  if (!projectPage)  {
    projectPage = new ProjectPage ( dataModel );
    projectPage->makeLayout();
    pages->addWidget ( projectPage );
  }
  projectPage->setRevertPage ( pages->currentWidget() );
  projectPage->loadProject();
  pages->setCurrentWidget ( projectPage );
}

void qtCOFE::Pages::pageChanged ( int index )  {
UNUSED_ARGUMENT(index);
Page  *crPage = qobject_cast<Page*>(pages->currentWidget());

  switch (crPage->pageID())  {

    case PAGE_NewUser:
    case PAGE_Login  :     prjListAct->setVisible ( false );
                           accountAct->setVisible ( false );
                           logoutAct ->setVisible ( false );
                         break;

    case PAGE_Account:     prjListAct->setVisible ( true  );
                           accountAct->setVisible ( false );
                           logoutAct ->setVisible ( true  );
                         break;

    case PAGE_ProjectList: prjListAct->setVisible ( false );
                           accountAct->setVisible ( true  );
                           logoutAct ->setVisible ( true  );
                         break;

    case PAGE_Project:     prjListAct->setVisible ( true  );
                           accountAct->setVisible ( true  );
                           logoutAct ->setVisible ( true  );
                         break;

    default:               prjListAct->setVisible ( false );
                           accountAct->setVisible ( false );
                           logoutAct ->setVisible ( true  );

  }

}

void qtCOFE::Pages::revertPage ( QWidget *page )  {
  pages->setCurrentWidget ( page );
}
