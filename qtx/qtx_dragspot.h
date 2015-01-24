// $Id: qtx_dragspot.h $
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
//  **** Module  :  qtx_dragspot <interface>
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

#ifndef qtx_DRAGSPOT_H
#define qtx_DRAGSPOT_H

#include <QLabel>
#include <QUrl>

namespace qtx  {

  class DragSpot : public QLabel  {
  Q_OBJECT

    public:
      DragSpot ( Qt::DropAction dropAction,
                 const QString & dragImage,
                 const QString & tool_tip=QString(),
                 QWidget *parent=0 );
      DragSpot ( Qt::DropAction dropAction,
                 const QPixmap & dragImage,
                 const QString & tool_tip=QString(),
                 QWidget *parent=0 );

      inline void addUrl      ( const QUrl & url )  { urls << url; }
      inline void addFilePath ( const QString & fpath )
                                 { urls << QUrl::fromLocalFile(fpath); }
      inline int  getNofUrls  () { return urls.count(); }

    protected:
      QByteArray     drop_data;
      Qt::DropAction drop_action;
      QList<QUrl>    urls;
//      virtual QSize	minimumSizeHint () const;
//      virtual QSize	maximumSizeHint () const;

      void mousePressEvent ( QMouseEvent * );

  };

}

#endif // qtx_DRAGSPOT_H
