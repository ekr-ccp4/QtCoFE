// $Id: qtx_proxydialog.cpp $
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
//  **** Module  :  qtx_proxydialog <implementation>
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

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QList>
#include <QNetworkProxyFactory>

#include "qtx/qtx_proxydialog.h"

#define UNUSED_ARGUMENT(x) (void)x

qtx::ProxyDialog::ProxyDialog ( QWidget *parent ) : QDialog(parent)  {

  setWindowFlags ( Qt::Dialog | Qt::WindowStaysOnTopHint );

  setupDialog();
  setMinimumWidth ( 400 );
//  setWindowFlags  ( Qt::Dialog );
  setWindowTitle  ( "Proxy settings"  );

}

qtx::ProxyDialog::~ProxyDialog()  {}

void qtx::ProxyDialog::setupDialog()  {
QFrame      *f;
QVBoxLayout *vbox;
QGridLayout *gbox;
QHBoxLayout *hbox;
int          r = 0;

  gbox = new QGridLayout();

  proxy_type_cbx = new QComboBox();
  proxy_type_cbx->addItem ( "No proxy (direct connection)",
                            QNetworkProxy::NoProxy );
  proxy_type_cbx->addItem ( "HTTP proxy",QNetworkProxy::HttpProxy );
  proxy_type_cbx->addItem ( "HTTP caching proxy",
                                  QNetworkProxy::HttpCachingProxy );
  proxy_type_cbx->addItem ( "SOCKS5 proxy",QNetworkProxy::Socks5Proxy );
  gbox->addWidget ( new QLabel("Proxy type"),r,0,1,1 );
  gbox->addWidget ( proxy_type_cbx          ,r,1,1,1 );
  gbox->addWidget ( new QLabel(" ")         ,r,2,1,1 );

  r++;
  proxy_host_lbl = new QLabel("Proxy host");
  proxy_port_lbl = new QLabel("Port");
  gbox->addWidget ( new QLabel(" "),r,0,1,1 );
  gbox->addWidget ( proxy_host_lbl ,r,1,1,1 );
  gbox->addWidget ( proxy_port_lbl ,r,2,1,1 );
  proxy_host_edt = new QLineEdit();
  proxy_port_spx = new QSpinBox();
  proxy_port_spx->setRange ( 0,9999 );
  proxy_port_spx->setValue ( 8080   );
  proxy_port_spx->setAlignment ( Qt::AlignRight );
  r++;

  proxy_url_lbl = new QLabel("<i>http://</i>");
  proxy_url_lbl->setAlignment ( Qt::AlignCenter | Qt::AlignRight );
  gbox->addWidget ( proxy_url_lbl ,r,0,1,1 );
  gbox->addWidget ( proxy_host_edt,r,1,1,1 );
  gbox->addWidget ( proxy_port_spx,r,2,1,1 );
  gbox->setContentsMargins ( 0,0,0,0 );
  gbox->setColumnStretch ( 0,0 );
  gbox->setColumnStretch ( 1,100 );
  gbox->setColumnStretch ( 2,1   );

  vbox = new QVBoxLayout();
  vbox->addLayout  ( gbox,0 );
  vbox->addSpacing ( 20 );

  proxy_authentification_cbx =
                 new QCheckBox ( "requires user autentification" );
  proxy_usr_lbl = new QLabel ( "User Id");
  proxy_usr_edt = new QLineEdit();
  proxy_pwd_lbl = new QLabel ( "Password");
  proxy_pwd_edt = new QLineEdit();
  proxy_pwd_edt->setEchoMode ( QLineEdit::Password );

  gbox = new QGridLayout();
  gbox->addWidget ( proxy_authentification_cbx,0,0,1,2 );
  gbox->addWidget ( new QLabel(" "),0,2,1,1 );
  gbox->addWidget ( proxy_usr_lbl  ,1,0,1,1 );
  gbox->addWidget ( proxy_usr_edt  ,1,1,1,1 );
  gbox->addWidget ( new QLabel(" "),1,2,1,1 );
  gbox->addWidget ( proxy_pwd_lbl  ,2,0,1,1 );
  gbox->addWidget ( proxy_pwd_edt  ,2,1,1,1 );
  gbox->addWidget ( new QLabel(" "),2,2,1,1 );
  gbox->setContentsMargins ( 0,0,0,0 );
  gbox->setColumnStretch ( 0,1   );
  gbox->setColumnStretch ( 1,10  );
  gbox->setColumnStretch ( 2,100 );

  vbox->addLayout ( gbox,0 );

  f = new QFrame();
  f->setFrameStyle ( QFrame::Panel | QFrame::Raised );
  f->setLineWidth  ( 2 );
  f->setLayout ( vbox );

  vbox = new QVBoxLayout();
  vbox->addWidget ( f,0 );

  apply_btn  = new QPushButton ( "apply"  );
  cancel_btn = new QPushButton ( "cancel" );

  hbox = new QHBoxLayout();
  hbox->addWidget ( new QLabel(" "),100 );
  hbox->addWidget ( apply_btn ,1 );
  hbox->addWidget ( cancel_btn,1 );
  hbox->addWidget ( new QLabel(" "),100 );
  vbox->addLayout ( hbox,0 );

  setLayout ( vbox );

  QNetworkProxy proxy = QNetworkProxy::applicationProxy();
  proxy_host_edt->setText  ( proxy.hostName() );
  proxy_port_spx->setValue ( proxy.port()     );

  r = proxy_type_cbx->findData ( proxy.type() );
  if (r<=0)
    r = proxy_type_cbx->findData ( QNetworkProxy::HttpProxy );
  proxy_type_cbx->setCurrentIndex ( r );

  proxy_authentification_cbx->setChecked ( false );

  connect ( proxy_authentification_cbx,SIGNAL(clicked()),
            this,SLOT(show_authentification()) );
  connect ( apply_btn,SIGNAL(clicked()) ,this,SLOT(apply())  );
  connect ( cancel_btn,SIGNAL(clicked()),this,SLOT(reject()) );

  connect ( proxy_host_edt,SIGNAL(textChanged(QString)),
            this,SLOT(proxy_changed()) );
  connect ( proxy_port_spx,SIGNAL(valueChanged(int)),
            this,SLOT(proxy_changed(int)) );
  connect ( proxy_type_cbx,SIGNAL(currentIndexChanged(int)),
            this,SLOT(proxy_changed(int)) );

  show_authentification();
  proxy_changed();

}


void qtx::ProxyDialog::apply()  {
QNetworkProxy proxy ( (QNetworkProxy::ProxyType)proxy_type_cbx ->
                    itemData(proxy_type_cbx->currentIndex()).toInt() );

  if (proxy.type()!=QNetworkProxy::NoProxy)  {

    proxy.setHostName ( proxy_host_edt->text ()    );
    proxy.setPort     ( proxy_port_spx->value()    );

    if (proxy_authentification_cbx->isChecked())  {
      proxy.setUser     ( proxy_usr_edt->text() );
      proxy.setPassword ( proxy_pwd_edt->text() );
    }

  }

  QNetworkProxy::setApplicationProxy(proxy);

  accept();

}

void qtx::ProxyDialog::show_authentification()  {
bool checked = proxy_authentification_cbx->isChecked();

  proxy_usr_lbl->setVisible ( checked );
  proxy_pwd_lbl->setVisible ( checked );
  proxy_usr_edt->setVisible ( checked );
  proxy_pwd_edt->setVisible ( checked );

}

void qtx::ProxyDialog::proxy_changed ( int port )  {
UNUSED_ARGUMENT(port);

  if ((proxy_host_edt->text().simplified().size()>2) &&
      (proxy_port_spx->value()>0))
       apply_btn->setEnabled ( true  );
  else apply_btn->setEnabled ( false );

  if (proxy_type_cbx->itemData(proxy_type_cbx->currentIndex()).toInt()==
      QNetworkProxy::NoProxy)  {
    proxy_host_lbl->setVisible ( false );
    proxy_port_lbl->setVisible ( false );
    proxy_url_lbl ->setVisible ( false );
    proxy_host_edt->setVisible ( false );
    proxy_port_spx->setVisible ( false );
    proxy_authentification_cbx->setVisible ( false );
    proxy_usr_lbl->setVisible ( false );
    proxy_pwd_lbl->setVisible ( false );
    proxy_usr_edt->setVisible ( false );
    proxy_pwd_edt->setVisible ( false );
  } else  {
    proxy_host_lbl->setVisible ( true );
    proxy_port_lbl->setVisible ( true );
    proxy_url_lbl ->setVisible ( true );
    proxy_host_edt->setVisible ( true );
    proxy_port_spx->setVisible ( true );
    proxy_authentification_cbx->setVisible ( true );
    show_authentification();
  }

}
