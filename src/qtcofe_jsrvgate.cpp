// $Id: qtcofe_jsrvgate.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_jsrvgate <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::RVGate
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include "qtcofe_jsrvgate.h"

qtCOFE::RvGate::RvGate ( QObject *parent ) : QObject ( parent )  {}

qtCOFE::RvGate::~RvGate() {}

void qtCOFE::RvGate::closeWindow()  {  emit close_window(); }
void qtCOFE::RvGate::printWindow()  {  emit print_window(); }
void qtCOFE::RvGate::prefDialog ()  {  emit pref_dialog (); }

void qtCOFE::RvGate::buttonClicked ( QString command, QString data )  {
  emit button_clicked ( command,data );
}

