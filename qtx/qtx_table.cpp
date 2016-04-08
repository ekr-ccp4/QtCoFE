// $Id: qtx_table.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    26.11.15   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_table <implementation>
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

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolButton>

#include <stdio.h>
#include <algorithm>

#include "qtx_table.h"


qtx::Table::Table ( QWidget *parent, int fontPointSize )
          : QTableWidget(parent)  {
//  setSelectionMode ( QAbstractItemView::ExtendedSelection );
  setSelectionBehavior ( QAbstractItemView::SelectRows );

  if (fontPointSize>0)
    setStyleSheet ( QString("font-size: %1pt;").arg(fontPointSize) );

}

qtx::Table::~Table()  {}

QTableWidgetItem *qtx::Table::setHorzHeader ( int col,
                                       const QString & text,
                                       const QString & toolTip )  {
QTableWidgetItem *item;
  item = new QTableWidgetItem ( text );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  if (!toolTip.isEmpty())
    item->setToolTip ( toolTip );
  setHorizontalHeaderItem ( col,item );
  return item;
}

QTableWidgetItem *qtx::Table::setVertHeader ( int row,
                                       const QString & text,
                                       const QString & toolTip )  {
QTableWidgetItem *item;
  item = new QTableWidgetItem ( text );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  if (!toolTip.isEmpty())
    item->setToolTip ( toolTip );
  setVerticalHeaderItem ( row,item );
  return item;
}

QTableWidgetItem *qtx::Table::setTableItem ( int row, int col,
                                       const QString & text,
                                       Qt::Alignment alignment )  {
QTableWidgetItem *item;
  item = new QTableWidgetItem ( text );
  setItem ( row,col,item );
  item->setTextAlignment ( alignment );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  return item;
}


QTableWidgetItem *qtx::Table::setTableItem ( int row, int col, int v,
                                             Qt::Alignment alignment ) {
QTableWidgetItem *item;
  if (alignment==Qt::AlignRight)
        item = new QTableWidgetItem ( tr("%1 ").arg(v) );
  else  item = new QTableWidgetItem ( tr("%1" ).arg(v) );
  setItem ( row,col,item );
  item->setTextAlignment ( alignment );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  return item;
}

QTableWidgetItem *qtx::Table::setTableItem ( int row, int col,
                                             double q,
                                             const QString & format )  {
QTableWidgetItem *item;
char              S[100];
  sprintf ( S,format.toLatin1().constData(),q );
  item = new QTableWidgetItem ( S );
  setItem ( row,col,item );
  item->setTextAlignment ( Qt::AlignRight );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  return item;
}

QTableWidgetItem *qtx::Table::setTableItemCheck ( int row, int col,
                                      bool checked, const QString text,
                                      Qt::Alignment alignment )  {
  if (checked)
       return setTableItemCheck3 ( row,col,Qt::Checked,text,alignment );
  else return setTableItemCheck3 ( row,col,Qt::Unchecked,text,alignment );
}


QTableWidgetItem *qtx::Table::setTableItemCheck3 ( int row, int col,
                                      Qt::CheckState checkState,
                                      const QString text,
                                      Qt::Alignment alignment )  {
QTableWidgetItem *item;
  item = new QTableWidgetItem ( text );
  setItem ( row,col,item );
  item->setTextAlignment ( alignment );
  item->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |
                                          Qt::ItemIsEnabled );
  item->setCheckState ( checkState );
  return item;
}


bool qtx::Table::hasSelection()  {
  return selectionModel()->hasSelection();
}


void qtx::Table::selectionChanged ( const QItemSelection & selected,
                                    const QItemSelection & deselected )  {
  QTableWidget::selectionChanged ( selected,deselected );
  emit selection_changed ( selectionModel()->hasSelection() );
}

QString qtx::Table::clipboardText()  {
QModelIndexList indexes = selectionModel()->selectedIndexes();
QModelIndex     previous,current;
QString         selected_text,text;
QVariant        data;
int             i;

  if (indexes.size()<1)  // no selection
    return selected_text; // empty line

  // QModelIndex::operator < sorts first by row, then by column.
  // this is what we need
  std::sort ( indexes.begin(),indexes.end() );

  selected_text = QString(" ");
  if (verticalHeader())
    selected_text.append ( "\t" );
  for (i=0;i<columnCount();i++)  {
    selected_text.append ( horizontalHeaderItem(i)->text()
                           .replace(QChar('\n'),QChar(' ')) +
                           QString(" \t") );
  }
//  selected_text.append ( QLatin1Char('\n') );

  // One needs a pair of indexes to find the row changes

  previous = indexes.first();
  indexes.removeFirst();
  if (verticalHeaderItem(previous.row()))
    selected_text.append ( verticalHeaderItem(previous.row())->text()
                           .replace(QChar('\n'),QChar(' '))+
                           QLatin1Char('\t') );
  else
    selected_text.append("\n \t");

  foreach (current,indexes)  {

    data = model()->data(previous);
    text = data.toString();

    // At this point `text` contains the text in one cell
    selected_text.append ( text );

    if (current.row()!=previous.row())  {
      // If you are at the start of the row the row number of
      // the previous index isn't the same. Text is followed by
      // a row separator, which is a newline.
      if (verticalHeaderItem(current.row()))
        selected_text.append ( QLatin1Char('\n') +
                           verticalHeaderItem(current.row())->text()
                           .replace(QChar('\n'),QChar(' ')) +
                           QLatin1Char('\t') );
      else
        selected_text.append("\n \t");
    } else  {
      // Otherwise it's the same row, so append a column
      //separator, which is a tab.
      selected_text.append ( " \t" );
    }

    previous = current;

  }

  // add last element
  selected_text.append ( model()->data(current).toString() );
  selected_text.append ( QLatin1Char('\n') );

  return selected_text;

}

void qtx::Table::setTableFont ( QFont & font )  {
int  n = columnCount();
  for (int i=0;i<rowCount();i++)
    for (int j=0;j<n;j++)
      if (item(i,j))
        item(i,j)->setFont ( font );
}

void qtx::Table::setTableFixedFont()  {
QFont fixed_font;
  fixed_font.setFamily ( "Courier" );
  setTableFont ( fixed_font );
}

void qtx::Table::setTableAlignment ( int Alignment )  {
int  n = columnCount();
  for (int i=0;i<rowCount();i++)
    for (int j=0;j<n;j++)
      if (item(i,j))
        item(i,j)->setTextAlignment ( Alignment );
}


void qtx::Table::alignCells ( int flag, bool hideshow )  {
int i,j;
  if (hideshow)
    setVisible ( false );
  for (i=0;i<rowCount();i++)
    for (j=0;j<columnCount();j++)
      if (item(i,j))
        item(i,j)->setTextAlignment ( flag );
  if (hideshow)
    setVisible ( true  );
}

void qtx::Table::alignCells ( int column, int flag, bool hideshow )  {
int i;
  if (hideshow)
    setVisible ( false );
  for (i=0;i<rowCount();i++)
    if (item(i,column))
      item(i,column)->setTextAlignment ( flag );
  if (hideshow)
    setVisible ( true  );
}

void qtx::Table::resizeCells ( bool hideshow )  {
  if (hideshow)
    setVisible ( false );
  resizeColumnsToContents();
  resizeRowsToContents   ();
  if (hideshow)
    setVisible ( true  );
}

void qtx::Table::copyToClipboard()  {
QString selected_text = clipboardText();

  if (!selected_text.isEmpty())
    QApplication::clipboard()->setText ( selected_text );

}

/*
void qtx::Table::setFullWidth ( bool limitOnly )  {
int i,w,count;

  if (limitOnly)  {
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  );
    w = verticalScrollBar()->width();
  } else  {
    w = 0;
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
  }

  count = columnCount();

  for (i=0;i<count;i++)
    w += columnWidth ( i );

  if (verticalHeader()->isVisible())
    w += verticalHeader()->width();

  setMaximumWidth ( w );
  if (!limitOnly)
    setMinimumWidth ( w );

}
*/

void qtx::Table::setFullSize ( bool limitWidth, bool limitHeight )  {
int i,h,w,count;

  w = 0;
  h = 0;

  if (limitWidth)  {
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded );
    h = horizontalScrollBar()->height();
  } else
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

  if (limitHeight)  {
    setVerticalScrollBarPolicy ( Qt::ScrollBarAsNeeded  );
    w = verticalScrollBar()->width();
  } else
    setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

  count = columnCount();
  for (i=0;i<count;i++) 
    w += columnWidth ( i );
  w += 2;

  count = rowCount();
  for (i=0;i<count;i++)
    h += rowHeight ( i );
  h += 2;

//  if (verticalHeader()->isVisible())
    w += verticalHeader()->width();

//  if (horizontalHeader()->isVisible())
    h += horizontalHeader()->height();

  if (!limitWidth)  {
    setMaximumWidth ( w );
    setMinimumWidth ( w );
  }

  if (!limitHeight)  {
    setMaximumHeight ( h );
    setMinimumHeight ( h );
  }

}

/*
void QTableWidgetWithCopyPaste::paste()
{
QString selected_text = qApp->clipboard()->text();
QStringList cells = selected_text.split(QRegExp(QLatin1String("\\n|\\t")));
while(!cells.empty() && cells.back().size() == 0)
{
  cells.pop_back(); // strip empty trailing tokens
}
int rows = selected_text.count(QLatin1Char('\n'));
int cols = cells.size() / rows;
if(cells.size() % rows != 0)
{
  // error, uneven number of columns, probably bad data
  QMessageBox::critical(this, tr("Error"),
    tr("Invalid clipboard data, unable to perform paste operation."));
  return;
}

if(cols != columnCount())
{
  // error, clipboard does not match current number of columns
  QMessageBox::critical(this, tr("Error"),
    tr("Invalid clipboard data, incorrect number of columns."));
  return;
}

// don't clear the grid, we want to keep any existing headers
setRowCount(rows);
// setColumnCount(cols);
int cell = 0;
for(int row=0; row < rows; ++row)
{
  for(int col=0; col < cols; ++col, ++cell)
  {
    QTableWidgetItem *newItem = new QTableWidgetItem(cells[cell]);
    setItem(row, col, newItem);
  }
}
}
*/

void qtx::Table::keyPressEvent ( QKeyEvent *event )  {
  if (event->matches(QKeySequence::Copy))
        copyToClipboard();
  else  QTableWidget::keyPressEvent(event);
}

bool qtx::Table::isEqual ( Table *t )  {
QTableWidgetItem *wij;
QTableWidgetItem *twij;
int               nr,nc, i,j;
bool              equal = true;

  nr = t->rowCount();
  nc = t->columnCount();
  equal = (rowCount()==nr) && (columnCount()==nc);

  if (!equal)  return equal;

  for (j=0;(j<nc) && equal;j++)  {
    wij   = t->horizontalHeaderItem(j);
    twij  = horizontalHeaderItem(j);
    equal = (wij && twij) || ((!wij) && (!twij));
    if (equal && wij)
      equal = wij->text()==twij->text();
  }

  for (i=0;(i<nr) && equal;i++)  {
    wij   = t->verticalHeaderItem(i);
    twij  = verticalHeaderItem(i);
    equal = (wij && twij) || ((!wij) && (!twij));
    if (equal && wij)
      equal = wij->text()==twij->text();
  }

  for (i=0;(i<nr) && equal;i++)
    for (j=0;(j<nc) && equal;j++)  {
      wij   = t->item(i,j);
      twij  = item(i,j);
      equal = (wij && twij) || ((!wij) && (!twij));
      if (equal && wij)
        equal = wij->text()==twij->text();
    }

  return equal;

}

void qtx::Table::copy ( Table *t )  {
QTableWidgetItem *wij;
QTableWidgetItem *twi;
int               i,j;

  clear();

  setRowCount    ( t->rowCount()    );
  setColumnCount ( t->columnCount() );

  if (t->horizontalHeader()->isVisible())
    for (j=0;j<t->columnCount();j++)  {
      wij = t->horizontalHeaderItem(j);
      if (wij)  {
        twi = new QTableWidgetItem(*wij);
        setHorizontalHeaderItem ( j,twi );
      }
    }
  else
    horizontalHeader()->setVisible ( false );

  if (t->verticalHeader()->isVisible())
    for (i=0;i<t->rowCount();i++)  {
      wij = t->verticalHeaderItem(i);
      if (wij)  {
        twi = new QTableWidgetItem(*wij);
        setVerticalHeaderItem ( i,twi );
      }
    }
  else
    verticalHeader()->setVisible ( false );

  for (i=0;i<t->rowCount();i++)  {
    for (j=0;j<t->columnCount();j++)  {
      wij = t->item(i,j);
      if (wij)  {
        twi = new QTableWidgetItem(*wij);
        setItem ( i,j,twi );
      }
    }
  }


}
