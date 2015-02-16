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

class QJsonObject;

namespace qtCOFE  {

  class DataModel;
  class Task;

  class Metadata {
    public:
      QString     desc;
      QString     fname;
      QStringList columns;
      Metadata () {}
      ~Metadata() {}
  };

  class JobData : public TaskData  {

    public:
      QList<Metadata *> metadata;

      JobData ();
      JobData ( TaskData *taskData );
      ~JobData();

      int suitability ( const JobData * jobData );

    protected:
      void clear();

  };

  extern int indexOf ( const QString & dtype,
                       const QList<JobData *> & jobData );



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
      int         order;
      bool        expanded;

      Job ( QObject * parent = 0 );
      Job ( const Task * task, QObject * parent = 0 );
      Job ( const QJsonObject & jobData, DataModel * dataModel,
            QObject * parent = 0 );
      virtual ~Job();

      void setJobData ( const QJsonObject & jobData,
                        DataModel * dataModel );

      inline int indexOf ( const QString & dtype )
                            { return qtCOFE::indexOf(dtype,outData); }

      int  hasInput ( const QList<JobData *> & jobData );
      bool hasInput ( const JobData * jobData );

      void copy ( const Task *task );

    protected:
      void clear();

  };

}

#endif // QTCOFE_JOB_H
