// $Id: qtcofe_page_newuser.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_newuser <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::NewUserPage
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include "qtcofe_page_newuser.h"
#include "qtcofe_preferences.h"
#include "qtcofe_srvdefs.h"
//#include "qtcofe_session.h"
#include "qtcofe_server.h"
#include "qjson/QJsonObject.h"
#include "qtx/qtx_lineedit.h"


qtCOFE::NewUserPage::NewUserPage ( Preferences    *prefs,
                                   Session        *ssn,
                                   DataModel      *dm,
                                   QWidget        *parent,
                                   Qt::WindowFlags flags )
                   : Page(prefs,ssn,dm,parent,flags)  {
  titleLbl       = NULL;
  gbox           = NULL;
  userNameEdt    = NULL;
  emailEdt       = NULL;
  loginNameEdt   = NULL;
  makeNewUserBtn = NULL;
  backToLoginBtn = NULL;
}

qtCOFE::NewUserPage::~NewUserPage()  {
}

void qtCOFE::NewUserPage::makeLayout()  {
int  row;

  setContentsMargins ( 100,100,0,50 );

  gbox = new QGridLayout();
  row  = 0;

  titleLbl = new QLabel ( "Request for new user account" );
  titleLbl->setStyleSheet (
        preferences->getFontSizeStyleSheet(2.00) +
        "font-style:italic;font-weight:bold"
        );
  gbox->addWidget ( titleLbl,row++,0,1,3 );
  gbox->addWidget ( new QLabel ( " " ),row++,0,1,3 );

  makeFormLine ( gbox,row++,"User name" ,userNameEdt ,20,
                 "Dr. John Smith" );
  makeFormLine ( gbox,row++,"E-mail"    ,emailEdt    ,20,
                 "john.smith@university.edu" );
  makeFormLine ( gbox,row++,"Login name",loginNameEdt,10,
                 "jsmith" );

  gbox->addWidget ( new QLabel ( " " ),row++,0,1,3 );

  makeNewUserBtn = makeDarkButton  (
           "Create new account and e-mail temporary password to me" );
  backToLoginBtn = makeLightButton ( "Back to User Login");

  gbox->addWidget ( makeNewUserBtn,row++,0,1,2 );
  gbox->addWidget ( backToLoginBtn,row++,0,1,2 );

  gbox->setColumnStretch ( 0,1 );
  gbox->setColumnStretch ( 1,1 );
  gbox->setColumnStretch ( 2,100 );

  gbox->setAlignment ( Qt::AlignLeft | Qt::AlignTop );
  setLayout ( gbox );

  connect ( makeNewUserBtn,SIGNAL(clicked()),
            this,SLOT(newUserBtnClicked()) );
  connect ( backToLoginBtn,SIGNAL(clicked()),
            this,SLOT(backToLoginBtnClicked()) );

}

void qtCOFE::NewUserPage::newUserBtnClicked()  {
QString  userName  = userNameEdt ->text().trimmed();
QString  loginName = loginNameEdt->text().trimmed();
QString  email     = emailEdt    ->text().trimmed();


  if (userName.isEmpty())  {
    QMessageBox::information ( this,"No User Name",
        "User Name must be specified." );
    return;
  }

  if (loginName.isEmpty())  {
    QMessageBox::information ( this,"No Login Name",
        "Login Name must be specified." );
    return;
  }

  if (!email.contains('@'))  {
    QMessageBox::information ( this,"No e-Mail Address",
        "Valid e-mail address must be specified." );
    return;
  }

  QJsonObject jsonData;
  jsonData.insert ( "user_name",userName  );
  jsonData.insert ( "login"    ,loginName );
  jsonData.insert ( "email"    ,email     );

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( qtCOFE_SERVER_ACT_CreateNewUser,
                                 jsonData,jsonReply );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive)) {
    QMessageBox::information ( this,"JSON Reply",
          "Result key is missed. This is a program error, please "
          "report to developers" );
  } else if (jsonReply.value("result").toString()=="OK")  {
    QMessageBox::information ( this,"Password e-mailed",
          "Your login password has been sent to you by e-mail given. " );
  } else if (jsonReply.value("result").toString()=="user_exists")  {
    QMessageBox::information ( this,"Password e-mailed",
          "An account with given login name already exists. If this "
          "is your previous account, please use the recover password "
          "option. Otherwise, choose a different login name." );
  } else  {
    QMessageBox::information ( this,"Fail to create account",
          "User account could not be created.<p>"
          "<b>Error message:</b><br><i>" +
          jsonReply.value("message").toString() +
          "</i>. This is a program or setup error, please report "
          "to developers." );
  }

}

void qtCOFE::NewUserPage::backToLoginBtnClicked()  {
  emit back_to_login();
}
