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
#include <QTreeWidgetItem>
#include <QProgressBar>

#include "qjson/QJsonObject.h"
#include "qjson/QJsonArray.h"

#include "qtcofe_job.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_defs.h"


// =================================================================

qtCOFE::JobData::JobData() : TaskData()  {
  disambiguated = false;
}

qtCOFE::JobData::JobData ( TaskData *taskData )  {
  disambiguated = false;
  type    = taskData->type;  // "dtype_xxx"
  subtype = taskData->subtype;
  mode    = taskData->mode;  // modificator of data entity number (E,U,G)
  nmode   = taskData->nmode; // data entity number
}

qtCOFE::JobData::~JobData() { clear(); }

void qtCOFE::JobData::clear()  {
  disambiguated = false;
  foreach (Component *c,components)
    if (c)  delete c;
  components.clear();
}

int qtCOFE::JobData::n_suitable ( JobData *jobData )  {
int nm = 0;

  if (subtype=="*")
    return jobData->components.count();

  if (jobData->type==type)
    for (int i=0;i<jobData->components.count();i++)
      if (subtype.contains(jobData->components[i]->subtype) ||
          jobData->components[i]->subtype.contains('*'))
        nm++;

  return nm;

}

/*
qtCOFE::JobData::SUITABILITY qtCOFE::JobData::E_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (nmode==jobData[0]->components.count())
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->components.count();
  if (nmode<=nm)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::G_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (jobData[0]->components.count()==nmode+1)
    return Suitable;

  if ((jobData[0]->components.count()>nmode) && jobData[0]->disambiguated)
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->components.count();
  if (nm>nmode)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::U_Suitable (
                                    const QList<JobData *> jobData )  {
int nm;

  if (jobData[0]->components.count()==nmode)
    return Suitable;

  if ((jobData[0]->components.count()<nmode) && jobData[0]->disambiguated)
    return Suitable;

  nm = 0;
  for (int i=0;i<jobData.count();i++)
    nm += jobData[i]->components.count();
  if (nm<nmode)  return Ambiguous;

  return Unsuitable;

}
*/

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::E_Suitable (
                                    const QList<JobData *> jobData )  {
int nm = n_suitable(jobData[0]);

  if (nm==nmode)
    return Suitable;

  for (int i=1;i<jobData.count();i++)
    nm += n_suitable(jobData[i]);
  if (nmode<=nm)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::G_Suitable (
                                    const QList<JobData *> jobData )  {
int nm = n_suitable(jobData[0]);

  if (nm==nmode+1)
    return Suitable;

  if ((nm>nmode) && jobData[0]->disambiguated)
    return Suitable;

  for (int i=1;i<jobData.count();i++)
    nm += n_suitable(jobData[i]);
  if (nm>nmode)  return Ambiguous;

  return Unsuitable;

}

qtCOFE::JobData::SUITABILITY qtCOFE::JobData::U_Suitable (
                                    const QList<JobData *> jobData )  {
int nm = n_suitable(jobData[0]);

  if (nm==nmode)
    return Suitable;

  if ((nm<nmode) && jobData[0]->disambiguated)
    return Suitable;

  for (int i=1;i<jobData.count();i++)
    nm += n_suitable(jobData[i]);
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

int qtCOFE::indexOf ( const QString & dtype,
                      const QList<JobData *> & jobData )  {
int k = -1;
  for (int i=0;(i<jobData.count()) && (k<0);i++)
    if (jobData.at(i)->type==dtype)
      k = i;
  return k;
}

int qtCOFE::indexOf ( const QString & dtype,
                      const QList<QList<JobData *> > & jobData )  {
int k = -1;
  for (int i=0;(i<jobData.count()) && (k<0);i++)
    if (jobData[i][0]->type==dtype)
      k = i;
  return k;
}


// =================================================================

qtCOFE::Job::Job ( QObject *parent ) : QObject(parent)  {
  treeItem = NULL;
  init();
}

qtCOFE::Job::Job ( const Task * task, QObject *parent )
           : QObject(parent)  {
  treeItem = NULL;
  init();
  copy ( task );
}

qtCOFE::Job::Job ( const QJsonObject & jobData,
                   DataModel * dataModel,
                   QObject *parent )
           : QObject(parent)  {
  treeItem = NULL;
  init();
  setJobData ( jobData,dataModel );
}

qtCOFE::Job::~Job() {
  clear();
}

void qtCOFE::Job::init()  {
  id          = 0;
  status      = qtCOFE_JOB_Idle;
  order       = 0;
  expanded    = true;
  progressBar = NULL;
}

void qtCOFE::Job::clear()  {
  foreach (JobData *jd,inpData)
    if (jd)  delete jd;
  inpData.clear();
  foreach (JobData *jd,outData)
    if (jd)  delete jd;
  outData.clear();
  init();
}

void qtCOFE::Job::copy ( const Task *task )  {

  clear();

  if (task)  {

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

}

void qtCOFE::Job::setJobData ( const QJsonObject & jobData,
                               DataModel * dataModel )  {
QJsonArray  data = jobData.value("data").toArray();
const Task *task = NULL;

  clear();

  type   = jobData.value("type"  ).toString();
  name   = jobData.value("name"  ).toString();
  desc   = jobData.value("desc"  ).toString();
  id     = jobData.value("id"    ).toDouble();
  status = jobData.value("status").toDouble();
  showStatus();

  for (int i=0;i<data.count();i++)  {
    QJsonArray dlist = data[i].toArray();
    if (dlist.count()>0)  {

      JobData *jd =new JobData();
      jd->disambiguated = (type==qtCOFE_TASK_Disambiguator);
      jd->type = dlist[0].toObject().value("type").toString();
      jd->copy ( dataModel->getTaskDataOut(type,jd->type) );

      for (int j=0;j<dlist.count();j++)  {

        Component *c = new Component();

        QJsonObject jobj = dlist[j].toObject();
        c->jobId   = jobj.value("jobId"  ).toDouble();
        c->subtype = jobj.value("subtype").toString();
        c->dname   = jobj.value("dname"  ).toString();
        c->desc    = jobj.value("desc"   ).toString();

//printf ( " p1\n"); fflush(stdout);
        QJsonArray flist = jobj.value("files").toArray();
        for (int k=0;k<flist.count();k++)
          c->fnames.append ( flist[k].toString() );

        QJsonArray mlist = jobj.value("metadata").toArray();
        for (int k=0;k<mlist.count();k++)  {
          QJsonObject meta = mlist[k].toObject();
          c->metadata[meta.value("key").toString()] =
                                       meta.value("value").toString();
        }
        /*
        for (int k=0;k<mlist.count();k++)  {
          QJsonArray  meta = mlist[k].toArray();
          QStringList metadata;
          for (int m=0;m<meta.count();m++)
            metadata.append ( meta[m].toString() );
          c->metadata.append ( metadata );
        }
        */

        jd->components.append ( c );

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


qtCOFE::JobData::SUITABILITY qtCOFE::Job::isInputSuitable (
                          int   inpNo,
                          const QList<QList<JobData *> > & jobData )  {
JobData::SUITABILITY suitable  = JobData::Unsuitable;

  for (int j=0;(j<jobData.count()) &&
               (suitable==JobData::Unsuitable);j++)
    suitable = inpData.at(inpNo)->isSuitable ( jobData[j] );

  return suitable;

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

void qtCOFE::Job::getOutputDataSpecs ( int       outNo,
                                       QString   subtype,
                                       QString & jobName,
                                       QString & dataName,
                                       QString & desc,
                                       int     & nSets)  {

  jobName = name;
  desc.clear();
  if (outData[outNo]->components.isEmpty())  {
    dataName = "<< in promise >>";
    nSets = 1;
  } else  {
    dataName.clear();
    nSets = 0;
    for (int i=0;i<outData[outNo]->components.count();i++)  {
      Component *c = outData[outNo]->components[i];
//      if ((c->subtype=="*") || (subtype=="*") || (c->subtype==subtype)) {
        if (!dataName.isEmpty())  {
          dataName.append ( "\n" );
          desc    .append ( "\n" );
        }
        dataName.append ( c->dname + "/" + c->subtype + "//" + subtype );
        desc.append     ( c->desc  );
        nSets++;
//      }
    }
//    nSets = outData[outNo]->components.count();
  }

}

void qtCOFE::Job::setProgressBar ( QProgressBar *pbar )  {
  progressBar = pbar;
  showStatus();
}

void qtCOFE::Job::copyTreeData ( Job * job )  {
  treeItem    = job->treeItem;
  progressBar = job->progressBar;
}

void qtCOFE::Job::showStatus()  {

  if (progressBar)  {

    if (status==qtCOFE_JOB_Starting)  {
      progressBar->setRange   ( 1,10 );
      progressBar->setVisible ( true );
    } else if ((qtCOFE_JOB_Running<=status) && (status<qtCOFE_JOB_Done)) {
      if (progressBar->maximum()!=0)
        progressBar->setRange ( 0,0 );
      progressBar->setVisible ( true );
    } else
      progressBar->setVisible ( false );

  }

}

