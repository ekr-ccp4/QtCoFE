// $Id: qtcofe_base.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_base <interface>
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

#ifndef QTCOFE_BASE_H
#define QTCOFE_BASE_H

#include <QMainWindow>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QSettings;
class QAction;
class QMenu;
class QToolBar;
QT_END_NAMESPACE

namespace qtCOFE  {

  class Preferences;
  class Session;
  class DataModel;

  class Base : public QMainWindow  {
  Q_OBJECT

    public:
      Base ( QWidget *parent=0, Qt::WindowFlags flags=0 );
      virtual ~Base();

    protected:
      QSettings   *settings;
      Preferences *preferences;
      Session     *session;
      DataModel   *dataModel;

      // Toolbar elements
      QAction     *prjListAct;
      QAction     *accountAct;
      QAction     *unlockAct;
      QAction     *preferencesAct;
      QAction     *helpAct;

      QAction     *exitAct;
      QAction     *logoutAct;
      QAction     *aboutAct;
      QAction     *aboutQtAct;

      QMenu       *fileMenu;
      QMenu       *helpMenu;

      QToolBar    *toolBar;

      QSize minimumSizeHint() const;
      QSize sizeHint() const;

      virtual void readSettings ();
      virtual void writeSettings();
      void closeEvent   ( QCloseEvent *event );

      void createAction (
                  QAction      *& action,
                  const char    * iconPath,
                  const char    * menuName,
                  const char    * toolbarName,
                  QKeySequence::StandardKey hotKey,
                  const char    * statusTip,
                  const QObject * receiver,
                  const char    * method,
                  Qt::ConnectionType connect_type = Qt::AutoConnection
                        );

      void createAction (
                  QAction      *& action,
                  const char    * iconPath,
                  const char    * menuName,
                  const char    * toolbarName,
                  const char    * hotKey,
                  const char    * statusTip,
                  const QObject * receiver,
                  const char    * method,
                  Qt::ConnectionType connect_type = Qt::AutoConnection
                        );

      void createActions  ();
      void createMenus    ();
      void createToolBar  ();
      void createStatusBar();

    protected slots:
      void about    ();
      void configure();
      void help     ();
      void logout   ();
      void readModel();
      virtual void account() {}
      virtual void unlock()  {}
      virtual void login  () {}
      virtual void projectList() {}
      virtual void actionAvailChanged() {}
      virtual void preferencesChanged();

  };

}

#endif // QTCOFE_BASE_H
