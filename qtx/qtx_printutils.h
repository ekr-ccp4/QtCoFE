// $Id: qtx_printutils.h $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    03.07.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_printutils <interface>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::PrintScalable
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#ifndef qtx_XMLUTILS_H
#define qtx_XMLUTILS_H

#include <QList>
#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
class QStatusBar;
class QPrinter;
class QPainter;
QT_END_NAMESPACE

namespace qtx {

  bool printWidget       ( QWidget *w, QWidget *parent,
                           QStatusBar *sbar );
  bool printWidgets      ( const QList<QWidget *> & ws, QWidget *parent,
                           QStatusBar *sbar );
  bool getPrinter        ( QPrinter *&printer, QPainter *&painter,
                           QWidget *parent );
  bool getPrinterPreview ( QPrinter     *&printer,
                           QPainter     *&painter,
                           QWidget       *parent,
                           const QObject *printingObject,
                           const char    *printingSlot  );

  extern QString exportImageFile;

  bool exportImage  ( QWidget *w, QWidget *parent,
                      const QString appTitle, QStatusBar *sbar );
  bool exportImages ( const QList<QWidget *> & ws, QWidget *parent,
                      const QString appTitle, QStatusBar *sbar );
  void clipCopy     ( QWidget *w );

  class PrintScalable : public QObject  {
  Q_OBJECT

    public:
      PrintScalable ( QObject * parent = 0 ) : QObject(parent) {}
      ~PrintScalable() {}

      bool printWidget ( QWidget *parent_widget );
      bool exportImage ( QWidget *parent_widget, QString appTitle,
                         int width, int height );
      void clipCopy    ( int width, int height );

    signals:
      void paintPicture ( QPainter * );

  };

}

#endif // qtx_PRINTUTILS_H
