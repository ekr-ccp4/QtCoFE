// $Id: qtcofe_page_account.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_account <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::AccountPage
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
#include <QCryptographicHash>

#include "qtcofe_page_account.h"
#include "qtcofe_preferences.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_session.h"
#include "qtcofe_server.h"
#include "qtx/qtx_lineedit.h"
#include "qjson/QJsonObject.h"


qtCOFE::AccountPage::AccountPage ( DataModel      *dm,
                                   QSettings      *stn,
                                   QWidget        *parent,
                                   Qt::WindowFlags flags )
                   : Page(dm,stn,parent,flags)  {
  prevPage     = NULL;
  userNameEdt  = NULL;
  emailEdt     = NULL;
  loginNameEdt = NULL;
  passwordEdt  = NULL;
  updateBtn    = NULL;
  cancelBtn    = NULL;
}

qtCOFE::AccountPage::~AccountPage() {
}

void qtCOFE::AccountPage::makeLayout()  {
QGridLayout *gbox;
QLabel      *titleLbl;
int          row = 0;

  setContentsMargins ( 100,100,0,50 );

  gbox = new QGridLayout();

  titleLbl = new QLabel ( "Account settings" );
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
  makeFormLine ( gbox,row++,"Login name",loginNameEdt,10,"jsmith"   );
  makeFormLine ( gbox,row++,"Password"  ,passwordEdt ,10,
                 "password (leave blank to keep the current one)" );
  passwordEdt->setEchoMode ( QLineEdit::Password	);
  loginNameEdt->setReadOnly ( true );

  gbox->addWidget ( new QLabel ( " " ),row++,0,1,3 );

  updateBtn = makeDarkButton  ( "Update My Account" );
  cancelBtn = makeLightButton ( "Cancel" );

  gbox->addWidget ( updateBtn,row++,0,1,2 );
  gbox->addWidget ( cancelBtn,row++,0,1,2 );

  gbox->setColumnStretch ( 0,1   );
  gbox->setColumnStretch ( 1,1   );
  gbox->setColumnStretch ( 2,100 );

  gbox->setAlignment ( Qt::AlignLeft | Qt::AlignTop );
  setLayout ( gbox );

  connect ( updateBtn,SIGNAL(clicked()),this,SLOT(updateBtnClicked()) );
  connect ( cancelBtn,SIGNAL(clicked()),this,SLOT(cancelBtnClicked()) );

}

void qtCOFE::AccountPage::fillUserData()  {

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( qtCOFE_SERVER_ACT_GetUserData,
                                 QJsonObject(),jsonReply );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive)) {
    QMessageBox::information ( this,"JSON Reply",
        "Result key is missed. This is a program error, please "
        "report to developers" );
  } else if (jsonReply.value("result").toString()!="OK")  {
    QMessageBox::information ( this,"Error",
      QString("Login is not confirmed, result code is "
              "<b>") + jsonReply.value("result").toString() + "</b>:"
              "<p><i>" + jsonReply.value("message").toString() + "</i>" );
  } else  {
    QJsonObject data = jsonReply.value("data").toObject();
    userNameEdt ->setText ( data.value("user_name").toString() );
    emailEdt    ->setText ( data.value("email"    ).toString() );
    loginNameEdt->setText ( data.value("login"    ).toString()  );
  }

}

void qtCOFE::AccountPage::updateBtnClicked()  {
QJsonObject jsonData;

  jsonData.insert ( "login"    ,loginNameEdt->text().trimmed() );
  jsonData.insert ( "user_name",userNameEdt ->text().trimmed() );
  jsonData.insert ( "email"    ,emailEdt    ->text().trimmed() );

  QString  password = passwordEdt->text().trimmed();
  if (!password.isEmpty())  {
    QCryptographicHash *cryptHash
                     = new QCryptographicHash(QCryptographicHash::Md5);
    cryptHash->addData ( password.toAscii() );
    password = cryptHash->result().toHex().toUpper();
    delete cryptHash;
  }
  jsonData.insert ( "pwd",password );

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( qtCOFE_SERVER_ACT_SetUserData,
                                 jsonData,jsonReply );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive)) {
    QMessageBox::information ( this,"JSON Reply",
          "Result key is missed. This is a program error, please "
          "report to developers" );
  } else  if (jsonReply.value("result").toString().trimmed()=="OK")  {
    session->loginName = loginNameEdt->text().trimmed();
    if (!password.isEmpty())
      session->password = password;
    emit revert_page ( prevPage );
  } else  {
     QMessageBox::information ( this,"Account data error",
            "Account data was not changed due to the following reason:"
            "<p><i>" + jsonReply.value("message").toString() );
  }

}

void qtCOFE::AccountPage::cancelBtnClicked()  {
  emit revert_page ( prevPage );
}
