// $Id: qtcofe_server.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_server <interface>
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

#ifndef QTCOFE_SERVER_H
#define QTCOFE_SERVER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class QJsonObject;

namespace qtCOFE  {

  class Preferences;
  class Session;
/*
  enum SERVER_ACTION  {
    SERVER_Login,
    SERVER_RecoverLogin,
    SERVER_CreateNewUser,
    SERVER_GetListOfProjects,
    SERVER_AddProject,
    SERVER_RenameProject,
    SERVER_DeleteProject,
    SERVER_GetUserData,
    SERVER_SetUserData
  };
*/
  enum SERVER_RC  {
    SERVER_RC_Ok,
    SERVER_RC_UnknownAction,
    SERVER_RC_JsonParseErr,
    SERVER_RC_NoAnswer,
    SERVER_RC_NotJsonAnswer,
    SERVER_RC_Unconfigured,
    SERVER_RC_NoMasterPath
  };

  class Server : public QObject  {
  Q_OBJECT

    public:
      Server ( Preferences *prefs,
               Session     *ssn,
               QObject     *parent = 0 );
      virtual ~Server();

      SERVER_RC call ( QString          action,
                       const QJsonObject &data,
                       QJsonObject       &reply );

      void makeErrorMessage ( QWidget *parent=0 );

    protected:
      Preferences *preferences;
      Session     *session;
      SERVER_RC    errorCode;
      QString      errMsg;

  };

}

#endif // QTCOFE_SERVER_H
