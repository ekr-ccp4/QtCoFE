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

//QT_BEGIN_NAMESPACE
//class QLabel;
//class QGridLayout;
//class QPushButton;
//QT_END_NAMESPACE

//namespace qtx  {
//  class LineEdit;
//}

class QJsonObject;

namespace qtCOFE  {

  class DataModel;

  class Metadata {
    public:
      QString     desc;
      QString     fname;
      QStringList columns;
      Metadata () {}
      ~Metadata() {}
  };

  class Job : public QObject  {
  Q_OBJECT

    public:
      QString     type;
      QString     name;
      QString     desc;
      QString     icon;
      QStringList dtypes;
      QList<QList<Metadata *> > metadata;
      int         id;
      bool        expanded;

      Job ( QObject * parent = 0 );
      Job ( const QJsonObject & jobData, DataModel * dataModel,
            QObject * parent = 0 );
      virtual ~Job();

      void setJobData ( const QJsonObject & jobData,
                        DataModel * dataModel );

    protected:
      void clear();

  };

}

#endif // QTCOFE_JOB_H
