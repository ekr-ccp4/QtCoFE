// $Id: qtcofe_job.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_job <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Job
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_JOB_H
#define QTCOFE_JOB_H

#include <QObject>
#include <QStringList>
#include <QMap>

#include "qtcofe_datamodel.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QTimer;
class QProgressBar;
QT_END_NAMESPACE


class QJsonObject;

namespace qtCOFE  {

  class DataModel;
  class Task;

  class Component {
    public:
      int                   jobId;   // job where file is kept
      QString               subtype; // data subtype
      QString               dname;   // display name
      QString               desc;
      QStringList           fnames;
      QMap<QString,QString> metadata;

      Component () {}
      ~Component() {}
  };

  class JobData : public TaskData  {

    public:

      enum SUITABILITY  {
        Unsuitable,
        Suitable,
        Ambiguous
      };

      bool            data_selected;
      QList<Component *> components;  // list on data sets/items

      JobData ();
      JobData ( TaskData *taskData );
      ~JobData();

      SUITABILITY isSuitable ( const QList<JobData *> jobData );
//      int suitability ( const JobData * jobData );

    protected:
      void clear();

      SUITABILITY E_Suitable ( const QList<JobData *> jobData );
      SUITABILITY G_Suitable ( const QList<JobData *> jobData );
      SUITABILITY U_Suitable ( const QList<JobData *> jobData );
      int n_suitable ( JobData *jobData );

  };

  extern int indexOf ( const QString & dtype,
                       const QList<JobData *> & jobData );
  extern int indexOf ( const QString & dtype,
                       const QList<QList<JobData *> > & jobData );

  class Job : public QObject  {
  Q_OBJECT

    public:
      QString     type;
      QString     name;
      QString     desc;
      QString     section;
      QString     icon;
      QList<JobData *> inpData;
      QList<JobData *> outData;  // list on data types
      int         id;
      int         status;  // 0,100,1000,2000 (norun, starting,
                           //                  running, finished)
      int         order;
      bool        expanded;

      Job ( QObject * parent = 0 );
      Job ( const Task * task, QObject * parent = 0 );
      Job ( const QJsonObject & jobData, DataModel * dataModel,
            QObject * parent = 0 );
      virtual ~Job();

      void setJobData ( const QJsonObject & jobData,
                        DataModel * dataModel );

      inline void setTreeItem ( QTreeWidgetItem *item )
                                                  { treeItem = item; }

      inline int indexOf ( const QString & dtype )
                            { return qtCOFE::indexOf(dtype,outData); }

      JobData::SUITABILITY isSuitable (
                            const QList<QList<JobData *> > & jobData );
      JobData::SUITABILITY isInputSuitable (
                            int inpNo, // index in inpData
                            const QList<QList<JobData *> > & jobData );

      void getOutputDataSpecs ( int           outNo,
                                QString       subtype,
                                QString     & jobName,
                                QStringList & dataName,
                                QStringList & desc,
                                QList<int>  & key
                              );

      void copy ( const Task *task );

      void setProgressBar ( QProgressBar *pbar );
      void showStatus();

      void copyTreeData ( Job * job );

    protected:
      QTreeWidgetItem *treeItem;
      QProgressBar    *progressBar;

      void init ();
      void clear();

  };

}

#endif // QTCOFE_JOB_H
