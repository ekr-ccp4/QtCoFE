// $Id: qtx_dragspot.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    04.05.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_dragspot <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::DragSpot
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QMouseEvent>
#include <QMessageBox>

#include "qtx_dragspot.h"

qtx::DragSpot::DragSpot ( Qt::DropAction dropAction,
                          const QString & dragImage,
                          const QString & tool_tip,
                          QWidget *parent )
             : QLabel(parent)  {
  drop_action = dropAction;
  setPixmap ( QPixmap(dragImage)
             .scaledToHeight(3*QFontMetrics(font()).height()/2) );
  if (!tool_tip.isEmpty())
    setToolTip ( tool_tip );
  setAcceptDrops ( true );
}


qtx::DragSpot::DragSpot ( Qt::DropAction dropAction,
                          const QPixmap & dragImage,
                          const QString & tool_tip,
                          QWidget *parent )
             : QLabel(parent)  {
  drop_action = dropAction;
  setPixmap ( dragImage );
  if (!tool_tip.isEmpty())
    setToolTip ( tool_tip );
  setAcceptDrops ( true );
}

void qtx::DragSpot::mousePressEvent ( QMouseEvent * event )  {
QDrag     *drag     = new QDrag(this);
QMimeData *mimeData = new QMimeData();

  mimeData->setUrls ( urls        );
//  mimeData->setText ( "just a test" );
  drag->setMimeData ( mimeData    );
  drag->setPixmap   ( *pixmap()   );
  drag->exec        ( drop_action );

  event->accept();

}
