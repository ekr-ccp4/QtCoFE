// $Id: qtx_application.h $
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
//  **** Module  :  qtx_application <interface>
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

#ifndef qtx_APPLICATION_H
#define qtx_APPLICATION_H

#include <QApplication>

namespace qtx {

  class Application : public QApplication  {
  Q_OBJECT

    public:
      Application ( int & argc, char ** argv );
      ~Application();

    signals:
      void openFile ( const QString fileName );

    protected:
#ifdef Q_OS_MAC
      virtual bool event ( QEvent *e );
#endif

  };

}

#endif // qtx_APPLICATION_H
