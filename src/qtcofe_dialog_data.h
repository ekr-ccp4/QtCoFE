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
class QCheckBox;
QT_END_NAMESPACE

class QJsonArray;

namespace qtx  {
  class Tree;
}

namespace qtCOFE  {

  class Preferences;
  class Session;
  class ProjectTree;
  class DataModel;
  class DataChoice;

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

      void resizeToData();

      QJsonArray *getSelections(); // after "done" in disambiguator

    protected:
      Preferences  *preferences;
      Session      *session;
      DataModel    *dataModel;
      qtx::Tree    *dataTree;
      QLabel       *prompt_lbl;
      QPushButton  *close_btn;
      QPushButton  *cancel_btn;
      QPushButton  *accept_btn;
      QList<DataChoice *> choices;

      void clear();
      void makeLayout();
      QTreeWidgetItem * makeSection ( const QString name,
                                      const QString icon,
                                      const QString statusIcon,
                                      const QString prompt
                                    );
      /*
      QTreeWidgetItem * makeRow     ( QTreeWidgetItem * item,
                                      QStringList     & fields,
                                      QString           jobType,
                                      bool              checked,
                                      DataChoice      * dataChoice,
                                      int               nData,
                                      int               nChecked
                                    );
      */
      QTreeWidgetItem * makeRow     ( QTreeWidgetItem * item,
                                      QStringList     & fnames,
                                      QStringList     & desc,
                                      QList<int>      & key,
                                      QString           jname,
                                      QString           jobType,
                                      bool              checked,
                                      DataChoice      * dataChoice,
                                      int               nChecked
                                    );

      void makeDataTree ( ProjectTree     * projectTree,
                          QTreeWidgetItem * jobNode,
                          QString           taskType );

    protected slots:
      void checkBoxClicked();

  };

}

#endif // QTCOFE_DIALOG_DATA_H
