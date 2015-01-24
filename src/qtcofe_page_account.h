// $Id: qtcofe_page_account.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_account <interface>
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

#ifndef QTCOFE_ACCOUNTPAGE_H
#define QTCOFE_ACCOUNTPAGE_H

#include "qtcofe_page.h"

namespace qtCOFE  {

  class AccountPage : public Page  {
  Q_OBJECT

    public:
      AccountPage ( Preferences    *prefs,
                    Session        *ssn,
                    DataModel      *dm,
                    QWidget        *parent = 0,
                    Qt::WindowFlags flags  = 0 );
      virtual ~AccountPage();

      virtual PAGE_ID pageID() { return PAGE_Account; }

      virtual void makeLayout();

      void fillUserData();

    protected:
      qtx::LineEdit *userNameEdt;
      qtx::LineEdit *emailEdt;
      qtx::LineEdit *loginNameEdt;
      qtx::LineEdit *passwordEdt;
      QPushButton   *updateBtn;
      QPushButton   *cancelBtn;

    protected slots:
      void updateBtnClicked();
      void cancelBtnClicked();

  };

}

#endif // QTCOFE_ACCOUNTPAGE_H
