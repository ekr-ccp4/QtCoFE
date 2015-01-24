// $Id: qtcofe_mainwindow.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_mainwindow <implementation>
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


#include "qtcofe_mainwindow.h"


qtCOFE::MainWindow::MainWindow ( QWidget        *parent,
                                 Qt::WindowFlags flags )
                  : Pages(parent,flags)  {
}

qtCOFE::MainWindow::~MainWindow() {
}

