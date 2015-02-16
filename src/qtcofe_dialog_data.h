// $Id: qtcofe_dialog_data.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_data <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE data dialog
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_DIALOG_DATA_H
#define QTCOFE_DIALOG_DATA_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTreeWidgetItem;
QT_END_NAMESPACE

class QJsonObject;

namespace qtx  {
  class Table;
}

namespace qtCOFE  {

  class Preferences;
  class Session;
  class ProjectTree;
  class DataModel;

  class DataDialog : public QDialog  {
  Q_OBJECT

    public:

      DataDialog ( QWidget         *parent,
                   ProjectTree     *projectTree,
                   QTreeWidgetItem *jobNode,
                   QString          taskType,
                   DataModel       *dm,
                   QString          winTitle,
                   QString          prompt,
                   Qt::WindowFlags  f = 0 );
      ~DataDialog();

    protected:
      Preferences  *preferences;
      Session      *session;
      DataModel    *dataModel;
      qtx::Table   *dataTable;
      QLabel       *prompt_lbl;
      QPushButton  *close_btn;

      void makeLayout    ();
      void makeRow       ( int   & row,
                           QString dataIcon,
                           QString fileName,
                           QString dataName,
                           QString signIcon,
                           QString metadata,
                           QString desc,
                           QString jobName,
                           int     span,
                           QBrush &brush
                         );
      void makeDataTable ( ProjectTree     *projectTree,
                           QTreeWidgetItem *jobNode,
                           QString          taskType );

      /*
    protected slots:
      void addClicked();
      void delClicked();
      */

  };

}

#endif // QTCOFE_DIALOG_DATA_H
