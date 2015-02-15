// $Id: qtcofe_datamodel.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_datamodel <implementation>
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

#include <QMessageBox>

#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"

#include "qtcofe_datamodel.h"
#include "qtcofe_server.h"
#include "qtcofe_srvdefs.h"


// =================================================================

qtCOFE::DataType::DataType ( QObject *parent ) : QObject(parent)  {
}

qtCOFE::DataType::~DataType() {
}

int qtCOFE::DataType::readData ( const QJsonObject & obj )  {
  type = obj.value("type").toString();
  name = obj.value("name").toString();
  desc = obj.value("desc").toString();
  icon = obj.value("icon").toString();
  return 0;
}


// =================================================================

qtCOFE::TaskSection::TaskSection ( QObject *parent ) : QObject(parent) {
}

qtCOFE::TaskSection::~TaskSection() {
}

int qtCOFE::TaskSection::readData ( const QJsonObject & obj )  {
  id    = obj.value("id"   ).toString();
  name  = obj.value("name" ).toString();
  desc  = obj.value("desc" ).toString();
  order = obj.value("order").toDouble();
  return 0;
}


// =================================================================

qtCOFE::Task::Task ( QObject *parent ) : QObject(parent)  {
}

qtCOFE::Task::~Task() {
}

int qtCOFE::Task::readData ( const QJsonObject & obj )  {
QJsonArray a;
QString    mode;
int        n;

  input_dtypes .clear();
  output_dtypes.clear();

  type    = obj.value("type"   ).toString();
  name    = obj.value("name"   ).toString();
  desc    = obj.value("desc"   ).toString();
  section = obj.value("section").toString();
  icon    = obj.value("icon"   ).toString();
  order   = obj.value("order"  ).toDouble();

  a = obj.value("input_dtypes").toArray();
  n = a.count();
  for (int i=0;i<n;i++)
    input_dtypes.append ( a[i].toString() );

  a = obj.value("input_dmodes").toArray();
  n = a.count();
  for (int i=0;i<n;i++)  {
    mode = a[i].toString();
    input_dmodes.append ( mode.at(0) );
    mode = mode.remove ( 0,1 );
    if (!mode.isEmpty())  input_dnums.append ( mode.toInt() );
                    else  input_dnums.append ( 0 );
  }

  a = obj.value("output_dtypes").toArray();
  n = a.count();
  for (int i=0;i<n;i++)
    output_dtypes.append ( a[i].toString() );

  a = obj.value("output_dmodes").toArray();
  n = a.count();
  for (int i=0;i<n;i++)  {
    mode = a[i].toString();
    output_dmodes.append ( mode.at(0) );
    mode = mode.remove ( 0,1 );
    if (!mode.isEmpty())  output_dnums.append ( mode.toInt() );
                    else  output_dnums.append ( 0 );
  }

  return 0;

}

int qtCOFE::Task::hasInput ( const QStringList & dtypes )  {
bool included = true;

  for (int i=0;(i<input_dtypes.count()) && included;i++)
    included = dtypes.contains ( input_dtypes[i] );

  if (!included)
    included = input_dtypes.contains("dtype_dummy") ||
               input_dtypes.contains("dtype_any");

  if (included)  return 1;
  return 0;

}


// =================================================================

qtCOFE::DataModel::DataModel ( Preferences *prefs,
                               Session     *ssn,
                               QObject     *parent )
                 : QObject(parent)  {
  preferences = prefs;
  session     = ssn;
}

qtCOFE::DataModel::~DataModel()  {
  clear();
}

void qtCOFE::DataModel::clear()  {

  foreach (DataType *dtype,data_types)
    if (dtype)  delete dtype;

  foreach (TaskSection *sect,sections)
    if (sect)  delete sect;

  foreach (Task *task,tasks)
    if (task)  delete task;

  data_types.clear();
  sections  .clear();
  tasks     .clear();

}


int qtCOFE::DataModel::readModel()  {
QJsonObject jsonData;
QJsonObject dataModel;
Server      server(preferences,session,this);
SERVER_RC   rc;
int         i,j,n;
int         retcode = 0;

  clear();

  rc = server.call ( qtCOFE_SERVER_ACT_GetDataModel,jsonData,dataModel );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(NULL);
    return 1;
  } else  {
    QJsonArray a;

    if (dataModel.contains("dtypes"))  {
      a = dataModel.value("dtypes").toArray();
      n = a.count();
      for (i=0;(i<n) && (!retcode);i++)  {
        DataType *dtype = new DataType();
        retcode = dtype->readData ( a[i].toObject() );
        if (!retcode)  data_types.append ( dtype );
                 else  delete dtype;
      }
    } else
      retcode = 2;

    if (dataModel.contains("sections"))  {
      a = dataModel.value("sections").toArray();
      n = a.count();
      for (i=0;(i<n) && (!retcode);i++)  {
        TaskSection *sec = new TaskSection();
        retcode = sec->readData ( a[i].toObject() );
        if (!retcode)  {
          sections.append ( sec );
          for (j=0;j<i;j++)
            if (sections[i]->order<sections[j]->order)  {
              sec = sections[i];
              sections[i] = sections[j];
              sections[j] = sec;
            }
        } else
          delete sec;
      }
    } else
      retcode = 3;

    if (dataModel.contains("tasks"))  {
      a = dataModel.value("tasks").toArray();
      n = a.count();
      for (i=0;(i<n) && (!retcode);i++)  {
        Task *task = new Task();
        retcode = task->readData ( a[i].toObject() );
        if (!retcode)  {
          tasks.append ( task );
          for (j=0;j<i;j++)
            if (tasks[i]->order<tasks[j]->order)  {
              task = tasks[i];
              tasks[i] = tasks[j];
              tasks[j] = task;
            }
        } else
          delete task;
      }
    } else
      retcode = 4;

  }

  return retcode;

}

const qtCOFE::Task *qtCOFE::DataModel::getTask (
                                              const QString & type )  {
Task *task = NULL;

  for (int i=0;(i<tasks.count()) && (!task);i++)
    if (tasks.at(i)->type==type)
      task = tasks.at(i);

  return task;

}

const qtCOFE::DataType *qtCOFE::DataModel::getDataType (
                                              const QString & type )  {
DataType *dtype = NULL;

  for (int i=0;(i<data_types.count()) && (!dtype);i++)
    if (data_types.at(i)->type==type)
      dtype = data_types.at(i);

  return dtype;

}

QString qtCOFE::DataModel::taskName ( const QString & type )  {
const Task *task = getTask ( type );
  if (task)  return task->name;
  return "";
}
