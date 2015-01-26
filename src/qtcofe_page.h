// $Id: qtcofe_page.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Page
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_PAGE_H
#define QTCOFE_PAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QGridLayout;
class QPushButton;
QT_END_NAMESPACE

namespace qtx  {
  class LineEdit;
}

namespace qtCOFE  {

  class Preferences;
  class Session;
  class DataModel;

  enum PAGE_ID  {
    PAGE_Template,
    PAGE_Login,
    PAGE_NewUser,
    PAGE_Account,
    PAGE_ProjectList,
    PAGE_Project
  };

  class Page : public QWidget  {
  Q_OBJECT

    public:
      Page ( DataModel      *dm,
             QWidget        *parent = 0,
             Qt::WindowFlags flags  = 0 );
      virtual ~Page();

      virtual PAGE_ID pageID() { return PAGE_Template; }

      virtual void makeLayout() {}

      void setRevertPage ( QWidget *page )  { prevPage = page; }

    signals:
      void revert_page ( QWidget * page );

    protected:
      Preferences *preferences;
      Session     *session;
      DataModel   *dataModel;
      QWidget     *prevPage;

      void makeFormLine ( QGridLayout    *gbox,
                          int             row,
                          const QString & name,
                          qtx::LineEdit *&edt,
                          int             size,
                          const QString & placeholder );
      QPushButton *makeDarkButton  ( const QString &text );
      QPushButton *makeLightButton ( const QString &text );

  };

}

#endif // QTCOFE_PAGE_H
