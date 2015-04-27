// $Id: qtcofe_server.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_server <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Server
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QProcess>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>

#include "qjson/QJsonDocument.h"
#include "qjson/QJsonObject.h"
#include "qjson/QJsonParseError.h"

#include "qtcofe_server.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_preferences.h"
#include "qtcofe_session.h"


qtCOFE::Server::Server ( Preferences *prefs, Session *ssn,
                         QObject *parent )
              : QObject(parent)  {
  preferences = prefs;
  session     = ssn;
  errorCode   = SERVER_RC_Ok;
}

qtCOFE::Server::~Server() {
}

qtCOFE::SERVER_RC qtCOFE::Server::call ( QString           action,
                                         const QJsonObject  &data,
                                         QJsonObject        &reply )  {
QString  processor    = preferences->getProcessingGate();
QString  projectsPath = preferences->getProjectsPath();
//QString  binPath      = preferences->getBinPath();

  if (processor.isEmpty())  {
    errorCode = SERVER_RC_Unconfigured;
    return errorCode;
  }
  if (projectsPath.isEmpty())  {
    errorCode = SERVER_RC_NoProjectsPath;
    return errorCode;
  }
  /*
  if (binPath.isEmpty())  {
    errorCode = SERVER_RC_NoBinPath;
    return errorCode;
  }
  */

  errorCode = SERVER_RC_Ok;
  errMsg.clear();

  QProcess process;
  QJsonObject jsonObject;
  jsonObject.insert ( "action"       ,action              );
  jsonObject.insert ( "login"        ,session->loginName  );
  jsonObject.insert ( "pwd"          ,session->password   );
  jsonObject.insert ( "projects_path",projectsPath        );
//  jsonObject.insert ( "bin_path"     ,binPath             );
  jsonObject.insert ( "session"      ,session->sessionID  );
  jsonObject.insert ( "project"      ,session->projectID  );
  jsonObject.insert ( "job"          ,session->jobID      );

  if (!data.empty())
    jsonObject.insert ( "data",data );
  QJsonDocument jsonDoc(jsonObject);

//  QMessageBox::information ( NULL,"",
//                             jsonDoc.toJson(QJsonDocument::Indented) );

  process.start ( "python",QStringList() << processor <<
                    QString(jsonDoc.toJson(QJsonDocument::Compact)) );
//  process.waitForStarted();
  process.waitForFinished();
  process.waitForReadyRead();
  QJsonParseError e;
  QString         answer = process.readAllStandardOutput().trimmed();
  if (answer.startsWith('{') || answer.startsWith('['))  {
    jsonDoc = QJsonDocument::fromJson ( answer.toAscii(),&e );
    if (jsonDoc.isEmpty())  {
      errorCode = SERVER_RC_JsonParseErr,
      errMsg    = e.errorString();
    } else  {
      reply = jsonDoc.object();
    }
  } else  {
    if (answer.isEmpty())  {
      errorCode = SERVER_RC_NoAnswer;
      errMsg    = process.readAllStandardError().trimmed();
    } else  {
      errorCode = SERVER_RC_NotJsonAnswer;
      errMsg    = answer;
    }
  }

  return errorCode;

}

void qtCOFE::Server::makeErrorMessage ( QWidget *parent )  {
QString message;

  switch (errorCode)  {
    case SERVER_RC_UnknownAction : message = QString(
            "Unknown action code for server call.<p>"
            "This is a program error, please report "
            "to developers." );
          break;
    case SERVER_RC_JsonParseErr : message = QString(
            "Reply from server cannot be parsed, error:<p>" +
            errMsg +
            "<p>This is a program error, please report "
            "to developers." );
        break;
    case SERVER_RC_NoAnswer : message = QString(
            "No answer from server:<p><i><pre>" +
             errMsg +
            "</pre></i><p>This is a program error, please report "
            "to developers." );
        break;
    case SERVER_RC_NotJsonAnswer : message = QString(
            "Reply from server:<p><i><pre>" +
            errMsg +
            "</pre></i><p>is misformatted. This is a program error, "
            "please report to developers." );
      break;
    case SERVER_RC_Unconfigured : message = QString(
            "Server URI is not set. Please open<br>"
            "<i>Preferences</i> and set up server URI." );
          break;
    case SERVER_RC_NoProjectsPath : message = QString(
            "Projects Path is not set. Please open<br>"
            "<i>Preferences</i> and set up Projects data path." );
          break;
    /*
    case SERVER_RC_NoBinPath : message = QString(
            "Master Path to executables is not set. Please open<br>"
            "<i>Preferences</i> and set up Path to executables." );
          break;
    */
    default : message = QString(
            "Unknown error code at server call (%1).<p>"
            "This is a program error, please report "
            "to developers." ).arg(int(errorCode));
  }

  QMessageBox::information ( parent,"Server call error",message );

}
