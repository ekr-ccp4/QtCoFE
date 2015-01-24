// $Id: qtcofe_session.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_session <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Session
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QObject>
#include <QSettings>

#include "qtcofe_session.h"
#include "qtcofe_defs.h"

qtCOFE::Session::Session ( QObject *parent ) : QObject(parent)  {
  projectID   = "default";
  projectPath = "default";
  jobID       = 0;
  keepLogged  = true;
  loggedIn    = false;
}

qtCOFE::Session::~Session()  {}


void qtCOFE::Session::readSettings  ( QSettings *settings )  {
  loginName  = settings->value(qtCOFE_SET_LoginName  ,loginName  ).toString();
  projectID  = settings->value(qtCOFE_SET_ProjectID  ,projectID  ).toString();
  projectPath= settings->value(qtCOFE_SET_ProjectPath,projectPath).toString();
  password   = settings->value(qtCOFE_SET_Password   ,password   ).toString();
  jobID      = settings->value(qtCOFE_SET_JobID      ,jobID      ).toInt ();
  keepLogged = settings->value(qtCOFE_SET_KeepLogged ,keepLogged ).toBool();
  loggedIn   = settings->value(qtCOFE_SET_LoggedIn   ,loggedIn   ).toBool();
}

void qtCOFE::Session::writeSettings ( QSettings *settings )  {
  settings->setValue ( qtCOFE_SET_LoginName  ,loginName   );
  settings->setValue ( qtCOFE_SET_ProjectID  ,projectID   );
  settings->setValue ( qtCOFE_SET_ProjectPath,projectPath );
  settings->setValue ( qtCOFE_SET_Password   ,password    );
  settings->setValue ( qtCOFE_SET_JobID      ,jobID       );
  settings->setValue ( qtCOFE_SET_KeepLogged ,keepLogged  );
  settings->setValue ( qtCOFE_SET_LoggedIn   ,loggedIn    );
}

