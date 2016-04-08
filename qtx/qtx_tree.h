// $Id: qtx_tree.h $
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
//  **** Module  :  qtx_tree <interface>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::Tree
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2015
//
// =================================================================
//

#ifndef qtx_TREE_H
#define qtx_TREE_H

#include <QTreeWidget>

namespace qtx  {

  class Tree : public QTreeWidget  {
  Q_OBJECT

    public:

      enum { ColumnLabels=QTreeWidgetItem::UserType+1 };

      Tree ( QWidget * parent=0, int fontPointSize=0 );
      ~Tree();

      QTreeWidgetItem *addTreeItem  ( QString name );
      QTreeWidgetItem *addTreeItem  ( QTreeWidgetItem *item,
                                      QString name );
      QTreeWidgetItem *addTreeItemD ( QString name,
                                      const QVariant & data,
                                      int role );
      QTreeWidgetItem *addTreeItemD ( QTreeWidgetItem *item,
                                      QString name,
                                      const QVariant & data,
                                      int role );

      QTreeWidgetItem *addTreeItem ( QString name, QString value );
      QTreeWidgetItem *addTreeItem ( QString name, int     value );
      QTreeWidgetItem *addTreeItem ( QString name, double  value,
                                                   int     prec  );

      bool  hasSelection   ();  //!< true if tree has selection

      QString clipboardText();  //!< produces clipboard text
      void  copyToClipboard();  //!< copies selection to Clipboard

    public slots:
      void setFullWidth();

    signals:
      void  selection_changed ( bool has_selection );

    protected:
      virtual void keyPressEvent ( QKeyEvent *event );

    protected slots:
      virtual void selectionChanged (
                                const QItemSelection & selected,
                                const QItemSelection & deselected );

    private:
      void append_selection ( QString & selected_text,
                              QTreeWidgetItem *item, int & h );

  };

}

#endif // qtx_TREE_H
