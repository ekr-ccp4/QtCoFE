// $Id: qtcofe_session.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_preferences <interface>
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

#ifndef QTCOFE_SESSION_H
#define QTCOFE_SESSION_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace qtCOFE  {

  class Session : public QObject  {
  Q_OBJECT

    public:
      QString  loginName;
      QString  password;
      QString  sessionID;
      QString  projectID;
      QString  projectPath;
      int      jobID;
      bool     keepLogged;
      bool     loggedIn;

      Session ( QObject *parent = 0 );
      ~Session();

      void readSettings  ( QSettings *settings );
      void writeSettings ( QSettings *settings );

  };

}

#endif // QTCOFE_SESSION_H
