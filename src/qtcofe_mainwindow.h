// $Id: qtcofe_mainwindow.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_mainwindow <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::MainWindow
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_MAINWINDOW_H
#define QTCOFE_MAINWINDOW_H

#include "qtcofe_pages.h"

namespace qtCOFE  {

  class MainWindow : public Pages  {
  Q_OBJECT

    public:
      MainWindow ( QWidget *parent=0, Qt::WindowFlags flags = 0 );
      virtual ~MainWindow();

  };

}

#endif // QTCOFE_MAINWINDOW_H
