// $Id: qtx_lineedit.cpp $
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
//  **** Module  :  qtx_lineedit <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::LineEdit
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include <QDrag>
#include <QMimeData>
#include <QUrl>

#include "qtx_lineedit.h"

qtx::LineEdit::LineEdit ( QString S, int nchars, QWidget *parent )
             : QLineEdit(S,parent) {
  field_len   = nchars;
  sample_char = 'W';
  drop_action = Qt::IgnoreAction;
}

QSize qtx::LineEdit::minimumSizeHint() const  {
QSize s = QLineEdit::minimumSizeHint();
//  s.setWidth ( 20*field_len*s.width()/35 );
  if (field_len>0)
    s.setWidth ( int(field_len*1.3*
               fontMetrics().charWidth(QString(QChar(sample_char)),0)) );
  return s;
}

QSize qtx::LineEdit::maximumSizeHint() const  {
  if (field_len>0)  return minimumSizeHint();
              else  return QSize();
}

void qtx::LineEdit::setDrag ( Qt::DropAction dropAction,
                              QString dragIcon, QString tool_tip )  {
  drop_action = dropAction;
  drag_image  = dragIcon;
  if (!tool_tip.isEmpty())
    setToolTip ( tool_tip );
}

void qtx::LineEdit::mousePressEvent ( QMouseEvent *event )  {

  if (drop_action!=Qt::IgnoreAction)  {

    QDrag     *drag     = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setUrls ( QList<QUrl>() << QUrl(text()) );
    drag->setMimeData ( mimeData );
    if (!drag_image.isEmpty())
      drag->setPixmap ( QPixmap(drag_image).scaledToHeight(2*height()) );

    drag->exec ( drop_action );

  }

  QLineEdit::mousePressEvent ( event );

}
