// $Id: qtx_printutils.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    09.01.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_printutils <implementation>
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

#include <QWidget>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QFileInfo>
#include <QApplication>
#include <QClipboard>
//#include <QMimeData>
//#include <QSvgGenerator>
//#include <QBuffer>

#include "qtx_printutils.h"

void print ( const QList<QWidget *> & ws, QPrinter *printer,
             QPainter *painter )  {
QWidget *w;
double   scale;
int      i,py,pw,ph,pp,cp,sy;

  pw = 0;
  ph = 0;
  for (i=0;i<ws.count();i++)  {
    w = ws.at(i);
    if (w)  {
      pw  = qMax ( pw,w->width() );
      ph += w->height();
    }
  }

  if (printer)  {
    scale = printer->pageRect().width()/double(pw);
    painter->scale ( scale,scale );
    pp = (int)(printer->pageRect().height()/scale);
  } else
    pp = ph;

  cp = 0;
  for (i=0;i<ws.count();i++)  {
    w = ws.at(i);
    if (w)  {
      ph = w->height();
      py = 0;
      sy = cp;
      while (py<ph)  {
        cp = qMin(pp-sy,ph-py);
        QRegion region ( 0,py,pw,cp );
        w->render ( painter,QPoint(0,sy),region );
        py += cp;
        if (printer && (py+sy<ph))
          printer->newPage();
        sy  = 0;
      }
    }
  }

  painter->end();

}


void paint ( QWidget *w, QPaintDevice & pd, QPainter & painter )  {
double scale;
int    pw,ph;

  pw = w->width ();
  ph = w->height();
  scale = pd.width()/double(pw);
  scale = qMin(scale,pd.height()/double(ph));
  painter.scale ( scale,scale );
  w->render ( &painter );
  painter.end();

}

bool qtx::getPrinter ( QPrinter *&printer, QPainter *&painter,
                       QWidget *parent )  {
QPrintDialog *dialog;
int           rc;

  printer = new QPrinter ( QPrinter::HighResolution );
  printer->setPageSize   ( QPrinter::A4             );

  dialog = new QPrintDialog ( printer,parent );
  dialog->setWindowTitle ( "Print Document"                  );
  dialog->setOption      ( QAbstractPrintDialog::PrintToFile );
//  dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);

  rc = dialog->exec();
  if (rc==QDialog::Accepted)  {
    painter = new QPainter();
    if (painter->begin(printer))  {
      delete dialog;
      return true;
    } else  {
      QMessageBox msgBox(parent);
      QString     msg;
      msgBox.setWindowTitle ( "Print Error" );
      msgBox.setIcon ( QMessageBox::Warning );
      msg = "Printing <b>is not possible</b>.<br>Device: ";
      msg.append     ( printer->printerName() );
      msgBox.setText ( msg );
      msgBox.exec    ();
      delete printer;
      delete painter;
      printer = NULL;
      painter = NULL;
    }
  } else  {
    painter = NULL;
    delete printer;
    printer = NULL;
  }

  delete dialog;
  return false;

}


bool qtx::getPrinterPreview ( QPrinter     *&printer,
                              QPainter     *&painter,
                              QWidget       *parent,
                              const QObject *printingObject,
                              const char    *printingSlot  )  {
QPrintPreviewDialog *dialog;
int                  rc;

  printer = new QPrinter ( QPrinter::HighResolution );
  printer->setPageSize   ( QPrinter::A4             );

  dialog = new QPrintPreviewDialog ( printer,parent );
  dialog->setWindowTitle ( "Print Document"                  );
//  dialog->setOption      ( QAbstractPrintDialog::PrintToFile );
  dialog->connect( dialog,SIGNAL(paintRequested(QPrinter*)),
                   printingObject,printingSlot );
//  dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);

  rc = dialog->exec();
  if (rc==QDialog::Accepted)  {
    painter = new QPainter();
    if (painter->begin(printer))  {
      delete dialog;
      return true;
    } else  {
      QMessageBox msgBox(parent);
      QString     msg;
      msgBox.setWindowTitle ( "Print Error" );
      msgBox.setIcon ( QMessageBox::Warning );
      msg = "Printing <b>is not possible</b>.<br>Device: ";
      msg.append     ( printer->printerName() );
      msgBox.setText ( msg );
      msgBox.exec    ();
      delete printer;
      delete painter;
      printer = NULL;
      painter = NULL;
    }
  } else  {
    painter = NULL;
    delete printer;
    printer = NULL;
  }

  delete dialog;
  return false;

}


bool qtx::printWidget ( QWidget *w, QWidget *parent, QStatusBar *sbar ) {
QPrinter *printer;
QPainter *painter;

  if (getPrinter(printer,painter,parent))  {
    print ( QList<QWidget *>() << w,printer,painter );
    if (sbar) sbar->showMessage ( "Printed.",4000 );
    delete printer;
    delete painter;
    return true;
  }

  return false;

}

bool qtx::printWidgets ( const QList<QWidget *> & ws, QWidget *parent,
                         QStatusBar *sbar ) {
QPrinter *printer;
QPainter *painter;

  if (getPrinter(printer,painter,parent))  {
    print ( ws,printer,painter );
    if (sbar) sbar->showMessage ( "Printed.",4000 );
    delete printer;
    delete painter;
    return true;
  }

  return false;

}



QString qtx::exportImageFile = "Untitled.pdf";


bool qtx::exportImage ( QWidget *w, QWidget *parent,
                        const QString appTitle, QStatusBar *sbar )  {
  return exportImages ( QList<QWidget *>() << w,parent,appTitle,sbar );
}


bool qtx::exportImages ( const QList<QWidget *> & ws, QWidget *parent,
                         const QString appTitle, QStatusBar *sbar )  {
QFileInfo fi(exportImageFile);
QString   exportType = "." + fi.suffix();
QString   fileName   = fi.path();
int       pw,ph;

  if (fileName.isEmpty())
       fileName  = fi.completeBaseName() + exportType;
#ifdef Platform_Windows
  else fileName += "\\" + fi.completeBaseName() + exportType;
#else
  else fileName += "/" + fi.completeBaseName() + exportType;
#endif

  fileName = QFileDialog::getSaveFileName ( parent,
                    "Export PDF/PS/PNG/JPG",fileName,
                    "PDF/PS/PNG/JPG (*.pdf *.ps *.png *.jpg *.jpeg)" );

  if (!fileName.isEmpty())  {

    QString suffix = QFileInfo(fileName).suffix().toLower();
    if ((suffix=="pdf") || (suffix=="ps"))  {
      QPainter painter;
      QPrinter printer(QPrinter::HighResolution);

      exportImageFile = fileName;
      printer.setOutputFileName ( fileName     );
      printer.setPageSize       ( QPrinter::A4 );
      if (suffix=="pdf")
            printer.setOutputFormat ( QPrinter::PdfFormat );
      else  printer.setOutputFormat ( QPrinter::PostScriptFormat );

/*
      QSvgGenerator generator;
     generator.setFileName(fileName);
     generator.setSize(QSize(200, 200));
     generator.setViewBox(QRect(0, 0, 200, 200));
//     generator.setTitle(tr("SVG Generator Example Drawing"));
//     generator.setDescription(tr("An SVG drawing created by the SVG Generator "
//                                 "Example provided with Qt."));
*/
      if (painter.begin(&printer))  {
        print ( ws,&printer,&painter );
        if (sbar)  sbar->showMessage ( QString("Exported into %1")
                                       .arg(fileName),4000 );
        return true;
      } else
        QMessageBox::warning ( parent, appTitle,
                                     QString("Cannot write file %1.")
                                     .arg(fileName) );

    } else if ((suffix=="png") || (suffix=="jpg") || (suffix=="jpeg")) {

      pw = 0;
      ph = 0;
      foreach (QWidget *w,ws)
        if (w)  {
          pw  = qMax ( pw,w->width() );
          ph += w->height();
        }

      QPixmap  pm ( pw,ph );
      QPainter painter;

      exportImageFile = fileName;
      if (painter.begin(&pm))  {
//        paint ( w,pm,painter );
//        w->render ( &painter );
        print ( ws,NULL,&painter );
        pm.save ( exportImageFile );
        if (sbar)  sbar->showMessage ( QString("Exported into %1")
                                       .arg(fileName),4000 );
        return true;
      } else
        QMessageBox::warning ( parent, appTitle,
                               QString("Cannot open painting device.") );

    } else
      QMessageBox::warning ( parent, appTitle,
                         QString("Unsupported export format<br><br>%1")
                                .arg(fileName) );

  }

  return false;

}

void qtx::clipCopy ( QWidget *w )  {
QImage   image   ( w->width(),w->height(),QImage::Format_ARGB32 );
QPainter painter ( &image );

  painter.fillRect ( 0,0,image.width(),image.height(),
                     QPalette().color(QPalette::Window) );
  w->render ( &painter );
  QApplication::clipboard()->setImage ( image,QClipboard::Clipboard );

}



bool qtx::PrintScalable::printWidget ( QWidget *parent_widget )  {
QPrinter *printer;
QPainter *painter;

  if (getPrinter(printer,painter,parent_widget))  {
    emit paintPicture ( painter );
    painter->end();
    delete printer;
    delete painter;
    return true;
  }

  return false;

}

bool qtx::PrintScalable::exportImage ( QWidget *parent_widget,
                                       QString appTitle,
                                       int width, int height )  {
QFileInfo fi(exportImageFile);
QString   exportType = "." + fi.suffix();
QString   fileName   = fi.path();

  if (fileName.isEmpty())
       fileName  = fi.completeBaseName() + exportType;
#ifdef Platform_Windows
  else fileName += "\\" + fi.completeBaseName() + exportType;
#else
  else fileName += "/" + fi.completeBaseName() + exportType;
#endif

  fileName = QFileDialog::getSaveFileName ( parent_widget,
                    "Export PDF/PS/PNG/JPG",fileName,
                    "PDF/PS/PNG/JPG (*.pdf *.ps *.png *.jpg *.jpeg)" );

  if (!fileName.isEmpty())  {

    QString suffix = QFileInfo(fileName).suffix().toLower();
    if ((suffix=="pdf") || (suffix=="ps"))  {
      QPainter painter;
      QPrinter printer(QPrinter::HighResolution);

      exportImageFile = fileName;
      printer.setOutputFileName ( fileName     );
      printer.setPageSize       ( QPrinter::A4 );
      if (suffix=="pdf")
            printer.setOutputFormat ( QPrinter::PdfFormat );
      else  printer.setOutputFormat ( QPrinter::PostScriptFormat );

      if (painter.begin(&printer))  {
        emit paintPicture ( &painter );
        painter.end();
        return true;
      } else
        QMessageBox::warning ( parent_widget, appTitle,
                                       QString("Cannot write file %1.")
                                       .arg(fileName) );

    } else if ((suffix=="png") || (suffix=="jpg") || (suffix=="jpeg")) {

      QPixmap  pm ( width,height );
      QPainter painter;

      exportImageFile = fileName;
      if (painter.begin(&pm))  {
        painter.fillRect ( 0,0,width,height,Qt::white );
        emit paintPicture ( &painter );
        painter.end();
        pm.save ( exportImageFile );
        return true;
      } else
        QMessageBox::warning ( parent_widget, appTitle,
                               QString("Cannot open painting device.") );

    } else
      QMessageBox::warning ( parent_widget, appTitle,
                         QString("Unsupported export format<br><br>%1")
                                .arg(fileName) );

  }

  return false;

}


void qtx::PrintScalable::clipCopy ( int width, int height )  {
QImage   image   ( width,height,QImage::Format_ARGB32 );
QPainter painter ( &image );

  painter.fillRect ( 0,0,image.width(),image.height(),Qt::white );
  emit paintPicture ( &painter );
  painter.end();
  QApplication::clipboard()->setImage ( image,QClipboard::Clipboard );

}


/*
void qtx::PrintScalable::clipCopy ( int width, int height )  {
QSvgGenerator generator;
QBuffer       buffer;

//     generator.setFileName("/Users/Eugene/untitled.svg");
   generator.setOutputDevice ( &buffer );
     generator.setSize(QSize(width,height));
     generator.setViewBox(QRect(0,0,width,height));
//     generator.setTitle(tr("SVG Generator Example Drawing"));
//     generator.setDescription(tr("An SVG drawing created by the SVG Generator "
//                                 "Example provided with Qt."));

    QPainter painter;
     painter.begin(&generator);
  emit paintPicture ( &painter );
     painter.end();

QMimeData mimeData;
mimeData.setData ( "image/svg+xml",buffer.buffer().constData() );

QFile file("/Users/Eugene/unt.svg" );
file.open(QFile::WriteOnly);
file.write(buffer.buffer());
file.close();

  QApplication::clipboard()->setMimeData ( &mimeData );

}
*/
