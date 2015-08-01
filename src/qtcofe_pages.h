// $Id: qtcofe_pages.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_pages <interface>
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

#ifndef QTCOFE_PAGES_H
#define QTCOFE_PAGES_H

#include "qtcofe_base.h"

QT_BEGIN_NAMESPACE
class QStackedWidget;
QT_END_NAMESPACE

namespace qtCOFE  {

  class LoginPage;
  class NewUserPage;
  class AccountPage;
  class ProjectListPage;
  class ProjectPage;

  class Pages : public Base  {
  Q_OBJECT

    public:
      Pages ( QWidget *parent=0, Qt::WindowFlags flags = 0 );
      virtual ~Pages();

    protected:
      QStackedWidget  *pages;
      LoginPage       *loginPage;
      NewUserPage     *newUserPage;
      AccountPage     *accountPage;
      ProjectListPage *projectListPage;
      ProjectPage     *projectPage;

    protected slots:
      void login       ();
      void newAccount  ();
      void account     ();
      void unlock      ();
      void projectList ();
      void project     ();
      void pageChanged ( int index );
      void revertPage  ( QWidget *page );

  };

}

#endif // QTCOFE_PAGES_H
