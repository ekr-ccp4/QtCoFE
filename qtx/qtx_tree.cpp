// $Id: qtx_tree.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    23.12.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_tree <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::Tree
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>

#include "qtx_tree.h"

qtx::Tree::Tree ( QWidget *parent, int fontPointSize )
         : QTreeWidget(parent)  {

  setSelectionMode     ( QAbstractItemView::ExtendedSelection );
  setSelectionBehavior ( QAbstractItemView::SelectRows        );

  if (fontPointSize>0)
    setStyleSheet ( QString ( "font-size: %1pt;" )
                    .arg((12+2*fontPointSize)/3) );

}

qtx::Tree::~Tree()  {}


QTreeWidgetItem *qtx::Tree::addTreeItem ( QString name )  {
QTreeWidgetItem *item = new QTreeWidgetItem ( QStringList() << name );
  addTopLevelItem ( item );
  return item;
}

QTreeWidgetItem *qtx::Tree::addTreeItem ( QTreeWidgetItem *item,
                                          QString name )  {
QTreeWidgetItem *itm = new QTreeWidgetItem ( QStringList() << name );
  if (item)  item->addChild  ( itm );
       else  addTopLevelItem ( itm );
  return itm;
}


QTreeWidgetItem *qtx::Tree::addTreeItemD ( QString name,
                                           const QVariant & data,
                                           int role )  {
QTreeWidgetItem *item = new QTreeWidgetItem ( QStringList() << name );
  item->setData ( 0,role,data );
  addTopLevelItem ( item );
  return item;
}

QTreeWidgetItem *qtx::Tree::addTreeItemD ( QTreeWidgetItem *item,
                                           QString          name,
                                           const QVariant & data,
                                           int role )  {
QTreeWidgetItem *itm = new QTreeWidgetItem ( QStringList() << name );
  itm->setData ( 0,role,data );
  if (item)  item->addChild  ( itm );
       else  addTopLevelItem ( itm );
  return itm;
}


QTreeWidgetItem *qtx::Tree::addTreeItem ( QString name,
                                          QString value )  {
QTreeWidgetItem *item;

  switch (columnCount())  {
    case 0  :
    case 1  : item = new QTreeWidgetItem ( QStringList() << name );
            break;
    default : item = new QTreeWidgetItem ( QStringList() << name <<
                                                            value );
  }

  addTopLevelItem ( item );
  return item;

}

QTreeWidgetItem *qtx::Tree::addTreeItem ( QString name,
                                          int     value )  {
QTreeWidgetItem *item = new QTreeWidgetItem (
      QStringList() << name << QString("%1").arg(value) );
  addTopLevelItem ( item );
  return item;
}

QTreeWidgetItem *qtx::Tree::addTreeItem ( QString name,
                                          qreal   value,
                                          int     prec )  {
QTreeWidgetItem *item = new QTreeWidgetItem (
      QStringList() << name << QString("%1").arg(value,0,'g',prec) );
  addTopLevelItem ( item );
  return item;
}


bool qtx::Tree::hasSelection()  {
  return selectionModel()->hasSelection();
}


void qtx::Tree::selectionChanged ( const QItemSelection & selected,
                                   const QItemSelection & deselected) {
  QTreeWidget::selectionChanged ( selected,deselected );
  emit selection_changed ( selectionModel()->hasSelection() );
}

void qtx::Tree::append_selection ( QString        & selected_text,
                                   QTreeWidgetItem *item,
                                   int            & h )  {
QWidget     *widget;
QGridLayout *gbox;
QLayoutItem *litem;
QLabel      *label;
int      i,ncols,k,nlines, j,gn;

  if (item->isSelected())  {

    ncols  = item->columnCount();
    nlines = 1;
    k      = 0;
    while (k<nlines)  {

      // offset for children
      for (i=0;i<h;i++)
        selected_text.append ( "   " );

      for (i=0;i<ncols;i++)  {

        gbox   = NULL;
        if (item->type()==qtx::Tree::ColumnLabels)  {
          widget = itemWidget ( item,i );
          if (widget)  {
            gbox = (QGridLayout*)widget->layout();
            if (gbox)  {
              nlines = qMax ( nlines,gbox->rowCount() );
              gn = gbox->columnCount();
              for (j=0;j<gn;j+=2)  {  // skip separators
                litem = gbox->itemAtPosition ( k,j );
                if (litem)  {
                  label = (QLabel*)litem->widget();
                  if (label)
                    selected_text.append ( label->text() );
                  if (j<gn-1)
                    selected_text.append ( QString(" ") +
                                           QLatin1Char('\t') );
                }
              }
            }
          }
        }

        if ((!gbox) && (!k))
          selected_text.append ( item->text(i) );

        if (i<ncols-1)
          selected_text.append ( QString(" ") + QLatin1Char('\t') );

      }

      selected_text.append ( QLatin1Char('\n') );

      k++;

    }

  }

  h++;
  k = item->childCount();
  for (i=0;i<k;i++)
    append_selection ( selected_text,item->child(i),h );
  h--;

}

QString qtx::Tree::clipboardText()  {
QString selected_text;
int     i,n,h;

  n = topLevelItemCount();
  h = 0;
  for (i=0;i<n;i++)
    append_selection ( selected_text,topLevelItem(i),h );

  return selected_text;

}


void qtx::Tree::copyToClipboard()  {
QString selected_text = clipboardText();

  if (!selected_text.isEmpty())
    QApplication::clipboard()->setText ( selected_text );

}

/*
void qtx::Tree::copyToClipboard()  {
QModelIndexList indexes = selectionModel()->selectedIndexes();
QModelIndex     previous,current;
QString         selected_text,text;
QVariant        data;

  if  (indexes.size()<1)  // no selection
    return;

  // QModelIndex::operator < sorts first by row, then by column.
  // this is what we need
  std::sort ( indexes.begin(),indexes.end() );


  // You need a pair of indexes to find the row changes

  previous = indexes.first();
  indexes.removeFirst();

  foreach (current,indexes)  {

    data = model()->data(previous);
    text = data.toString();

    // At this point `text` contains the text in one cell
    selected_text.append ( text );

    if (current.row()!=previous.row())  {
      // If you are at the start of the row the row number of
      // the previous index isn't the same. Text is followed by
      // a row separator, which is a newline.
      selected_text.append ( QLatin1Char('\n') );
    } else  {
      // Otherwise it's the same row, so append a column
      //separator, which is a tab.
      selected_text.append ( QString(" ") + QLatin1Char('\t') );
    }

    previous = current;

  }

  // add last element
  selected_text.append ( model()->data(current).toString() );
  selected_text.append ( QLatin1Char('\n') );
  //  qApp->clipboard()->setText(selected_text);
  QApplication::clipboard()->setText(selected_text);

}
*/

void qtx::Tree::keyPressEvent ( QKeyEvent *event )  {
  if (event->matches(QKeySequence::Copy))
        copyToClipboard();
  else  QTreeWidget::keyPressEvent(event);
}


void qtx::Tree::setFullWidth()  {

  for (int i=0;i<columnCount();i++)  {
    header()->setResizeMode ( i,QHeaderView::ResizeToContents	);
    resizeColumnToContents ( i );
  }

}
