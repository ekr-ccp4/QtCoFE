// $Id: qtcofe_datamodel.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_datamodel <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::DataType
//       ~~~~~~~~~  qtCOFE::TaskSection
//                  qtCOFE::Task
//                  qtCOFE::DataModel
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_DATAMODEL_H
#define QTCOFE_DATAMODEL_H

#include <QObject>
#include <QStringList>

class QJsonObject;

namespace qtCOFE  {

  class Preferences;
  class Session;

  /// Data Type description class
  class DataType : public QObject  {
  Q_OBJECT

    public:
      QString type;
      QString name;
      QString desc;
      QString icon;

      DataType ( QObject * parent = 0 );
      virtual ~DataType();

      int readData ( const QJsonObject & obj );

  };

  class TaskSection : public QObject  {
  Q_OBJECT

    public:
      QString id;
      QString desc;
      QString name;
      int     order;

      TaskSection ( QObject * parent = 0 );
      virtual ~TaskSection();

      int readSectionData ( const QJsonObject & obj );

  };

  class TaskData  {
    public:
      QString type;    // "dtype_xxx"
      QString subtype; // data-dependent; list of comma-separated ids
      QChar   mode;    // modificator of data entity number (E,G,U)
      int     nmode;   // data entity number

      TaskData ();
      ~TaskData();
      bool checkType ( QString & dtype, QString & dsubtype  );
      void copy ( const TaskData * taskData ); // from taskData to "this"
      int  guessNData ( int nData );

  };

  extern int indexOf ( const QString & dtype, QString & subtype,
                       const QList<TaskData *> & taskData );

  /// Task description class
  class Task : public QObject  {
  Q_OBJECT

    public:
      QString  type;
      QString  name;
      QString  desc;
      QString  section;
      QString  icon;
      int      order;

      QList<TaskData *> inpData;
      QList<TaskData *> outData;

      Task ( QObject * parent = 0 );
      virtual ~Task();

      int readTaskDesc ( const QJsonObject & obj );

//      int  hasInput ( const QStringList & dtypes );
//      bool hasInput ( const QString     & dtype  );

      void printTask();

    protected:
      void clear();

  };

  class DataModel : public QObject  {
  Q_OBJECT

    public:
      QList<DataType    *> data_types;
      QList<TaskSection *> sections;
      QList<Task        *> tasks;

      DataModel ( Preferences *prefs,
                  Session     *ssn,
                  QObject     *parent = 0 );
      virtual ~DataModel();

      int readModel();  // read model definitions from server

      const Task     *getTask        ( const QString & type );
      const TaskData *getTaskDataOut ( const QString & taskType,
                                       const QString & dataType );
      const DataType *getDataType    ( const QString & type );

      QString taskName ( const QString & type );

      inline Preferences *getPreferences()  { return preferences; }
      inline Session     *getSession    ()  { return session;     }

      void printModel();

    protected:
      Preferences *preferences;
      Session     *session;

      void clear();

  };

}

#endif // QTCOFE_DATAMODEL_H
