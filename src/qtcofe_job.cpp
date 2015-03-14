// $Id: qtcofe_job.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_job <implementation>
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

#include <QMessageBox>

#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"

#include "qtcofe_job.h"
#include "qtcofe_datamodel.h"

qtCOFE::JobData::JobData() : TaskData()  {}

qtCOFE::JobData::JobData ( TaskData *taskData )  {
  type  = taskData->type;  // "dtype_xxx"
  mode  = taskData->mode;  // modificator of data entity number (E,U,G)
  nmode = taskData->nmode; // data entity number
}

qtCOFE::JobData::~JobData() { clear(); }

void qtCOFE::JobData::clear()  {
  foreach (Metadata *m,metadata)
    if (m)  delete m;
  metadata.clear();
}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::E_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (nmode==jobData[0]->metadata.count())
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->metadata.count();
  if (nmode<=nm)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::G_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (jobData[0]->metadata.count()==nmode+1)
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->metadata.count();
  if (nm>nmode)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::U_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (jobData[0]->metadata.count()==nmode)
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->metadata.count();
  if (nm<nmode)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::isSuitable (
                                    const QList<JobData *> jobData )  {

  if (type=="dtype_dummy")     return Suitable;
  if (type=="dtype_any")       return Suitable;
  if (type!=jobData[0]->type)  return Unsuitable;

  switch (mode.toAscii())  {
    default  :
    case 'E' : return E_Suitable ( jobData );
    case 'G' : return G_Suitable ( jobData );
    case 'U' : return U_Suitable ( jobData );
  }

  return Unsuitable;

}

/*
int qtCOFE::JobData::suitability ( const JobData * jobData )  {

  if (type=="dtype_dummy")  return 1;
  if (type=="dtype_any")    return 1;
  if (type!=jobData->type)  return 0;

  return 1;

}
*/

int qtCOFE::indexOf ( const QString & dtype,
                      const QList<JobData *> & jobData )  {
int k = -1;
  for (int i=0;(i<jobData.count()) && (k<0);i++)
    if (jobData.at(i)->type==dtype)
      k = i;
  return k;
}


qtCOFE::Job::Job ( QObject *parent ) : QObject(parent)  {
  id       = 0;
  expanded = true;
}

qtCOFE::Job::Job ( const Task * task, QObject *parent )
           : QObject(parent)  {
  id       = 0;
  order    = 0;
  expanded = true;
  copy ( task );
}

qtCOFE::Job::Job ( const QJsonObject & jobData,
                   DataModel * dataModel,
                   QObject *parent )
           : QObject(parent)  {
  id       = 0;
  order    = 0;
  expanded = true;
  setJobData ( jobData,dataModel );
}

qtCOFE::Job::~Job() {
  clear();
}

void qtCOFE::Job::clear()  {
  foreach (JobData *jd,inpData)
    if (jd)  delete jd;
  inpData.clear();
  foreach (JobData *jd,outData)
    if (jd)  delete jd;
  outData.clear();
}

void qtCOFE::Job::copy ( const Task *task )  {

  clear();

  type    = task->type;
  name    = task->name;
  desc    = task->desc;
  section = task->section;
  icon    = task->icon;
  order   = task->order;

  foreach (TaskData *td,task->inpData)
    inpData.append ( new JobData(td) );

  foreach (TaskData *td,task->outData)
    outData.append ( new JobData(td) );

}

void qtCOFE::Job::setJobData ( const QJsonObject & jobData,
                               DataModel * dataModel )  {
QJsonArray  data = jobData.value("data").toArray();
const Task *task = NULL;

  clear();

  type = jobData.value("type").toString();
  name = jobData.value("name").toString();
  desc = jobData.value("desc").toString();
  id   = jobData.value("id").toDouble();

  for (int i=0;i<data.count();i++)  {
    QJsonArray dlist = data[i].toArray();
    if (dlist.count()>0)  {
      JobData *jd =new JobData();
      jd->type = dlist[0].toObject().value("type").toString();
      jd->copy ( dataModel->getTaskDataOut(type,jd->type) );
      for (int j=0;j<dlist.count();j++)  {
        Metadata *m = new Metadata();
        QJsonObject jobj = dlist[j].toObject();
        m->jobId = jobj.value("jobId").toDouble();
        m->desc  = jobj.value("desc" ).toString();
        m->fname = jobj.value("file" ).toString();
        if (jobj.keys().contains("columns",Qt::CaseInsensitive))  {
          QJsonArray clist = jobj.value("columns").toArray();
          for (int k=0;k<clist.count();k++)
            m->columns.append ( clist[k].toString() );
        }
        jd->metadata.append ( m );
      }
      outData.append ( jd );
    }
  }

  if (jobData.keys().contains("expanded",Qt::CaseInsensitive))
    expanded = jobData.value("expanded").toBool(true);

  task = dataModel->getTask ( type );
  if (task)  {
    if (name.isEmpty())  name = task->name;
    if (desc.isEmpty())  desc = task->desc;
    icon = task->icon;
    if (outData.isEmpty())  {
      foreach (TaskData *td,task->outData)  {
        JobData *jd = new JobData();
        jd->copy ( td );
        outData.append ( jd );
      }
    }
  }

}

qtCOFE::JobData::SUITABILITY qtCOFE::Job::isSuitable (
                          const QList<QList<JobData *> > & jobData )  {
JobData::SUITABILITY suitable  = JobData::Suitable;
bool                 ambiguous = false;

  for (int i=0;(i<inpData.count()) &&
               (suitable!=JobData::Unsuitable);i++)  {
    suitable = JobData::Unsuitable;
    for (int j=0;(j<jobData.count()) &&
                 (suitable==JobData::Unsuitable);j++)
      suitable = inpData.at(i)->isSuitable ( jobData[j] );
    if (suitable==JobData::Ambiguous)
      ambiguous = true;
  }

  if (suitable==JobData::Unsuitable)  return JobData::Unsuitable;
  if (ambiguous)                      return JobData::Ambiguous;
  return JobData::Suitable;

}

/*
int qtCOFE::Job::hasInput ( const QList<JobData *> & jobData )  {
int suitable = 1;

  for (int i=0;(i<inpData.count()) && (suitable!=0);i++)  {
    suitable = 0;
    for (int j=0;(j<jobData.count()) && (suitable==0);j++)
      suitable = inpData.at(i)->suitability ( jobData.at(j) );
  }

  return suitable;

/--
bool included = true;

  for (int i=0;(i<inpData.count()) && included;i++)
    included = (qtCOFE::indexOf(inpData.at(i)->type,jobData)>=0);

  for (int i=0;(i<inpData.count()) && (!included);i++)  {
//    QString dtype = inpData.at(i)->type;
    included = (inpData.at(i)->type=="dtype_dummy") ||
               (inpData.at(i)->type=="dtype_any");
  }

  if (included)  return 1;
  return 0;
--/

}

bool qtCOFE::Job::hasInput ( const JobData * jobData )  {
bool b = false;

  for (int i=0;(i<inpData.count()) && (!b);i++)
    b = (inpData.at(i)->type==jobData->type);

  return b;

}
*/
