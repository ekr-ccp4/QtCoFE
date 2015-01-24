// $Id: qtx_lineedit.h $
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
//  **** Module  :  qtx_lineedit <interface>
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

#ifndef qtx_LINEEDIT_H
#define qtx_LINEEDIT_H

#include <QLineEdit>

namespace qtx  {

  class LineEdit : public QLineEdit  {
    Q_OBJECT

    public:
      LineEdit ( QString S, int nchars, QWidget *parent=0 );

      inline void setFieldSize  ( int  nchars ) { field_len   = nchars; }
      inline void setSampleChar ( char sample ) { sample_char = sample; }
      void setDrag ( Qt::DropAction dropAction,
                     QString dragIcon, QString tool_tip );

    protected:
      int            field_len;
      char           sample_char;
      Qt::DropAction drop_action;
      QString        drag_image;
      virtual QSize	minimumSizeHint () const;
      virtual QSize	maximumSizeHint () const;

      void mousePressEvent ( QMouseEvent *event );

  };

}

#endif // qtx_LINEEDIT_H
