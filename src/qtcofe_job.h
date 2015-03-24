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

#include "qtcofe_datamodel.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QTimer;
QT_END_NAMESPACE


class QJsonObject;

namespace qtCOFE  {

  class DataModel;
  class Task;

  class Metadata {
    public:
      int         jobId;  // job where file is kept
      QString     desc;
      QString     fname;
      QStringList columns;

      Metadata () {}
      ~Metadata() {}
  };

  class JobData : public TaskData  {

    public:

      enum SUITABILITY  {
        Unsuitable,
        Suitable,
        Ambiguous
      };

      bool         disambiguated;
      QList<Metadata *> metadata;

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
      QList<JobData *> outData;
      int         id;
      int         status;  // 0,1000,2000 (norun, running, finished)
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

      void getOutputDataSpecs ( int       outNo,
                                QString & jobName,
                                QString & fileName,
                                QString & desc,
                                int     & nSets);

      void copy ( const Task *task );

      void startTimer();

    protected:
      QTreeWidgetItem *treeItem;
      QTimer          *timer;
      int              timerCount;

      void init ();
      void clear();

     protected slots:
      void timerSlot();

  };

}

#endif // QTCOFE_JOB_H
