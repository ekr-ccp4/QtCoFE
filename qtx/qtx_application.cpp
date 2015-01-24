// $Id: qtx_application.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    09.01.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_application <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::Application
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include <QFileOpenEvent>
//#include <QMessageBox>

#include "qtx_application.h"

qtx::Application::Application ( int & argc, char ** argv )
                : QApplication(argc,argv)  {}

qtx::Application::~Application() {}

#ifdef Q_OS_MAC

bool qtx::Application::event ( QEvent *e )  {

  if (e->type()==QEvent::FileOpen) {
    QFileOpenEvent *fileOpen = static_cast<QFileOpenEvent *>(e);
    emit openFile ( fileOpen->file() );
    return true;
  }

  return QApplication::event(e);

}

#endif
