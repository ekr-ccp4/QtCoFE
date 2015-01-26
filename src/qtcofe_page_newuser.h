// $Id: qtcofe_page_newuser.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_newuser <interface>
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

#ifndef QTCOFE_NEWUSERPAGE_H
#define QTCOFE_NEWUSERPAGE_H

#include "qtcofe_page.h"

namespace qtCOFE  {

  class NewUserPage : public Page  {
  Q_OBJECT

    public:
      NewUserPage ( DataModel      *dm,
                    QWidget        *parent = 0,
                    Qt::WindowFlags flags  = 0 );
      virtual ~NewUserPage();

      virtual PAGE_ID pageID() { return PAGE_NewUser; }

      virtual void makeLayout();

    signals:
      void back_to_login();

    protected:
      QLabel        *titleLbl;
      QGridLayout   *gbox;
      qtx::LineEdit *userNameEdt;
      qtx::LineEdit *emailEdt;
      qtx::LineEdit *loginNameEdt;
      QPushButton   *makeNewUserBtn;
      QPushButton   *backToLoginBtn;

    protected slots:
      void newUserBtnClicked    ();
      void backToLoginBtnClicked();

  };

}

#endif // QTCOFE_NEWUSERPAGE_H
