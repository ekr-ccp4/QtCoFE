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

#include "qtcofe_job.h"
#include "qtcofe_datamodel.h"


qtCOFE::Job::Job ( QObject *parent ) : QObject(parent)  {
  id       = 0;
  expanded = true;
}

qtCOFE::Job::Job ( const QJsonObject & jobData,
                   DataModel * dataModel,
                   QObject *parent )
           : QObject(parent)  {
  id       = 0;
  expanded = true;
  setJobData ( jobData,dataModel );
}

qtCOFE::Job::~Job() {
}

void qtCOFE::Job::setJobData ( const QJsonObject & jobData,
                               DataModel * dataModel )  {
const Task *task = NULL;

  type = jobData.value("type").toString();
  name = jobData.value("name").toString();
  desc = jobData.value("desc").toString();
  id   = jobData.value("id").toDouble();
  if (jobData.keys().contains("expanded",Qt::CaseInsensitive))
    expanded = jobData.value("expanded").toBool(true);

  task = dataModel->getTask ( type );
  if (task)  {
    if (name.isEmpty())  name = task->name;
    if (desc.isEmpty())  desc = task->desc;
    icon = task->icon;
  }
}

