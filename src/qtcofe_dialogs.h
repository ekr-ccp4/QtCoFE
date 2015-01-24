// $Id: qtcofe_dialogs.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_dialogs <interface>
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

#ifndef QTCOFE_DIALOGS_H
#define QTCOFE_DIALOGS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE


namespace qtCOFE  {

  class Text2Dialog : public QDialog  {
  Q_OBJECT

    public:

      Text2Dialog ( QWidget *parent, const QString dlgTitle,
                    Qt::WindowFlags f = 0 );
      ~Text2Dialog();

      void setPrompt       ( const QString prompt );
      void setInput1       ( const QString label, const QString value,
                             const QString regexp );
      void setInput2       ( const QString label, const QString value,
                             const QString regexp );
      void setOkButton     ( const QString text,
                             const QString iconPath="" );
      void setCancelButton ( const QString text,
                             const QString iconPath="" );

      QString getInput1();
      QString getInput2();

    protected:
      QLabel      *prompt_lbl;
      QLabel      *text1_lbl;
      QLabel      *text2_lbl;
      QLineEdit   *text1_edt;
      QLineEdit   *text2_edt;
      QPushButton *ok_btn;
      QPushButton *cancel_btn;

      void makeLayout();
      void setInput ( QLabel       *lbl,
                      QLineEdit    *edt,
                      const QString label,
                      const QString value,
                      const QString regexp );

  };

}

#endif // QTCOFE_DIALOGS_H
