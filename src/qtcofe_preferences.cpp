// $Id: qtcofe_preferences.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_preferences <implementation>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::Preferences
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#include <QDialog>
#include <QTableWidget>
#include <QSettings>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QFontInfo>
#include <QMessageBox>
#include <QProcess>
#include <QApplication>

#include "qtx/qtx_lineedit.h"
#include "qtcofe_preferences.h"
#include "qtcofe_defs.h"
#include "qtcofe_srvdefs.h"

qtCOFE::Preferences::Preferences ( QWidget *parent ) : QDialog(parent)  {
QDir     dir ( QApplication::applicationDirPath() );
QString  ccp4MasterPath;
QChar    dirsep = QDir::separator();

  fontChanged = false;

  treeWidth  = 300;
  plotHeight = 400;
  plotWidth  = 505;

//  maxDecorationSize = 1000;

  refreshPeriod = qtCOFE_DefaultRefreshPeriod;
  refreshMode   = qtCOFE_SmartRefresh;

#ifdef Q_OS_MAC
  useSystemBrowser = false;
#else
  useSystemBrowser = true;
#endif
//  gdExtSelection = false;         // graph data extended selection
//  cfgFromCCP4gui = false;

  if (dir.cdUp())  {
    ccp4Path = dir.canonicalPath();
    if (!ccp4Path.endsWith(dirsep))
      ccp4Path.append ( dirsep );
  }

  if (dir.cdUp())  {
    ccp4MasterPath = dir.canonicalPath();
  } else if (!ccp4Path.isEmpty())  {
    dir.setPath ( ccp4Path );
    dir.cdUp();
    ccp4MasterPath = dir.canonicalPath();
  }

  if ((!ccp4MasterPath.isEmpty()) && (!ccp4MasterPath.endsWith(dirsep)))
    ccp4MasterPath.append ( dirsep );


  procPath       = "";
  masterDataPath = "";
  binPath        = "";

#ifdef Q_OS_MAC
  binPath     = ccp4Path + "bin/";
  cootPath    = ccp4Path + "coot.app";
  ccp4mgPath  = ccp4Path + "QtMG.app";
  viewhklPath = ccp4Path + "ViewHKL.app";
  browserPath = "/Applications/Safari.app";
  checkMacVersion();
#elif defined(Q_OS_WIN)

  cootPath   = "runwincoot.bat";
  ccp4mgPath = "winccp4mg.exe";

  if (!ccp4mgPath.isEmpty())  {
    QStringList lst = QDir(ccp4MasterPath).entryList (
                                            QStringList() << "WinCoot*" );
    if (lst.count()>0)  {
      cootPath = ccp4MasterPath + lst.at(lst.count()-1) + dirsep +
                                                     "runwincoot.bat";
    } else {
      cootPath = "C:\\WinCoot\\runwincoot.bat";
      if (!QFileInfo(cootPath).exists())  cootPath = "runwincoot.bat";
    }
    lst = QDir(masterPath).entryList ( QStringList() << "CCP4MG*" );
    if (lst.count()>0)
      ccp4mgPath = masterPath + lst.at(lst.count()-1) + dirsep +
                                                     "winccp4mg.exe";
    else  {
      ccp4mgPath = "C:\\CCP4MG\\winccp4mg.exe";
      if (!QFileInfo(ccp4mgPath).exists())  ccp4mgPath = "winccp4mg.exe";
    }
  }
  if (!ccp4Path.isEmpty())  {
    binPath     = ccp4Path + "bin" +  dirsep;
    viewhklPath = binPath  + "viewhkl.exe";
  } else
    viewhklPath = "viewhkl.exe";
  browserPath = "C:\\Program Files\\Internet Explorer\\iexplore.exe";
  oldMac      = false;

#else

  cootPath   = "coot";
  ccp4mgPath = "ccp4mg";

  if (!masterDataPath.isEmpty())  {
    QStringList lst;
    if (QFileInfo(ccp4Path+"/restore").exists())
      cootPath = ccp4Path + "bin/coot";
    else  {
      lst = QDir(masterDataPath).entryList ( QStringList() << "coot*" );
      if (lst.count()>0)
        cootPath = masterDataPath + lst.at(lst.count()-1) + dirsep +
                                            "bin" + dirsep + "coot";
    }
    lst = QDir(masterDataPath).entryList ( QStringList() << "ccp4mg*" );
    if (lst.count()>0)
      ccp4mgPath = masterDataPath + lst.at(lst.count()-1) + dirsep +
                                          "bin" + dirsep + "ccp4mg";
  }
  if (!ccp4Path.isEmpty())  {
    binPath     = ccp4Path + "bin" +  dirsep;
    viewhklPath = binPath  + "viewhkl";
  } else
    viewhklPath = "viewhkl";
  browserPath = "firefox";
  oldMac      = false;

#endif

  exportDir  = "";
  workingDir = "./";  // changes when log file is read
//  logFind    = "";

//  logFindCaseSensitive = false;
//  logFindWholeWords    = false;

  setMinimumWidth ( 500 );
  setMaximumHeight( 100 );

  logFont.setFamily    ( "courier" );
  logFont.setPointSize ( 12 );

  makeLayout();

}

qtCOFE::Preferences::~Preferences()  {}

void qtCOFE::Preferences::checkMacVersion()  {
QProcess process(this);
  process.start ( "defaults",QStringList() << "read" <<
                  "loginwindow" << "SystemVersionStampAsString" );
  process.waitForFinished();
  macVersion = process.readAllStandardOutput().simplified();
  oldMac     = (macVersion.compare("10.6.8")<0);
}

void qtCOFE::Preferences::makeLayout()  {
QWidget     *tab;
QPushButton *btn;
QGridLayout *gbox;
QVBoxLayout *vbox;
QHBoxLayout *hbox;
int          row;

  tabs = new QTabWidget();

  // Applications tab

  gbox = new QGridLayout();
  gbox->setVerticalSpacing ( 0 );
  row  = 0;

  gbox->addWidget ( new QLabel("Server URI:"),row++,0,1,2 );
  server_uri_edt = new QLineEdit ( procPath );
  server_uri_btn = new QPushButton ( "browse" );
  connect ( server_uri_btn,SIGNAL(clicked()),
            this,SLOT(browse_server_dir()) );
  gbox->addWidget ( server_uri_edt,row,0,1,1 );
  gbox->addWidget ( server_uri_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Path to executables:"),row++,0,1,2 );
  bin_path_edt = new QLineEdit ( binPath );
  bin_path_btn = new QPushButton ( "browse" );
  connect ( bin_path_btn,SIGNAL(clicked()),this,SLOT(browse_bin_dir()) );
  gbox->addWidget ( bin_path_edt,row,0,1,1 );
  gbox->addWidget ( bin_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Master Path to repositories:"),row++,0,1,2 );
  master_path_edt = new QLineEdit ( masterDataPath );
  master_path_btn = new QPushButton ( "browse" );
  connect ( master_path_btn,SIGNAL(clicked()),
            this,SLOT(browse_master_dir()) );
  gbox->addWidget ( master_path_edt,row,0,1,1 );
  gbox->addWidget ( master_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Path to Coot:"),row++,0,1,2 );
  coot_path_edt = new QLineEdit ( cootPath );
  coot_path_btn = new QPushButton ( "browse" );
  connect ( coot_path_btn,SIGNAL(clicked()),this,SLOT(browse_coot()) );
  gbox->addWidget ( coot_path_edt,row,0,1,1 );
  gbox->addWidget ( coot_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Path to ccp4mg:"),row++,0,1,2 );
  ccp4mg_path_edt = new QLineEdit ( ccp4mgPath );
  ccp4mg_path_btn = new QPushButton ( "browse" );
  connect ( ccp4mg_path_btn,SIGNAL(clicked()),this,SLOT(browse_ccp4mg()) );
  gbox->addWidget ( ccp4mg_path_edt,row,0,1,1 );
  gbox->addWidget ( ccp4mg_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  use_system_browser_chk = new QCheckBox (
                            "use System Browser for external links:" );
  use_system_browser_chk->setChecked ( useSystemBrowser );
  gbox->addWidget ( use_system_browser_chk,row++,0,1,2 );
  browser_path_edt = new QLineEdit ( browserPath );
  browser_path_btn = new QPushButton ( "browse" );
  connect ( browser_path_btn,SIGNAL(clicked()),this,SLOT(browse_browser()) );
  gbox->addWidget ( browser_path_edt,row,0,1,1 );
  gbox->addWidget ( browser_path_btn,row++,1,1,1 );

  connect ( use_system_browser_chk,SIGNAL(clicked()),
            this,SLOT(useSystemBrowserClicked()) );
  useSystemBrowserClicked();

  tab = new QWidget();
  tab->setLayout ( gbox );
  tabs->addTab ( tab,"Applications" );


  // Layout tab

  gbox = new QGridLayout();
//  gbox->setVerticalSpacing ( 0 );
  row  = 0;

  plot_height_sbx = new QSpinBox();
  plot_height_sbx->setRange  ( 50,1500    );
  plot_height_sbx->setSuffix ( " px"      );
  plot_height_sbx->setValue  ( plotHeight );
  gbox->addWidget ( new QLabel("Embedded graph height:"),row,0,1,1 );
  gbox->addWidget ( plot_height_sbx,row,1,1,1 );
  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

  plot_width_sbx = new QSpinBox();
  plot_width_sbx->setRange  ( 50,1500   );
  plot_width_sbx->setSuffix ( " px"     );
  plot_width_sbx->setValue  ( plotWidth );
  gbox->addWidget ( new QLabel("Embedded graph width:"),row,0,1,1 );
  gbox->addWidget ( plot_width_sbx,row,1,1,1 );
  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

  tree_width_sbx = new QSpinBox();
  tree_width_sbx->setRange  ( 50,1500   );
  tree_width_sbx->setSuffix ( " px"     );
  tree_width_sbx->setValue  ( treeWidth );
  gbox->addWidget ( new QLabel("Graph tree width:"),row,0,1,1 );
  gbox->addWidget ( tree_width_sbx,row,1,1,1 );
  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

//  max_decor_size_sbx = new QSpinBox();
//  max_decor_size_sbx->setRange  ( 50,1500   );
//  max_decor_size_sbx->setSuffix ( " px"     );
//  max_decor_size_sbx->setValue  ( maxDecorationSize );
//  gbox->addWidget ( new QLabel("Maximum size of pictures: "),row,0,1,1 );
//  gbox->addWidget ( max_decor_size_sbx,row,1,1,1 );
//  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

  font_edt = new qtx::LineEdit ( "",20 );
  font_edt->setReadOnly ( true );
  setFontSpec();
  btn = new QPushButton("Choose");
  gbox->addWidget ( new QLabel("Monospace font:"),row,0,1,1 );
  gbox->addWidget ( font_edt,row,1,1,1 );
  gbox->addWidget ( btn,row++,2,1,1 );

  connect ( btn,SIGNAL(clicked()),this,SLOT(choose_font()) );

  gbox->addWidget ( new QLabel(" "),row,0,1,3 );

  gbox->setColumnStretch ( 0,1   );
  gbox->setColumnStretch ( 1,1   );
  gbox->setColumnStretch ( 2,100 );

  gbox->setRowStretch  ( row,100 );

  tab = new QWidget();
  tab->setLayout ( gbox );
  tabs->addTab ( tab,"Layout" );


  // Behaviour tab

  gbox = new QGridLayout();
//  gbox->setVerticalSpacing ( 0 );
  row  = 0;

  refresh_mode_cmb = new QComboBox();
  refresh_mode_cmb->addItem ( "Smart"   ,qtCOFE_SmartRefresh    );
  refresh_mode_cmb->addItem ( "Constant",qtCOFE_ConstantRefresh );
  refresh_mode_cmb->addItem ( "Manual"  ,qtCOFE_ManualRefresh   );

  refresh_lbl = new QLabel ( "Refresh period:" );

  refresh_period_sbx = new QSpinBox();
  refresh_period_sbx->setRange   ( 1,100   );
  refresh_period_sbx->setSuffix  ( " secs" );
  refresh_period_sbx->setValue   ( refreshPeriod/1000 );

//  refresh_period_sbx->setEnabled ( refreshEnabled );
  gbox->addWidget ( new QLabel("Refresh mode:"),row,0,1,1 );
  gbox->addWidget ( refresh_mode_cmb         ,row++,1,1,1 );
  gbox->addWidget ( refresh_lbl                ,row,0,1,1 );
  gbox->addWidget ( refresh_period_sbx         ,row,1,1,1 );
  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

  connect ( refresh_mode_cmb,SIGNAL(currentIndexChanged(int)),
            this,SLOT(refreshModeChanged(int)) );

//  gd_ext_browsing_chk = new QCheckBox ( "Extended graph data selection" );
//  gd_ext_browsing_chk->setChecked ( gdExtSelection );
//  gbox->addWidget ( gd_ext_browsing_chk,row,0,1,2 );
//  gbox->addWidget ( new QLabel(" "),row++,2,1,1 );

  gbox->addWidget ( new QLabel(" "),row,0,1,3 );

  gbox->setColumnStretch ( 0,1   );
  gbox->setColumnStretch ( 1,1   );
  gbox->setColumnStretch ( 2,100 );

  gbox->setRowStretch  ( row,100 );

  tab = new QWidget();
  tab->setLayout ( gbox );
  tabs->addTab ( tab,"Behaviour" );


  // Making the dialog

  hbox = new QHBoxLayout();
  hbox->addStretch ( 100 );

  btn = new QPushButton ( "Apply" );
  btn->setDefault ( true );
  connect ( btn,SIGNAL(clicked()),this,SLOT(apply()) );
  hbox->addWidget ( btn,1 );

  btn = new QPushButton ( "Close" );
  connect ( btn,SIGNAL(clicked()),this,SLOT(close()) );
  hbox->addWidget ( btn,1 );

  vbox = new QVBoxLayout();
  vbox->addWidget ( tabs,100 );
  vbox->addLayout ( hbox,1 );

  setLayout ( vbox );

}

void qtCOFE::Preferences::setFontSpec()  {
  if (font_edt)
    font_edt->setText ( logFont.family() +
                        QString(" %1pt").arg(logFont.pointSize()) );
}

void qtCOFE::Preferences::choose_font()  {
  logFont = QFontDialog::getFont ( &fontChanged,logFont,this,
                                   "Choose Monospace Font" );
  if (fontChanged)
    setFontSpec();
}

void qtCOFE::Preferences::actualize()  {

  server_uri_edt      -> setText    ( procPath       );
  master_path_edt     -> setText    ( masterDataPath );
  bin_path_edt        -> setText    ( binPath        );
  coot_path_edt       -> setText    ( cootPath       );
  ccp4mg_path_edt     -> setText    ( ccp4mgPath     );

  plot_height_sbx     -> setValue   ( plotHeight );
  plot_width_sbx      -> setValue   ( plotWidth  );
  tree_width_sbx      -> setValue   ( treeWidth  );
//  max_decor_size_sbx  -> setValue   ( maxDecorationSize  );
  refresh_period_sbx  -> setValue   ( refreshPeriod/1000 );
  refresh_mode_cmb    -> setCurrentIndex ( refreshMode   );

  refreshModeChanged ( refreshMode );

//  refresh_period_sbx  -> setEnabled ( refreshEnabled     );
//  gd_ext_browsing_chk -> setChecked ( gdExtSelection     );

  setFontSpec();

}

void qtCOFE::Preferences::setProcPath ( const QString & server_uri )  {
  procPath = server_uri;
  server_uri_edt->setText    ( procPath       );
//  server_uri_edt->setEnabled ( !cfgFromCCP4gui );
//  server_uri_btn->setEnabled ( !cfgFromCCP4gui );
}

void qtCOFE::Preferences::setMasterPath ( const QString & master_path )  {
  masterDataPath = master_path;
  master_path_edt->setText    ( masterDataPath );
//  server_uri_edt->setEnabled ( !cfgFromCCP4gui );
//  server_uri_btn->setEnabled ( !cfgFromCCP4gui );
}

void qtCOFE::Preferences::setBinPath ( const QString & bin_path )  {
  binPath = bin_path;
  bin_path_edt->setText    ( binPath );
//  server_uri_edt->setEnabled ( !cfgFromCCP4gui );
//  server_uri_btn->setEnabled ( !cfgFromCCP4gui );
}

void qtCOFE::Preferences::setCootPath ( const QString & coot_path )  {
  cootPath = coot_path;
  coot_path_edt->setText    ( cootPath        );
//  coot_path_edt->setEnabled ( !cfgFromCCP4gui );
//  coot_path_btn->setEnabled ( !cfgFromCCP4gui );
}

void qtCOFE::Preferences::setMGPath ( const QString & ccp4mg_path )  {
  ccp4mgPath = ccp4mg_path;
  ccp4mg_path_edt->setText    ( ccp4mgPath      );
//  ccp4mg_path_edt->setEnabled ( !cfgFromCCP4gui );
//  ccp4mg_path_btn->setEnabled ( !cfgFromCCP4gui );
}

QString readPGMPath ( QString & old_path, const char *tag,
                      QSettings *settings )  {
QString new_path = settings->value(tag,old_path).toString();
  if (QFileInfo(old_path).exists() && (!QFileInfo(new_path).exists()))
    return old_path;
  return new_path;
}

void qtCOFE::Preferences::readSettings  ( QSettings *settings )  {

  if (settings->contains(qtCOFE_SET_PrefWinSize))  {
    resize (settings->value(qtCOFE_SET_PrefWinSize,QSize(700,800)).toSize());
    move   (settings->value(qtCOFE_SET_PrefWinPos,QPoint(100,50)).toPoint());
  }

  treeWidth     = settings->value ( qtCOFE_SET_TreeWidth ,300 ).toInt();
  plotWidth     = settings->value ( qtCOFE_SET_PlotWidth ,505 ).toInt();
  plotHeight    = settings->value ( qtCOFE_SET_PlotHeight,400 ).toInt();
//  maxDecorationSize = settings->value(qtCOFE_SET_MaxDecorSize,
//                                           maxDecorationSize ).toInt();
  refreshPeriod    = settings->value(qtCOFE_SET_RefreshPeriod,
                                   qtCOFE_DefaultRefreshPeriod).toInt();
  useSystemBrowser = settings->value(qtCOFE_SET_UseSystemBrowser,
                                             useSystemBrowser).toBool();
  refreshMode      = settings->value(qtCOFE_SET_RefreshMode,
                                                   refreshMode).toInt();
//  gdExtSelection   = settings->value(qtCOFE_SET_gdExtSelection,
//                                               gdExtSelection).toBool();


  masterDataPath = settings->value(qtCOFE_SET_MasterDataPath,
                                            masterDataPath).toString();
  binPath = settings->value(qtCOFE_SET_BinPath,binPath).toString();

  if (settings->value(qtCOFE_SET_AppDir,"*").toString()
      .compare(QApplication::applicationFilePath()) == 0)  {
    // CCP4 directory has not moved, assume no new release, so coot
    // and mg paths should be read
    procPath = settings->value(qtCOFE_SET_ProcPath,"").toString();
    cootPath  = readPGMPath ( cootPath ,qtCOFE_SET_CootPath ,settings );
#ifdef Q_OS_WIN32
    if (!cootPath.endsWith(".bat",Qt::CaseInsensitive))
      cootPath = "C:\\Wincoot\\runwincoot.bat";
#endif
    ccp4mgPath  = readPGMPath ( ccp4mgPath,qtCOFE_SET_MGPath  ,settings );
    viewhklPath = readPGMPath ( viewhklPath,qtCOFE_SET_ViewHKLPath,settings);
  }

  browserPath   = readPGMPath ( browserPath,qtCOFE_SET_BrowserPath,settings);
  exportDir     = settings->value(qtCOFE_SET_ExportDir,""      ).toString();
//  logFind       = settings->value(qtCOFE_SET_LogFind  ,""      ).toString();
//  logFindCaseSensitive = settings->value(qtCOFE_SET_LogFindCase ,false)
//                                                              .toBool();
//  logFindWholeWords    = settings->value(qtCOFE_SET_LogFindWords,false)
//                                                              .toBool();

  logFont.fromString ( settings->value(qtCOFE_SET_LogFont,
                                       logFont.toString()).toString() );

  if (logFont.pointSize()<=0)  {
    logFont.setFamily    ( "courier" );
    logFont.setPointSize ( 12 );
    logFont.setStyle     ( QFont::StyleNormal );
    logFont.setWeight    ( 50 );
  }

}

void qtCOFE::Preferences::writeSettings ( QSettings *settings )  {

  settings->setValue ( qtCOFE_SET_PrefWinSize     ,size()            );
  settings->setValue ( qtCOFE_SET_PrefWinPos      ,pos ()            );

  settings->setValue ( qtCOFE_SET_TreeWidth       ,treeWidth         );
  settings->setValue ( qtCOFE_SET_PlotWidth       ,plotWidth         );
  settings->setValue ( qtCOFE_SET_PlotHeight      ,plotHeight        );
//  settings->setValue ( qtCOFE_SET_MaxDecorSize    ,maxDecorationSize );
  settings->setValue ( qtCOFE_SET_RefreshPeriod   ,refreshPeriod     );
  settings->setValue ( qtCOFE_SET_UseSystemBrowser,useSystemBrowser  );
  settings->setValue ( qtCOFE_SET_RefreshMode     ,refreshMode       );
//  settings->setValue ( qtCOFE_SET_gdExtSelection  ,gdExtSelection    );
  settings->setValue ( qtCOFE_SET_AppDir,
                                QApplication::applicationFilePath()  );
  settings->setValue ( qtCOFE_SET_ProcPath        ,procPath          );
  settings->setValue ( qtCOFE_SET_MasterDataPath  ,masterDataPath    );
  settings->setValue ( qtCOFE_SET_BinPath         ,binPath           );
  settings->setValue ( qtCOFE_SET_CootPath        ,cootPath          );
  settings->setValue ( qtCOFE_SET_MGPath          ,ccp4mgPath        );
  settings->setValue ( qtCOFE_SET_ViewHKLPath     ,viewhklPath       );
  settings->setValue ( qtCOFE_SET_BrowserPath     ,browserPath       );
  settings->setValue ( qtCOFE_SET_ExportDir       ,exportDir         );
//  settings->setValue ( qtCOFE_SET_LogFind         ,logFind           );
//  settings->setValue ( qtCOFE_SET_LogFindCase     ,logFindCaseSensitive );
//  settings->setValue ( qtCOFE_SET_LogFindWords    ,logFindWholeWords    );

  settings->setValue ( qtCOFE_SET_LogFont         ,logFont.toString() );

}


void qtCOFE::Preferences::apply()  {

  procPath          = server_uri_edt  ->text();
  masterDataPath    = master_path_edt ->text();
  binPath           = bin_path_edt    ->text();
  cootPath          = coot_path_edt   ->text();
  ccp4mgPath        = ccp4mg_path_edt ->text();
  browserPath       = browser_path_edt->text();

  plotHeight        = plot_height_sbx->value();
  plotWidth         = plot_width_sbx ->value();
  treeWidth         = tree_width_sbx ->value();
//  maxDecorationSize = max_decor_size_sbx ->value();
  refreshPeriod     = refresh_period_sbx -> value()*1000;
  refreshMode       = refresh_mode_cmb   -> currentIndex();
//  gdExtSelection    = gd_ext_browsing_chk->isChecked();

//  accept();

  emit applyPreferences();

}

void qtCOFE::Preferences::close()  {

  if (
      (procPath          != server_uri_edt ->text())  ||
      (masterDataPath    != master_path_edt->text())  ||
      (binPath           != bin_path_edt   ->text())  ||
      (cootPath          != coot_path_edt  ->text())  ||
      (ccp4mgPath        != ccp4mg_path_edt->text())  ||
      (plotHeight        != plot_height_sbx->value()) ||
      (plotWidth         != plot_width_sbx ->value()) ||
      (treeWidth         != tree_width_sbx ->value()) ||
//      (maxDecorationSize != max_decor_size_sbx ->value()) ||
      (refreshPeriod     != refresh_period_sbx->value()*1000)  ||
      (refreshMode       != refresh_mode_cmb->currentIndex())
//      (gdExtSelection ^ gd_ext_browsing_chk->isChecked())
     )  {
    if (QMessageBox::question(this,tr("Apply changes?"),
         tr("Changes have been made. Do you wish to apply them?"),
         QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)  {
      apply ();
      accept();
      return;
    }
  }

  reject();

}

void qtCOFE::Preferences::refreshModeChanged ( int index )  {
  refresh_lbl        -> setVisible ( index != qtCOFE_ManualRefresh );
  refresh_period_sbx -> setVisible ( index != qtCOFE_ManualRefresh );
}

void qtCOFE::Preferences::useSystemBrowserClicked()  {
  useSystemBrowser = use_system_browser_chk->isChecked();
  browser_path_edt->setEnabled ( useSystemBrowser );
  browser_path_btn->setEnabled ( useSystemBrowser );
}


QString qtCOFE::Preferences::browse ( const QString title,
                                      const QString & suggestion )  {
QString fileName;
  if (!suggestion.isEmpty())
    fileName = QFileInfo(suggestion).absolutePath();
#ifdef Q_OS_MAC
  fileName = QFileDialog::getOpenFileName ( this,title,fileName,
                                            tr("Applications (*.app)"));
#endif
#ifdef Q_OS_LINUX
  fileName = QFileDialog::getOpenFileName ( this,title,fileName,
                                            tr("Executables (*)"));
#endif
#ifdef Q_OS_WIN32
  fileName = QFileDialog::getOpenFileName ( this,title,fileName,
                                       tr("Executables (*.exe *.bat)"));
#endif
  return fileName;
}

void qtCOFE::Preferences::browse_server_dir() {
QString dirName;
  if (!dirName.isEmpty())
    dirName = QFileInfo(server_uri_edt->text()).absolutePath();
  dirName = QFileDialog::getExistingDirectory ( this,
                                   "Locate Server Directory",dirName );
  if (!dirName.isEmpty())  {
    if (!dirName.endsWith('/'))
      dirName.append ( "/" );
    server_uri_edt->setText ( dirName );
  }
}

void qtCOFE::Preferences::browse_master_dir() {
QString dirName;
  if (!dirName.isEmpty())
    dirName = QFileInfo(master_path_edt->text()).absolutePath();
  dirName = QFileDialog::getExistingDirectory ( this,
                              "Locate Master Data Directory",dirName );
  if (!dirName.isEmpty())  {
    if (!dirName.endsWith('/'))
      dirName.append ( "/" );
    master_path_edt->setText ( dirName );
  }
}

void qtCOFE::Preferences::browse_bin_dir() {
QString dirName;
  if (!dirName.isEmpty())
    dirName = QFileInfo(bin_path_edt->text()).absolutePath();
  dirName = QFileDialog::getExistingDirectory ( this,
                         "Locate directory with executables",dirName );
  if (!dirName.isEmpty())  {
    if (!dirName.endsWith('/'))
      dirName.append ( "/" );
    bin_path_edt->setText ( dirName );
  }
}

void qtCOFE::Preferences::browse_coot()  {
QString fileName = browse ( "Locate Coot",coot_path_edt->text() );
  if (!fileName.isEmpty())
    coot_path_edt->setText ( fileName );
}

void qtCOFE::Preferences::browse_ccp4mg()  {
QString fileName = browse ( "Locate ccp4mg",ccp4mg_path_edt->text() );
  if (!fileName.isEmpty())
    ccp4mg_path_edt->setText ( fileName );
}

void qtCOFE::Preferences::browse_browser()  {
QString fileName = browse ( "Locate internet browser",
                            browser_path_edt->text() );
  if (!fileName.isEmpty())
    browser_path_edt->setText ( fileName );
}

void qtCOFE::Preferences::setExportDir ( const QString & path )  {
QFileInfo fileInfo(path);

  if (fileInfo.isDir())
        exportDir = path;
  else  exportDir = fileInfo.absoluteDir().absolutePath();

  if (!exportDir.endsWith("/"))
    exportDir.append ( "/" );

}

QString qtCOFE::Preferences::ccp4DocIndex()  {
QString docPath = ccp4Path + qtCOFE_Doc_Directory + "INDEX.html";
  if (QFileInfo(docPath).exists())
    return docPath;
  return QString();
}

QString qtCOFE::Preferences::ccp4Doc ( const QString & pgmName )  {
QString docPath = ccp4Path + qtCOFE_Doc_Directory + pgmName + ".html";
  if (QFileInfo(docPath).exists())
    return docPath;
  return QString();
}


QString qtCOFE::Preferences::getProcessingGate()  {
QString  processor = procPath + qtCOFE_SERVER_ProcGate;
bool     done = false;

  QApplication::processEvents();

  while ((!QFileInfo(processor).exists()) && (!done))  {
    if (QMessageBox::question ( this,"Server misconfigured",
         "Server is misconfigured, looking at<p>'" +
         procPath + "'<p>Would you like to specify the server "
         "now (just navigate to directory containing " +
         qtCOFE_SERVER_ProcGate +")? This needs to be done only once.",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
           QMessageBox::Yes)  {
      browse_server_dir();
      if (server_uri_edt->text()==procPath)
        done = true;
      else  {
        procPath = server_uri_edt->text();
        processor = procPath + qtCOFE_SERVER_ProcGate;
      }
    } else
      done = true;
  }

  if (done)
    processor.clear();

  return processor;

}

QString qtCOFE::Preferences::getMasterPath()  {
bool  done = false;

  QApplication::processEvents();

  while ((!QFileInfo(masterDataPath).exists()) && (!done))  {
    if (QMessageBox::question ( this,"Master Path misconfigured",
         "Master Data Path is misconfigured, looking at<p>'" +
         masterDataPath + "'<p>Would you like to specify the MDP "
         "now (just navigate to directory where all local data and " +
         "projects should be kept)? This needs to be done only once.",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
                                                   QMessageBox::Yes)  {
      browse_master_dir();
      if (master_path_edt->text()==masterDataPath)
            done = true;
      else  masterDataPath = master_path_edt->text();
    } else
      done = true;
  }

//  if (done)
//    masterDataPath.clear();

  return masterDataPath;

}

QString qtCOFE::Preferences::getBinPath()  {
bool  done = false;

  QApplication::processEvents();

  while ((!QFileInfo(binPath).exists()) && (!done))  {
    if (QMessageBox::question ( this,"Path to executables misconfigured",
         "Path to executables is misconfigured, looking at<p>'" +
         binPath + "'<p>Would you like to specify the path "
         "now (just navigate to directory where all executables " +
         "should be found)? This needs to be done only once.",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
                                                   QMessageBox::Yes)  {
      browse_bin_dir();
      if (bin_path_edt->text()==binPath)
            done = true;
      else  binPath = bin_path_edt->text();
    } else
      done = true;
  }

  return binPath;

}

void qtCOFE::Preferences::launchCoot ( const QStringList & arguments )  {

  QApplication::processEvents();

  if (!QFileInfo(cootPath).exists())  {
    if (QMessageBox::question ( this,"Coot not found",
         "Coot is not found, looking at<p>'" +
         cootPath + "'<p>Would you like to locate Coot "
         "executable now (this needs to be done only once)?",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
           QMessageBox::Yes)  {
      browse_coot();
      if (coot_path_edt->text()==cootPath)
        return;
      cootPath = coot_path_edt->text();
    } else
      return;
  }

#ifdef Q_OS_MAC
  QString coot_path = cootPath;
  if (coot_path.endsWith(".app"))
    coot_path.append ( "/Contents/MacOS/applet" );
      //    coot_path.append ( "/Contents/coot/bin/coot" );
  QProcess::startDetached ( coot_path,
                            QStringList() << arguments <<
                            "--no-state-script" <<
                            "-c" << "(no-coot-tips)" <<
                            "--no-guano",
                            workingDir );

/*  -- old version in which current directory is not sensed by Coot
  if (oldMac)
    QProcess::startDetached ( cootPath+"/Contents/MacOS/coot",
                              QStringList() << arguments <<
                              "--no-state-script" <<
                              "-c" << "(no-coot-tips)" <<
                              "--no-guano",
                              workingDir );
  else
    QProcess::startDetached ( "open",QStringList() << "-n" << "-a" <<
                              cootPath << "--args" <<
                              "-c" << "(no-coot-tips)" <<
                              arguments <<
                              "--no-state-script" <<
                              "--no-guano",
                              workingDir );
*/
#else
  QProcess::startDetached ( cootPath,QStringList() << arguments <<
                              "--no-state-script" <<
                              "-c" << "(no-coot-tips)" <<
                              "--no-guano",
                              workingDir );
#endif

}

void qtCOFE::Preferences::launchMG ( const QStringList & arguments )  {

  QApplication::processEvents();

  if (!QFileInfo(ccp4mgPath).exists())  {
    if (QMessageBox::question ( this,"CCP4MG not found",
         "CCP4MG is not found, looking at<p>'" +
         ccp4mgPath + "'<p>Would you like to locate CCP4MG "
         "(or QtMG) executable now (this needs to be done only once)?",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
           QMessageBox::Yes)  {
      browse_ccp4mg();
      if (ccp4mg_path_edt->text()==ccp4mgPath)
        return;
      ccp4mgPath = ccp4mg_path_edt->text();
    } else
      return;
  }

#ifdef Q_OS_MAC
  if (oldMac)
    QProcess::startDetached ( ccp4mgPath+"/Contents/MacOS/QtMG",
                              arguments,workingDir );
  else
    QProcess::startDetached ( "open",QStringList() << "-n" << "-a" <<
                              ccp4mgPath << "--args" << arguments,
                              workingDir );
#else
  QProcess::startDetached ( ccp4mgPath,arguments,workingDir );
#endif

}

void qtCOFE::Preferences::launchViewHKL ( const QStringList & arguments )  {

  QApplication::processEvents();

  if (!QFileInfo(viewhklPath).exists())  {
    if (QMessageBox::question ( this,"ViewHKL not found",
         "ViewHKL is not found, looking at<p>'" +
         viewhklPath + "'<p>Would you like to locate ViewHKL "
         "executable now (this needs to be done only once)?",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
           QMessageBox::Yes)  {
      QString fileName = browse ( "Locate viewhkl",viewhklPath );
      if (fileName.isEmpty())
        return;
      viewhklPath = fileName;
    } else
      return;
  }

#ifdef Q_OS_MAC
  if (oldMac)
    QProcess::startDetached ( viewhklPath+"/Contents/MacOS/viewhkl",
                              arguments,workingDir );
  else
    QProcess::startDetached ( "open",QStringList() << "-n" << "-a" <<
                              viewhklPath << "--args" << arguments,
                              workingDir );
#else
  QProcess::startDetached ( viewhklPath,arguments,workingDir );
#endif

}


void qtCOFE::Preferences::launchBrowser ( const QStringList & arguments ) {
bool ok = false;

  QApplication::processEvents();

  while (!ok)  {

#ifdef Q_OS_MAC
    if (browserPath.endsWith("Safari.app"))  {
      if (oldMac)
        ok = QProcess::startDetached ( browserPath+"/Contents/MacOS/Safari",
                                       arguments );
      else
        ok = QProcess::startDetached ( "open",arguments );
    } else
      ok = QProcess::startDetached ( "open",QStringList() << "-n" << "-a" <<
                                browserPath << "--args" << arguments );
#else
    ok = QProcess::startDetached ( browserPath,arguments );
#endif

    if (!ok) {
      if (!QFileInfo(browserPath).exists())
        ok = (QMessageBox::question ( this,"Browser not found",
               "Internet browser is not found, looking at<p>'" +
               browserPath + "'<p>Would you like to locate your browser "
               "executable now (this needs to be done only once)?",
               QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
               QMessageBox::Yes);
      else
        ok = (QMessageBox::question ( this,"Browser not launched",
               "Cannot launch internet browser, looking at<p>'" +
               browserPath + "'<p>Would you like to locate your browser "
               "executable now (this needs to be done only once)?",
               QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
               QMessageBox::Yes);
      if (ok)  {
        browse_browser();
        if (browser_path_edt->text()!=browserPath)  {
          browserPath = browser_path_edt->text();
          ok = false; // make another attempt
        }
      } else
        ok = true;  // user calcelled

    }

  }

}

QFont qtCOFE::Preferences::getScaledFont ( const QFont & font )  {
QFont f = font;
qreal s = -1.0;

  if (logFont.pointSizeF()>0.0)    s = logFont.pointSizeF();
  else if (logFont.pointSize()>0)  s = logFont.pointSize ();
  if (s>0.0)  s /= 12.0;
        else  s  = 1.0;

  if (f.pointSizeF()>0.0)   f.setPointSizeF ( s*f.pointSizeF() );
  else if (f.pointSize()>0) f.setPointSize  ( s*f.pointSize () );
  else if (f.pixelSize()>0) f.setPixelSize  ( s*f.pixelSize () );

  return f;

}

int qtCOFE::Preferences::getFontPointSize()  {
  if (logFont.pointSize()>0)  return logFont.pointSize();
  return 12;
}

int qtCOFE::Preferences::getFontPixelSize()  {
  return QFontInfo(logFont).pixelSize();
}

int qtCOFE::Preferences::getFontHeight()  {
  return QFontMetrics(logFont).height();
}

bool qtCOFE::Preferences::isFontChanged()  {
  if (fontChanged)  {
    fontChanged = false;
    return true;
  }
  return false;
}

QString qtCOFE::Preferences::getFontSizeStyleSheet ( qreal scale )  {
  return QString("font-family: %1; font-style: %2; "
                 "font-size: %3pt; color:black;")
          .arg(QFontInfo(logFont).family())
          .arg(QFontInfo(logFont).style())
          .arg(int(scale*QFontInfo(logFont).pointSize()));
}

int  qtCOFE::Preferences::getToolButtonSize()  {
//    return 3*fontMetrics().height()/2;
  return 3*QFontInfo(logFont).pixelSize()/2;
}

QString qtCOFE::Preferences::getToolButtonStyle()  {
  return "padding:0px;margin:0px;border:0px;color:black;";
}
