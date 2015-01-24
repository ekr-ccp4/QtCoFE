// $Id: qtcofe_dialogs.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialogs <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE dialog classes
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>

#include "qtcofe_dialogs.h"

qtCOFE::Text2Dialog::Text2Dialog ( QWidget        *parent,
                                   const QString   dlgTitle,
                                   Qt::WindowFlags f )
                   : QDialog ( parent,f )  {
  setWindowTitle ( dlgTitle );
  makeLayout();
}

qtCOFE::Text2Dialog::~Text2Dialog()  {}

void qtCOFE::Text2Dialog::setPrompt ( const QString prompt )  {
  if (prompt_lbl)  prompt_lbl->setText ( prompt );
}


void qtCOFE::Text2Dialog::setInput ( QLabel       *lbl,
                                     QLineEdit    *edt,
                                     const QString label,
                                     const QString value,
                                     const QString regexp )  {

  if (label=="#")   {
    if (lbl)  lbl->setVisible ( false );
    if (edt)  edt->setVisible ( false );
  } else  {
    if (lbl)  lbl->setText ( label );
    if (edt)  edt->setText ( value );

    if (!regexp.isEmpty())  {
      QRegExp     rx(regexp);
      QValidator *validator = new QRegExpValidator(rx, this);
      edt->setValidator(validator);
    }
  }

}

void qtCOFE::Text2Dialog::setInput1 ( const QString label,
                                      const QString value,
                                      const QString regexp )  {
  setInput ( text1_lbl,text1_edt,label,value,regexp );
}

void qtCOFE::Text2Dialog::setInput2 ( const QString label,
                                      const QString value,
                                      const QString regexp )  {
  setInput ( text2_lbl,text2_edt,label,value,regexp );
}

void qtCOFE::Text2Dialog::setOkButton ( const QString text,
                                        const QString iconPath )  {
  if (ok_btn)  {
    ok_btn->setText ( text );
    if (!iconPath.isEmpty())
      ok_btn->setIcon ( QIcon(iconPath) );
  }
}

void qtCOFE::Text2Dialog::setCancelButton ( const QString text,
                                            const QString iconPath )  {
  if (cancel_btn)  {
    cancel_btn->setText ( text );
    if (!iconPath.isEmpty())
      cancel_btn->setIcon ( QIcon(iconPath) );
  }
}

QString qtCOFE::Text2Dialog::getInput1()  {
  if (text1_edt)  return text1_edt->text();
  return QString();
}

QString qtCOFE::Text2Dialog::getInput2()  {
  if (text2_edt)  return text2_edt->text();
  return QString();
}


void qtCOFE::Text2Dialog::makeLayout()  {
QGridLayout *gbox;
QHBoxLayout *hbox;
QWidget     *w;

  gbox = new QGridLayout();

  prompt_lbl = new QLabel(" ");
  gbox->addWidget ( prompt_lbl,0,0,1,2 );

  text1_lbl = new QLabel(" ");
  text1_edt = new QLineEdit();
  gbox->addWidget ( text1_lbl,1,0,1,1 );
  gbox->addWidget ( text1_edt,1,1,1,1 );

  text2_lbl = new QLabel(" ");
  text2_edt = new QLineEdit();
  gbox->addWidget ( text2_lbl,2,0,1,1 );
  gbox->addWidget ( text2_edt,2,1,1,1 );

  w          = new QWidget();
  w->setSizePolicy ( QSizePolicy::Expanding,QSizePolicy::Minimum );
  ok_btn     = new QPushButton("Ok");
  cancel_btn = new QPushButton("Cancel");
  hbox = new QHBoxLayout();
  hbox->addWidget ( w       ,100 );
  hbox->addWidget ( cancel_btn,1 );
  hbox->addWidget ( ok_btn    ,1 );
  gbox->addLayout ( hbox,3,0,1,2 );

  connect ( ok_btn    ,SIGNAL(clicked()),this,SLOT(accept()) );
  connect ( cancel_btn,SIGNAL(clicked()),this,SLOT(reject()) );

  setLayout ( gbox );

}

