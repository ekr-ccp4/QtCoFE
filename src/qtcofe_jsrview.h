// $Id: qtcofe_jsrview.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_jsrview <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::JSRView
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_JSRVIEW_H
#define QTCOFE_JSRVIEW_H

#include <QMainWindow>
//#include <QDateTime>

QT_BEGIN_NAMESPACE
class QWebView;
class QUrl;
class QSettings;
class QCloseEvent;
QT_END_NAMESPACE

namespace qtCOFE  {

  class RvGate;
  class Preferences;

  class JSRView : public QMainWindow  {
  Q_OBJECT
  Q_PROPERTY(int gwPlotWidth READ getGwPlotWidth)
  Q_PROPERTY(int gwTreeWidth READ getGwTreeWidth)
  Q_PROPERTY(int gwHeight    READ getGwHeight)
  Q_PROPERTY(int ioresult    READ ioresult)

    public:
      JSRView ( Preferences    *pref,
                QSettings      *stn,
                QWidget        *parent=0,
                Qt::WindowFlags flags = 0 );
      virtual ~JSRView();

      void loadPage ( const QUrl uri );

      int getGwPlotWidth();
      int getGwTreeWidth();
      int getGwHeight   ();

      Q_INVOKABLE QString readFile ( QString fname );
      int ioresult () { return iores; }

    public slots:
      void closeWindow     ();
      void printWindow     ();
      void prefDialog      ();
      void buttonClicked   ( QString command, QString data );
      void graphWidgetSize ( int & gwHeight, int & gwTreeWidth,
                             int & gwGraphWidth );

    protected:
      QWebView    *webView;
      QSettings   *settings;
      Preferences *preferences;
      int          iores;

      void readSettings ();
      void writeSettings();

      void closeEvent ( QCloseEvent *event );

      void exportFile ( const QString & fname );

    private slots:
      void loadHasFinished      ( bool ok );
      void jsWindowObjectCleared();
      void preferencesChanged   ();

  };

}

#endif // QTCOFE_JSRVIEW_H
