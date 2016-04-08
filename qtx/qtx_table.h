// $Id: qtx_table.h $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    25.11.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_table <interface>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::Table
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2015
//
// =================================================================
//

#ifndef qtx_TABLE_H
#define qtx_TABLE_H

#include <QTableWidget>
#include <QMap>

namespace qtx  {

  class Table : public QTableWidget  {
  Q_OBJECT

    public:
      Table ( QWidget * parent=0, int fontPointSize=0 );
      ~Table();

      QTableWidgetItem *setHorzHeader ( int col,
                                const QString & text,
                                const QString & toolTip=QString() );
      QTableWidgetItem *setVertHeader ( int col,
                                const QString & text,
                                const QString & toolTip=QString() );

      QTableWidgetItem *setTableItem  ( int row, int col,
                              const QString & text,
                              Qt::Alignment alignment=Qt::AlignLeft  );
      QTableWidgetItem *setTableItem  ( int row, int col, int v,
                              Qt::Alignment alignment=Qt::AlignRight );
      QTableWidgetItem *setTableItem  ( int row, int col, double q,
                                        const QString & format );
      QTableWidgetItem *setTableItemCheck ( int row, int col,
                              bool checked,
                              const QString text = QString(),
                              Qt::Alignment alignment=Qt::AlignRight );
      QTableWidgetItem *setTableItemCheck3 ( int row, int col,
                              Qt::CheckState checkState,
                              const QString text = QString(),
                              Qt::Alignment alignment=Qt::AlignRight );
          
      void setTableFont      ( QFont & font  );
      void setTableFixedFont ();
      void setTableAlignment ( int Alignment );
 
    /*
      QToolButton      *setTableItemColor ( int row, int col,
                                            QColor color );
    */

      bool  hasSelection   ();  //!< true if table has selection

      QString clipboardText();  //!< produces clipboard text
      void  copyToClipboard();  //!< copies selection to Clipboard

      void alignCells   ( int flag, bool hideshow = true );
      void alignCells   ( int column, int flag, bool hideshow = true );
      void resizeCells  ( bool hideshow = true );

      bool isEqual ( Table *t );
      void copy    ( Table *t );

    public slots:
      void setFullSize  ( bool limitWidth  = false,
                          bool limitHeight = false );

    signals:
      void  selection_changed ( bool has_selection );

    protected:

      virtual void keyPressEvent ( QKeyEvent *event );

    protected slots:
      virtual void selectionChanged (
                               const QItemSelection & selected,
                               const QItemSelection & deselected );

  };

}

#endif // qtx_TABLE_H
