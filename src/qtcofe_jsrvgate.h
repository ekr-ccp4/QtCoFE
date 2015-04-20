// $Id: qtcofe_jsrvgate.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_jsrvgate <interface>
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

#ifndef QTCOFE_JSRVGATE_H
#define QTCOFE_JSRVGATE_H

#include <QObject>

namespace qtCOFE  {

  class RvGate : public QObject  {
  Q_OBJECT

    public:
      RvGate ( QObject *parent=0 );
      virtual ~RvGate();

    signals:
      void close_window   ();
      void print_window   ();
      void pref_dialog    ();
      void button_clicked ( QString command, QString data );

    public slots:
      void closeWindow   ();
      void printWindow   ();
      void prefDialog    ();
      void buttonClicked ( QString command, QString data );

  };

}

#endif // QTCOFE_JSRVGATE_H
