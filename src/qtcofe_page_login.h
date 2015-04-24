// $Id: qtcofe_page_login.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_login <interface>
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

#ifndef QTCOFE_LOGINPAGE_H
#define QTCOFE_LOGINPAGE_H

#include "qtcofe_page.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

namespace qtx  {
  class LineEdit;
}

namespace qtCOFE  {

  class LoginPage : public Page  {
  Q_OBJECT

    public:
      LoginPage ( DataModel      *dm,
                  QSettings      *stn,
                  QWidget        *parent = 0,
                  Qt::WindowFlags flags  = 0 );
      virtual ~LoginPage();

      virtual PAGE_ID pageID() { return PAGE_Login; }

      virtual void makeLayout();

      bool login();

    signals:
      void user_logged_in();
      void new_account_requested();

    protected:
      qtx::LineEdit *loginNameEdt;
      qtx::LineEdit *passwordEdt;
      QCheckBox     *keepLoggedCkb;
      QPushButton   *signInBtn;
      QPushButton   *recoverBtn;
      QPushButton   *signOnBtn;

      bool login ( const QString loginName,
                   const QString cpw,
                   bool          stayLogged,
                   bool          silent );

    protected slots:
      void signInBtnClicked ();
      void recoverBtnClicked();
      void signOnBtnClicked ();

  };

}

#endif // QTCOFE_LOGINPAGE_H
