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

  tabs                   = NULL;
  server_uri_edt         = NULL;
  projects_path_edt      = NULL;
  ccp4_path_edt          = NULL;
  browser_path_edt       = NULL;
  server_uri_btn         = NULL;
  projects_path_btn      = NULL;
  ccp4_path_btn          = NULL;
  browser_path_btn       = NULL;
  tree_width_sbx         = NULL;
  plot_width_sbx         = NULL;
  plot_height_sbx        = NULL;
  font_edt               = NULL;
  refresh_mode_cmb       = NULL;
  refresh_lbl            = NULL;
  refresh_period_sbx     = NULL;
  use_system_browser_chk = NULL;

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

  serverUri    = "";
  projectsPath = "";
//  binPath      = "";

#ifdef Q_OS_MAC
//  binPath     = ccp4Path + "bin/";
//  cootPath    = ccp4Path + "coot.app";
//  ccp4mgPath  = ccp4Path + "QtMG.app";
//  viewhklPath = ccp4Path + "ViewHKL.app";
  browserPath = "/Applications/Safari.app";
  checkMacVersion();
#elif defined(Q_OS_WIN)

/*
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
    
*/
    
  browserPath = "C:\\Program Files\\Internet Explorer\\iexplore.exe";
  oldMac      = false;

#else

/*
  cootPath   = "coot";
  ccp4mgPath = "ccp4mg";

  if (!projectsPath.isEmpty())  {
    QStringList lst;
    if (QFileInfo(ccp4Path+"/restore").exists())
      cootPath = ccp4Path + "bin/coot";
    else  {
      lst = QDir(projectsPath).entryList ( QStringList() << "coot*" );
      if (lst.count()>0)
        cootPath = projectsPath + lst.at(lst.count()-1) + dirsep +
                                            "bin" + dirsep + "coot";
    }
    lst = QDir(projectsPath).entryList ( QStringList() << "ccp4mg*" );
    if (lst.count()>0)
      ccp4mgPath = projectsPath + lst.at(lst.count()-1) + dirsep +
                                          "bin" + dirsep + "ccp4mg";
  }
  if (!ccp4Path.isEmpty())
    viewhklPath = ccp4Path + "bin" +  dirsep  + "viewhkl";
  else
    viewhklPath = "viewhkl";
*/    
    
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
  server_uri_edt = new QLineEdit ( serverUri );
  server_uri_btn = new QPushButton ( "browse" );
  connect ( server_uri_btn,SIGNAL(clicked()),
            this,SLOT(browse_server_dir()) );
  gbox->addWidget ( server_uri_edt,row,0,1,1 );
  gbox->addWidget ( server_uri_btn,row++,1,1,1 );

//  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );
//
//  gbox->addWidget ( new QLabel("Path to executables:"),row++,0,1,2 );
//  bin_path_edt = new QLineEdit ( binPath );
//  bin_path_btn = new QPushButton ( "browse" );
//  connect ( bin_path_btn,SIGNAL(clicked()),this,SLOT(browse_bin_dir()) );
//  gbox->addWidget ( bin_path_edt,row,0,1,1 );
//  gbox->addWidget ( bin_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Path to porject repositories:"),row++,0,1,2 );
  projects_path_edt = new QLineEdit   ( projectsPath );
  projects_path_btn = new QPushButton ( "browse" );
  connect ( projects_path_btn,SIGNAL(clicked()),
            this,SLOT(browse_projects_dir()) );
  gbox->addWidget ( projects_path_edt,row,0,1,1 );
  gbox->addWidget ( projects_path_btn,row++,1,1,1 );

  gbox->addWidget ( new QLabel(" "),row++,0,1,2 );

  gbox->addWidget ( new QLabel("Path to CCP4:"),row++,0,1,2 );
  ccp4_path_edt = new QLineEdit ( ccp4Path );
  ccp4_path_btn = new QPushButton ( "browse" );
  connect ( ccp4_path_btn,SIGNAL(clicked()),this,SLOT(browse_ccp4()) );
  gbox->addWidget ( ccp4_path_edt,row,0,1,1 );
  gbox->addWidget ( ccp4_path_btn,row++,1,1,1 );

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

  server_uri_edt      -> setText    ( serverUri    );
  projects_path_edt   -> setText    ( projectsPath );
  ccp4_path_edt       -> setText    ( ccp4Path     );

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

void qtCOFE::Preferences::setServerUri ( const QString & server_uri )  {
  serverUri = server_uri;
  server_uri_edt->setText    ( serverUri );
}

void qtCOFE::Preferences::setProjectsPath (
                                     const QString & projects_path )  {
  projectsPath = projects_path;
  projects_path_edt->setText    ( projectsPath );
}

void qtCOFE::Preferences::setCCP4Path ( const QString & ccp4_path )  {
  ccp4Path = ccp4_path;
  ccp4_path_edt->setText ( ccp4Path );
}

QString readPath ( QString & old_path, const char *tag,
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
  refreshPeriod    = settings->value(qtCOFE_SET_RefreshPeriod,
                                   qtCOFE_DefaultRefreshPeriod).toInt();
  useSystemBrowser = settings->value(qtCOFE_SET_UseSystemBrowser,
                                             useSystemBrowser).toBool();
  refreshMode      = settings->value(qtCOFE_SET_RefreshMode,
                                                   refreshMode).toInt();

  projectsPath = settings->value(qtCOFE_SET_ProjectsPath,
                                            projectsPath).toString();

  if (settings->value(qtCOFE_SET_AppDir,"*").toString()
      .compare(QApplication::applicationFilePath()) == 0)  {
    // CCP4 directory has not moved, assume no new release, so coot
    // and mg paths should be read
    serverUri = settings->value(qtCOFE_SET_ServerUri,"").toString();
    ccp4Path  = readPath ( ccp4Path,qtCOFE_SET_CCP4Path,settings );
  }

  browserPath = readPath ( browserPath,qtCOFE_SET_BrowserPath,settings);
  exportDir   = settings->value(qtCOFE_SET_ExportDir,""      ).toString();

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
  settings->setValue ( qtCOFE_SET_RefreshPeriod   ,refreshPeriod     );
  settings->setValue ( qtCOFE_SET_UseSystemBrowser,useSystemBrowser  );
  settings->setValue ( qtCOFE_SET_RefreshMode     ,refreshMode       );
  settings->setValue ( qtCOFE_SET_AppDir,
                                QApplication::applicationFilePath()  );
  settings->setValue ( qtCOFE_SET_ServerUri       ,serverUri         );
  settings->setValue ( qtCOFE_SET_ProjectsPath    ,projectsPath      );
  settings->setValue ( qtCOFE_SET_CCP4Path        ,ccp4Path          );
  settings->setValue ( qtCOFE_SET_BrowserPath     ,browserPath       );
  settings->setValue ( qtCOFE_SET_ExportDir       ,exportDir         );

  settings->setValue ( qtCOFE_SET_LogFont         ,logFont.toString() );

}


void qtCOFE::Preferences::apply()  {

  serverUri         = server_uri_edt    ->text();
  projectsPath      = projects_path_edt ->text();
  ccp4Path          = ccp4_path_edt     ->text();
  browserPath       = browser_path_edt  ->text();

  plotHeight        = plot_height_sbx->value();
  plotWidth         = plot_width_sbx ->value();
  treeWidth         = tree_width_sbx ->value();
  refreshPeriod     = refresh_period_sbx -> value()*1000;
  refreshMode       = refresh_mode_cmb   -> currentIndex();

//  accept();

  emit applyPreferences();

}

void qtCOFE::Preferences::close()  {

  if (
      (serverUri     != server_uri_edt   ->text())  ||
      (projectsPath  != projects_path_edt->text())  ||
      (ccp4Path      != ccp4_path_edt    ->text())  ||
      (plotHeight    != plot_height_sbx  ->value()) ||
      (plotWidth     != plot_width_sbx   ->value()) ||
      (treeWidth     != tree_width_sbx   ->value()) ||
      (refreshPeriod != refresh_period_sbx->value()*1000)  ||
      (refreshMode   != refresh_mode_cmb ->currentIndex())
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


QString qtCOFE::Preferences::browse ( const QString & title,
                                      const QString & suggestion )  {
QString fileName;
  if (!suggestion.isEmpty())
        fileName = QFileInfo(suggestion).absolutePath();
  else  fileName = QString();
QMessageBox::information(this,"p1","p1" );
#ifdef Q_OS_MAC
  fileName = "/Applications";
  fileName = QFileDialog::getOpenFileName ( this,title,fileName,
                                            tr("Applications (*.app)"));
QMessageBox::information(this,"p2","p2" );
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

void qtCOFE::Preferences::browse_projects_dir() {
QString dirName;
  if (!dirName.isEmpty())
    dirName = QFileInfo(projects_path_edt->text()).absolutePath();
  dirName = QFileDialog::getExistingDirectory ( this,
                              "Locate Projects Directory",dirName );
  if (!dirName.isEmpty())  {
    if (!dirName.endsWith('/'))
      dirName.append ( "/" );
    projects_path_edt->setText ( dirName );
  }
}

/*
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
*/

void qtCOFE::Preferences::browse_ccp4()  {
QString dirName;
  if (!dirName.isEmpty())
    dirName = QFileInfo(ccp4_path_edt->text()).absolutePath();
  dirName = QFileDialog::getExistingDirectory ( this,
                         "Locate CCP4 Setup Directory",dirName,
                         QFileDialog::ShowDirsOnly |
                         QFileDialog::DontUseNativeDialog );
  if (!dirName.isEmpty())  {
    if (!dirName.endsWith('/'))
      dirName.append ( "/" );
    ccp4_path_edt->setText ( dirName );
  }
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
QString  processor = serverUri + qtCOFE_SERVER_ProcGate;
bool     done = false;

  QApplication::processEvents();

  while ((!QFileInfo(processor).exists()) && (!done))  {
    if (QMessageBox::question ( this,"Server misconfigured",
         "Server is misconfigured, looking at<p>'" +
         serverUri + "'<p>Would you like to specify the server "
         "now (just navigate to directory containing " +
         qtCOFE_SERVER_ProcGate +")? This needs to be done only once.",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
           QMessageBox::Yes)  {
      browse_server_dir();
      if (server_uri_edt->text()==serverUri)
        done = true;
      else  {
        serverUri = server_uri_edt->text();
        processor = serverUri + qtCOFE_SERVER_ProcGate;
      }
    } else
      done = true;
  }

  if (done)
    processor.clear();

  return processor;

}

QString qtCOFE::Preferences::getProjectsPath()  {
bool  done = false;

  QApplication::processEvents();

  while ((!QFileInfo(projectsPath).exists()) && (!done))  {
    if (QMessageBox::question ( this,"Master Path misconfigured",
         "Projects Path is misconfigured, looking at<p>'" +
         projectsPath + "'<p>Would you like to specify the MDP "
         "now (just navigate to directory where all local data and " +
         "projects should be kept)? This needs to be done only once.",
         QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
                                                   QMessageBox::Yes)  {
      browse_projects_dir();
      if (projects_path_edt->text()==projectsPath)
            done = true;
      else  projectsPath = projects_path_edt->text();
    } else
      done = true;
  }

//  if (done)
//    masterDataPath.clear();

  return projectsPath;

}


void qtCOFE::Preferences::launchApp ( const QString     & relPath,
                                      const QStringList & arguments,
                                      const QString     & msgTitle,
                                      const QString     & msgBody )  {
QDir    ccp4Dir(ccp4Path);
QString appPath;
bool    exists,done;

  QApplication::processEvents();

  done   = false;
  while (!done)  {
  
    appPath = ccp4Dir.absoluteFilePath ( relPath );
    exists  = QFileInfo(appPath).exists();

    if (!exists)  {
      QString msgName = appPath;
      if (msgName.isEmpty())
        msgName = "Empty";
      if (QMessageBox::question ( this,msgTitle,msgBody + 
           "<p><i>" + msgName + "</i>"
           "<p>Would you like to locate CCP4 "
           "directory now (this needs to be done only once)?",
           QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes ) ==
             QMessageBox::Yes)  {
        browse_ccp4();
        if (ccp4_path_edt->text()==ccp4Path)
          done = true;
        ccp4Path = ccp4_path_edt->text();
        ccp4Dir.setPath ( ccp4Path );
      } else
        done = true;
    } else
      done = true;

  }

  QApplication::processEvents();

  if (exists)
    QProcess::startDetached ( appPath,arguments,workingDir );

}

void qtCOFE::Preferences::launchCoot ( const QStringList & arguments )  {
  launchApp ( "bin/coot",
              QStringList() << arguments <<
                        "--no-state-script" <<
                        "-c" << "(no-coot-tips)" <<
                        "--no-guano",
              "Coot not found","Coot is not found, looking at" );

}

void qtCOFE::Preferences::launchMG ( const QStringList & arguments )  {
  launchApp ( "bin/ccp4mg",arguments,"CCP4 MG not found",
              "CCP4 MG is not found, looking at" );
}

void qtCOFE::Preferences::launchViewHKL ( const QStringList & arguments )  {
  launchApp ( "bin/viewhkl",arguments,"ViewHKL not found",
              "ViewHKL is not found, looking at" );
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
