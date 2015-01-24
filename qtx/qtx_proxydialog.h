// $Id: qtx_proxydialog.h $
//
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
//  **** Module  :  qtx_proxydialog <interface>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::ProxyDialog
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#ifndef qtx_PROXY_DIALOG_H
#define qtx_PROXY_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QLabel;
class QPushButton;
class QComboBox;
QT_END_NAMESPACE

namespace qtx  {

  class ProxyDialog : public QDialog  {
  Q_OBJECT

   public:
     ProxyDialog ( QWidget *parent=0 );
     ~ProxyDialog();

     void setupDialog();

   private slots:
     void apply                ();
     void show_authentification();
     void proxy_changed        ( int port=0 );

   private:
     QLabel      *proxy_host_lbl;
     QLabel      *proxy_port_lbl;
     QLabel      *proxy_url_lbl;
     QLabel      *proxy_usr_lbl;
     QLabel      *proxy_pwd_lbl;
     QComboBox   *proxy_type_cbx;
     QLineEdit   *proxy_host_edt;
     QSpinBox    *proxy_port_spx;
     QLineEdit   *proxy_usr_edt;
     QLineEdit   *proxy_pwd_edt;
     QCheckBox   *proxy_authentification_cbx;
     QPushButton *apply_btn;
     QPushButton *cancel_btn;

  };

}

#endif // qtx_PROXY_DIALOG_H
