// $Id: qtcofe_jsrview.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_jsrview <implementation>
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

#include <QWebView>
#include <QWebFrame>
#include <QSettings>
#include <QFileDialog>
#include <QPrinter>
#include <QMessageBox>

#include "qtcofe_jsrview.h"
#include "qtcofe_preferences.h"
#include "qtcofe_jsrvgate.h"
#include "qtx/qtx_printutils.h"
#include "qtcofe_defs.h"


qtCOFE::JSRView::JSRView ( Preferences    *pref,
                           QSettings      *stn,
                           QWidget        *parent,
                           Qt::WindowFlags flags )
                : QMainWindow ( parent,flags )  {

  webView = new QWebView(this);
  setCentralWidget ( webView );

  preferences = pref;
  settings    = stn;

  connect ( preferences,SIGNAL(applyPreferences()),
            this,SLOT(preferencesChanged()) );

  connect ( webView,SIGNAL(loadFinished(bool)),
            this,SLOT(loadHasFinished(bool)) );

  connect ( webView->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()),
            this,SLOT(jsWindowObjectCleared()) );

  readSettings();

}

qtCOFE::JSRView::~JSRView() {}

void qtCOFE::JSRView::closeEvent ( QCloseEvent *event )  {
  writeSettings();
  QMainWindow::closeEvent ( event );
}


void qtCOFE::JSRView::readSettings()  {

  restoreState ( settings->value(qtCOFE_SET_JSRV_WinState).toByteArray() );
  resize ( settings->value(qtCOFE_SET_JSRV_WinSize,QSize(700,800)).toSize() );
  move   ( settings->value(qtCOFE_SET_JSRV_WinPos,QPoint(100,50)).toPoint() );
  qtx::exportImageFile = settings->value ( qtCOFE_SET_JSRV_ImageFile,
                                           "Untitled.pdf" ).toString();

}

void qtCOFE::JSRView::writeSettings()  {

  settings->setValue ( qtCOFE_SET_JSRV_WinState ,saveState()   );
  settings->setValue ( qtCOFE_SET_JSRV_WinSize  ,size     ()   );
  settings->setValue ( qtCOFE_SET_JSRV_WinPos   ,pos      ()   );
  settings->setValue ( qtCOFE_SET_JSRV_ImageFile,qtx::exportImageFile );

}


void qtCOFE::JSRView::preferencesChanged()  {}

void qtCOFE::JSRView::prefDialog()  {
  preferences->actualize();
  preferences->exec();
}


void qtCOFE::JSRView::exportFile ( const QString & fname )  {
QString fileName = QFileDialog::getSaveFileName ( this,"Export As",
               preferences->exportDir + QFileInfo(fname).fileName() );

  if (!fileName.isEmpty())  {
    preferences->setExportDir ( fileName );
    QFile::remove ( fileName );
    if (!QFile::copy(fname,fileName))  {
      if (!QFileInfo(fname).exists())
        QMessageBox::information ( this,"Error",
          "File<p>"+fname+"<p>does not exist" );
      else if (QFileInfo(fileName).exists())
        QMessageBox::information ( this,"Error",
          "Cannot overwrite existing file:<p>"+fileName+
          "<p>User permissions?" );
      else
        QMessageBox::information ( this,"Error",
          "Cannot export file:<p>"+fileName+"<p>Disk full?" );
    }
  }

}

void qtCOFE::JSRView::buttonClicked ( QString command, QString data ) {

  if (command==qtCOFE_JSRV_COMMAND_Coot)
    preferences->launchCoot ( data.split ( qtCOFE_JSRV_par_del,
                         QString::SkipEmptyParts,Qt::CaseSensitive) );
  else if (command==qtCOFE_JSRV_COMMAND_CCP4MG)
    preferences->launchMG ( data.split ( qtCOFE_JSRV_par_del,
                         QString::SkipEmptyParts,Qt::CaseSensitive) );
  else if (command==qtCOFE_JSRV_COMMAND_ViewHKL)
    preferences->launchViewHKL ( data.split ( qtCOFE_JSRV_par_del,
                         QString::SkipEmptyParts,Qt::CaseSensitive) );
  else if (command==qtCOFE_JSRV_COMMAND_Browser)
    preferences->launchBrowser ( data.split ( qtCOFE_JSRV_par_del,
                         QString::SkipEmptyParts,Qt::CaseSensitive) );
  else if (command==qtCOFE_JSRV_COMMAND_Export)
    exportFile ( data );
  else
    QMessageBox::information ( this,"BUTTON",
       QString("Clicked ")+command+" '"+data+"'" );
}


void qtCOFE::JSRView::loadPage ( const QUrl url )  {
  webView->load ( url );
}

void qtCOFE::JSRView::jsWindowObjectCleared()  {
  webView->page()->mainFrame()->addToJavaScriptWindowObject (
                                        qtCOFE_JSRV_rvGateName,this );
}

void qtCOFE::JSRView::loadHasFinished ( bool ok )  {
UNUSED_ARGUMENT(ok);
  setWindowTitle ( webView->title() );
}

void qtCOFE::JSRView::closeWindow()  {
  close();
}

void qtCOFE::JSRView::printWindow()  {
QPrinter *printer;
QPainter *painter;

  if (qtx::getPrinterPreview(printer,painter,this,
                             webView,SLOT(print(QPrinter*))))  {
    painter->end();
    webView->print ( printer );
    delete printer;
    delete painter;
  }

}

int qtCOFE::JSRView::getGwPlotWidth()  {
  return preferences->plotWidth;
}


void qtCOFE::JSRView::graphWidgetSize ( int & gwHeight,
                                         int & gwTreeWidth,
                                         int & gwGraphWidth )  {
  gwHeight     = preferences->plotHeight;
  gwTreeWidth  = preferences->treeWidth;
  gwGraphWidth = preferences->plotWidth;
}

