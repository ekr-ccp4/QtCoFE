// $Id: qtcofe_dialog_import.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_import <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE data import dialog
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_DIALOG_IMPORT_H
#define QTCOFE_DIALOG_IMPORT_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QToolButton;
class QPushButton;
QT_END_NAMESPACE

class QJsonObject;

namespace qtx  {
  class Table;
}

namespace qtCOFE  {

  class Preferences;
  class Session;
  class DataModel;

  class DataImportDialog : public QDialog  {
  Q_OBJECT

    public:

      DataImportDialog ( QWidget *parent, int jobID, DataModel *dm,
                         Qt::WindowFlags f = 0 );
      ~DataImportDialog();

      void makeEmptyTable();

      int importCount();

    protected:
      Preferences  *preferences;
      Session      *session;
      int           JobID;
      DataModel    *dataModel;
      qtx::Table   *importTable;
      QToolButton  *add_btn;
      QToolButton  *del_btn;
      QPushButton  *close_btn;

      void makeLayout      ();
      void makeImportTable ( const QJsonObject & jobData );
      void importFile      ( const QString     & fpath   );

    protected slots:
      void addClicked();
      void delClicked();

  };

}

#endif // QTCOFE_DIALOG_IMPORT_H
