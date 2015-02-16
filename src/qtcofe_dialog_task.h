// $Id: qtcofe_dialog_task.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialog_TASK <interface>
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

#ifndef QTCOFE_DIALOG_TASK_H
#define QTCOFE_DIALOG_TASK_H

#include <QMap>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QScrollArea;
class QPushButton;
class QSignalMapper;
QT_END_NAMESPACE


namespace qtCOFE  {

  class DataModel;
  class JobData;
  class ProjectTree;

  class TaskDialog : public QDialog  {
  Q_OBJECT

    public:

      TaskDialog ( QWidget *parent, DataModel *dm,
                   const QList<JobData *> & projData,
                   ProjectTree *jTree,
                   Qt::WindowFlags f = 0 );
      ~TaskDialog();

      void fixSize();

      QString getSelTaskType()  { return selTaskType;            }
      int     getSelTaskKey ()  { return buttonMap[selTaskType]; }

    protected:
      DataModel        *dataModel;
      ProjectTree      *jobTree;
      QScrollArea      *scrollArea;
      QSignalMapper    *signalMapper;
      QPushButton      *cancel_btn;
      QMap<QString,int> buttonMap;
      QString           selTaskType;

      void makeLayout ( const QList<JobData *> & projData );

    protected slots:
      void taskSelected ( const QString & type );

  };

}

#endif // QTCOFE_DIALOG_TASK_H
