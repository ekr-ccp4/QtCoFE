// $Id: qtcofe_page.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Page
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <stdio.h>

#include "qtcofe_page.h"
#include "qtcofe_datamodel.h"
#include "qtcofe_preferences.h"
#include "qtcofe_session.h"
#include "qtx/qtx_lineedit.h"


qtCOFE::Page::Page ( DataModel      *dm,
                     QSettings      *stn,
                     QWidget        *parent,
                     Qt::WindowFlags flags )
            : QWidget(parent,flags)  {
  dataModel   = dm;
  preferences = dataModel->getPreferences();
  session     = dataModel->getSession    ();
  settings    = stn;
//  setStyleSheet ( preferences->getFontSizeStyleSheet(1.0) );
}

qtCOFE::Page::~Page() {
}


void qtCOFE::Page::makeFormLine ( QGridLayout   *gbox,
                                  int             row,
                                  const QString & name,
                                  qtx::LineEdit *&edt,
                                  int             size,
                                  const QString & placeholder )  {
QLabel *label;
QString ss = preferences->getFontSizeStyleSheet(1.25);
int     leh;

  label = new QLabel ( name );
  label->setStyleSheet ( ss );
  gbox->addWidget ( label,row,0,1,1 );

  edt = new qtx::LineEdit ( "",size );
  edt->setStyleSheet ( ss + "font-style:italic;" );
  leh = 2.5*preferences->getFontPixelSize();
  edt->setMinimumHeight ( leh );
  edt->setMaximumHeight ( leh );
  edt->setPlaceholderText ( placeholder );
  gbox->addWidget ( edt,row,1,1,1 );

}

QPushButton *qtCOFE::Page::makeDarkButton ( const QString & text )  {
QPushButton *btn = new QPushButton ( text );
int          leh = 2.5*preferences->getFontPixelSize();
QString      ss  = QString(
  "QPushButton{color: rgb(250,250,250); border-radius:6px; "
  "border-width:0px; border-style:outset; border-color: rgb(75,75,75);"
  "padding-left: %1px; padding-right: %2px; "
  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
  "stop: 0 #A0A0A0, stop: 0.4 #808080, "
      "stop: 0.6 #606060, stop: 1.0 #404040);").arg(leh/2).arg(leh/2) +
      preferences->getFontSizeStyleSheet(1.0) + "}" +
  "QPushButton:hover{color: rgb(255,255,0); border-width:1px;}";

  btn->setMinimumHeight ( leh );
  btn->setMaximumHeight ( leh );
  btn->setStyleSheet ( ss );
  btn->setGeometry ( 10,0,10,0 );

  return btn;

}

QPushButton *qtCOFE::Page::makeLightButton ( const QString & text )  {
QPushButton *btn = new QPushButton ( text );
int          leh = 2.5*preferences->getFontPixelSize();
QString      ss  = QString(
  "QPushButton{color: rgb(0,0,0); border-radius:6px; "
  "border-width:0px; border-style:outset; border-color: rgb(75,75,75);"
  "padding-left: %1px; padding-right: %2px; "
  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
  "stop: 0 #D0D0D0, stop: 0.4 #B0B0B0, "
  "stop: 0.6 #909090, stop: 1.0 #707070);").arg(leh/2).arg(leh/2) +
      preferences->getFontSizeStyleSheet(1.0) + "}" +
  "QPushButton:hover{color: rgb(0,0,127); border-width:1px;}";                   ;

  btn->setMinimumHeight ( leh );
  btn->setMaximumHeight ( leh );
  btn->setStyleSheet ( ss );
  btn->setGeometry ( 10,0,10,0 );

  return btn;

}
