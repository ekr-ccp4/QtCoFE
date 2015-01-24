// $Id: qtcofe.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module   :  qtcofe <implementation>
//       ~~~~~~~~~~
//  **** Project  :  CCP4 CoFE Project
//       ~~~~~~~~~~
//  **** Functions:  Main Program
//       ~~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//


#include <QApplication>
#include <QMessageBox>
#include <QCleanlooksStyle>
#include <QLibraryInfo>
#include <QDir>

#ifdef Q_OS_WIN
#include <io.h>
#else
#include <unistd.h>
#endif

#include "qtcofe_mainwindow.h"
#include "qtcofe_defs.h"

int qtcofe ( int argc, char *argv[] )  {
QApplication        app(argc,argv);
qtCOFE::MainWindow *w;
int                 rc;

  app.setOrganizationName ( qtCOFE_Company     );
  app.setApplicationName  ( qtCOFE_Application );

#ifndef Q_OS_MAC
  QApplication::setStyle ( new QCleanlooksStyle() );
#endif

  w = new qtCOFE::MainWindow();

  w->show();

  rc = app.exec();

  delete w;

  return rc;

}


int  main ( int argc, char *argv[] )  {
char *S;
char *cwd;
char *t;

#ifdef Q_OS_LINUX
  setenv ( "LIBOVERLAY_SCROLLBAR","0",1 );
#endif

  //  Calculate absolute path to qt plugins directory and add it
  //  to library path

  S    = new char[PATH_MAX+10];
  S[0] = 0;

  if (argv[0][0]=='.')  {
    cwd = getcwd ( NULL,0 );
    if (cwd)  {
      strcpy ( S,cwd );
      strcat ( S,"/" );
      if (argv[0][1]=='/')
            strcat ( S,argv[0]+2 );
      else  strcat ( S,argv[0]   );
    }
  } else  {
    strcpy ( S,argv[0] );
#ifdef Q_OS_WIN
    if (strlen(S)<3)                      S[0] = char(0);
    else if ((S[1]!=':') || (S[2]!='\\')) S[0] = char(0);
#else
    if (S[0]!='/')  S[0] = char(0);
#endif
  }

  if (S[0])  {
    t = strrchr ( S,'/' );
    if (!t)
      t = strrchr ( S,'\\' );
    if (t)  {
      t[1] = char(0);
      strcat ( S,qtCOFE_QtPlugins_RepPath   );
      strcat ( S,qtCOFE_QtPlugins_Directory );
    }
  } else  {
    t = getenv ( qtCOFE_CCP4_Env );
    if (t)  {
      strcpy ( S,t );
      strcat ( S,"/" );
      strcat ( S,qtCOFE_QtPlugins_Directory );
    }
  }

 QApplication::setLibraryPaths ( QStringList(S) );

#ifdef Q_OS_MAC
  QApplication::addLibraryPath ( qtCOFE_QtPlugins_RepPath );
#endif

  delete[] S;

  return qtcofe ( argc,argv );

}

