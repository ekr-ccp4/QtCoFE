// $Id: qtcofe_page_login.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_login <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::LoginPage
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFontMetrics>
#include <QMessageBox>
#include <QInputDialog>
#include <QCryptographicHash>

#include "qtcofe_page_login.h"
#include "qtcofe_preferences.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_session.h"
#include "qtcofe_server.h"
#include "qtx/qtx_lineedit.h"
#include "qjson/QJsonDocument.h"
#include "qjson/QJsonObject.h"
#include "qjson/QJsonValue.h"

qtCOFE::LoginPage::LoginPage ( DataModel      *dm,
                               QWidget        *parent,
                               Qt::WindowFlags flags )
                 : Page(dm,parent,flags)  {
  loginNameEdt  = NULL;
  passwordEdt   = NULL;
  keepLoggedCkb = NULL;
  signInBtn     = NULL;
  recoverBtn    = NULL;
  signOnBtn     = NULL;
}

qtCOFE::LoginPage::~LoginPage() {
}

void qtCOFE::LoginPage::makeLayout()  {
QGridLayout *gbox;
QLabel      *label;
QFrame      *hline;
int          row;

  setContentsMargins ( 100,20,0,50 );

  gbox = new QGridLayout();
  row  = 0;

  makeFormLine ( gbox,row++,"Login name",loginNameEdt,10,"name" );
  makeFormLine ( gbox,row++,"Password",passwordEdt,10,"password" );
  passwordEdt->setEchoMode ( QLineEdit::Password	);

  keepLoggedCkb = new QCheckBox ( "keep me signed in unless I log out" );
  keepLoggedCkb->setChecked ( session->keepLogged );
  gbox->addWidget ( keepLoggedCkb,row++,0,1,2 );

  signInBtn = makeDarkButton ( "Sign In" );
  gbox->addWidget ( signInBtn,row++,0,1,2 );

  hline = new QFrame();
  hline->setFrameStyle ( QFrame::Raised | QFrame::HLine );
  hline->setLineWidth ( 3 );
  gbox->addWidget ( new QLabel ( " " ),row++,0,1,2 );
  gbox->addWidget ( hline,row++,0,1,2 );
  label = new QLabel ( " " );
  label->setMinimumHeight ( 1 );
  label->setMaximumHeight ( 1 );
  gbox->addWidget ( label,row++,0,1,2 );

  recoverBtn = makeLightButton ( "I forgot my login name or password" );
  gbox->addWidget ( recoverBtn,row++,0,1,2 );

  signOnBtn = makeLightButton ( "Create new user account" );
  gbox->addWidget ( signOnBtn,row++,0,1,2 );

  gbox->setColumnStretch ( 0,1 );
  gbox->setColumnStretch ( 1,1 );
  gbox->setColumnStretch ( 2,100 );

  gbox->setAlignment ( Qt::AlignLeft | Qt::AlignCenter );
  setLayout ( gbox );

  connect ( signInBtn ,SIGNAL(clicked()),this,SLOT(signInBtnClicked()) );
  connect ( recoverBtn,SIGNAL(clicked()),this,SLOT(recoverBtnClicked()));
  connect ( signOnBtn, SIGNAL(clicked()),this,SLOT(signOnBtnClicked()) );

}

bool qtCOFE::LoginPage::login()  {
  return login ( session->loginName,session->password,
                 session->keepLogged,true );
}

bool qtCOFE::LoginPage::login ( const QString loginName,
                                const QString cpw,
                                bool          stayLogged,
                                bool          silent )  {

  session->loggedIn = false;

  if (loginName.isEmpty())  return false;
  if (cpw.isEmpty())        return false;

  QJsonObject jsonData;
  jsonData.insert ( "login",loginName  );
  jsonData.insert ( "pwd"  ,cpw        );

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( qtCOFE_SERVER_ACT_Login,
                                 jsonData,jsonReply );

  if (rc!=SERVER_RC_Ok)  {
    if (!silent)
      server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive)) {
    if (!silent)
      QMessageBox::information ( this,"JSON Reply",
              "Result key is missed. This is a program error, please "
              "report to developers" );
  } else if (jsonReply.value("result").toString().toLower()
             == "rejected")  {
    if (!silent)
      QMessageBox::information ( this,"Login failed",
              "Either login name or password is incorrect.<p>"
              "Please try again" );
  } else if (jsonReply.value("result").toString().toLower()
             != "ok")  {
    if (!silent)
      QMessageBox::information ( this,"Login error",
              "Login attempt could not be completed.<p>"
              "<b>Error message:</b><br><i>" +
              jsonReply.value("message").toString() +
              "</i>.<p>This is a program or setup error, please report "
              "to developers." );
  } else  {
    session->loginName  = loginName;
    session->keepLogged = stayLogged;
    session->password   = cpw;
    session->loggedIn   = true;
    emit user_logged_in();
  }

  return session->loggedIn;

}


void qtCOFE::LoginPage::signInBtnClicked()  {
QCryptographicHash *cryptHash;
QString             loginName  = loginNameEdt->text().trimmed();
QString             password   = passwordEdt->text ().trimmed();
QString             cpw;
bool                stayLogged = keepLoggedCkb->isChecked();

  if (loginName.isEmpty())  {
    QMessageBox::information ( this,"No Login Name",
        "Login Name must be specified." );
    return;
  }
  if (password.isEmpty())  {
    QMessageBox::information ( this,"No Login Name",
        "Password must be specified." );
    return;
  }

  cryptHash = new QCryptographicHash(QCryptographicHash::Md5);
  cryptHash->addData ( password.toAscii() );
  cpw = cryptHash->result().toHex().toUpper();
  delete cryptHash;

  login ( loginName,cpw,stayLogged,false );

}

void qtCOFE::LoginPage::recoverBtnClicked()  {
QString email =
  QInputDialog::getText ( this,"Registered E-mail",
    "<p><i>Please type e-mail address used at creating your account,<br>and "
    "your login details will be e-mailed to you:</i>" );

  if (!email.isEmpty())  {

    QJsonObject jsonData;
    jsonData.insert ( "email",email );

    Server      server(preferences,session,this);
    QJsonObject jsonReply;
    SERVER_RC   rc = server.call ( qtCOFE_SERVER_ACT_RecoverLogin,
                                   jsonData,jsonReply );

    if (rc!=SERVER_RC_Ok)  {
      server.makeErrorMessage(this);
    } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive))  {
      QMessageBox::information ( this,"JSON Reply",
        "Result key is missed. This is a program error, please "
        "report to developers" );
    } else if (jsonReply.value("result").toString().toLower()=="ok")  {
      QMessageBox::information ( this,"Login Data Recovered",
        "Your login details have been sent to you by e-mail given." );
    } else  {
      QMessageBox::information ( this,"Login recovery failed",
        "No user account was registered with e-mail given.<p>"
        "Please try again" );
    }

  }

}

void qtCOFE::LoginPage::signOnBtnClicked()  {
  emit new_account_requested();
}
