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
#include <stdio.h>

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

qtCOFE::TaskData::TaskData () {
  mode  = 'E';
  nmode = 1;
}

qtCOFE::TaskData::~TaskData() {}

void qtCOFE::TaskData::copy ( const TaskData * taskData )  {
// from taskData to "this"
  if (taskData)  {
    type  = taskData->type;  // "dtype_xxx"
    mode  = taskData->mode;  // modificator of data entity number (E,G,U)
    nmode = taskData->nmode; // data entity number
  }
}

int qtCOFE::TaskData::guessNData ( int nData )  {
  switch (mode.toAscii())  {
    default:
    case 'E' : if (nmode<=nData)  return nmode;
               return -1;
    case 'G' : if (nmode<nData)   return nData;
               return -1;
    case 'U' : if (nmode>=nData)  return nData;
               return -1;
  }
}


int qtCOFE::indexOf ( const QString & dtype,
                      const QList<TaskData *> & taskData )  {
int k = -1;
  for (int i=0;(i<taskData.count()) && (k<0);i++)
    if (taskData.at(i)->type==dtype)
      k = i;
  return k;
}


// =================================================================

qtCOFE::Task::Task ( QObject *parent ) : QObject(parent)  {
}

qtCOFE::Task::~Task()  {
  clear();
}

void qtCOFE::Task::clear()  {
  foreach (TaskData *td,inpData)
    if (td)  delete td;
  inpData.clear();
  foreach (TaskData *td,outData)
    if (td)  delete td;
  outData.clear();
}

int qtCOFE::Task::readData ( const QJsonObject & obj )  {
QJsonArray a,ai;
QString    mode;

  clear();

  type    = obj.value("type"   ).toString();
  name    = obj.value("name"   ).toString();
  desc    = obj.value("desc"   ).toString();
  section = obj.value("section").toString();
  icon    = obj.value("icon"   ).toString();
  order   = obj.value("order"  ).toDouble();

  a = obj.value("inp_data").toArray();
  for (int i=0;i<a.count();i++)  {
    ai = a[i].toArray();
    TaskData *td = new TaskData();
    td->type  = ai[0].toString();
    td->mode  = ai[1].toString()[0];
    td->nmode = ai[2].toDouble();
    inpData.append ( td );
  }

  a = obj.value("out_data").toArray();
  for (int i=0;i<a.count();i++)  {
    ai = a[i].toArray();
    TaskData *td = new TaskData();
    td->type  = ai[0].toString();
    td->mode  = ai[1].toString()[0];
    td->nmode = ai[2].toDouble();
    outData.append ( td );
  }

  return 0;

}

/*
int qtCOFE::Task::hasInput ( const QStringList & dtypes )  {
bool included = true;

  for (int i=0;(i<inpData.count()) && included;i++)
    included = dtypes.contains ( inpData.at(i)->type );

  for (int i=0;(i<inpData.count()) && (!included);i++)  {
    QString dtype = inpData.at(i)->type;
    included = (inpData.at(i)->type=="dtype_dummy") ||
               (inpData.at(i)->type=="dtype_any");
  }

  if (included)  return 1;
  return 0;

}


bool qtCOFE::Task::hasInput ( const QString & dtype )  {
bool b = false;

  for (int i=0;(i<inpData.count()) && (!b);i++)
    b = (inpData.at(i)->type==dtype);

  return b;

}
*/

void qtCOFE::Task::printTask()  {
  printf ( "   Type   : '%s'\n",type.toAscii().data() );
  printf ( "   Name   : '%s'\n",name.toAscii().data() );
  printf ( "   Desc   : '%s'\n",desc.toAscii().data() );
  printf ( "   Section: '%s'\n",section.toAscii().data() );
  printf ( "   Icon   : '%s'\n",icon.toAscii().data() );
  printf ( "   Order  : %i\n",order );
  printf ( "\n   Input Data:\n" );
  for (int i=0;i<inpData.count();i++)
    printf ( "      %3i ['%s','%c',%i]\n",i+1,
             inpData.at(i)->type.toAscii().data(),
             inpData.at(i)->mode.toAscii(),
             inpData.at(i)->nmode );
  printf ( "\n   Output Data:\n" );
  for (int i=0;i<outData.count();i++)
    printf ( "      %3i ['%s','%c',%i]\n",i+1,
             outData.at(i)->type.toAscii().data(),
             outData.at(i)->mode.toAscii(),
             outData.at(i)->nmode );
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

const qtCOFE::TaskData *qtCOFE::DataModel::getTaskDataOut (
                                          const QString & taskType,
                                          const QString & dataType )  {
const  Task *task = getTask ( taskType );
TaskData *td = NULL;
  if (task)  {
    for (int i=0;(i<task->outData.count()) && (!td);i++)
      if (task->outData.at(i)->type==dataType)
        td = task->outData.at(i);
  }
  return td;
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

void qtCOFE::DataModel::printModel()  {
  printf ( " ------------------------------------------------------\n");
  printf ( "   TASKS\n" );
  printf ( " ------------------------------------------------------\n");
  for (int i=0;i<tasks.count();i++)  {
    printf ( "\n TASK #%i\n",i+1 );
    tasks.at(i)->printTask();
  }
}
