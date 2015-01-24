// $Id: qtcofe_taskdialog.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_taskdialog <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE task dialog class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_TASKDIALOG_H
#define QTCOFE_TASKDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QScrollArea;
class QPushButton;
class QSignalMapper;
QT_END_NAMESPACE


namespace qtCOFE  {

  class Preferences;
  class DataModel;

  class TaskDialog : public QDialog  {
  Q_OBJECT

    public:

      TaskDialog ( QWidget *parent, DataModel *dm, Preferences *prefs,
                   Qt::WindowFlags f = 0 );
      ~TaskDialog();

      void fixSize();

      QString getSelTaskType()  { return selTaskType; }

    protected:
      Preferences   *preferences;
      DataModel     *dataModel;
      QScrollArea   *scrollArea;
      QSignalMapper *signalMapper;
      QPushButton   *cancel_btn;

      QString       selTaskType;

      void makeLayout();

    protected slots:
      void taskSelected ( const QString & type );

  };

}

#endif // QTCOFE_TASKDIALOG_H
