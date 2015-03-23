// $Id: qtcofe_preferences.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_preferences <interface>
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

#ifndef QTCOFE_PREFERENCES_H
#define QTCOFE_PREFERENCES_H

#include <QDialog>
#include <QFont>

QT_BEGIN_NAMESPACE
class QSettings;
class QTabWidget;
class QLineEdit;
class QSpinBox;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

namespace qtx  {
  class LineEdit;
}

namespace qtCOFE  {

  class Preferences : public QDialog  {
  Q_OBJECT

    public:
//      int      treeWidth;
//      int      plotHeight;
//      int      plotWidth;
//      int      maxDecorationSize;
      QString  procPath;
      QString  masterDataPath;
      QString  binPath;
      QString  ccp4Path;
      QString  cootPath;
      QString  ccp4mgPath;
      QString  viewhklPath;
      QString  browserPath;
      QString  exportDir;
      QString  workingDir;  //!< not stored in preferences
//      QString  logFind;
      QString  macVersion;
      QFont    logFont;
      int      refreshPeriod;
      bool     useSystemBrowser;
      bool     refreshEnabled;
//      bool     gdExtSelection;   //!< graph data extended selection
//      bool     logFindCaseSensitive;
//      bool     logFindWholeWords;
      bool     oldMac;
//      bool     cfgFromCCP4gui;

      Preferences ( QWidget *parent = 0 );
      ~Preferences();

      void actualize();

      void setExportDir  ( const QString & path );

      void launchCoot    ( const QStringList & arguments );
      void launchMG      ( const QStringList & arguments );
      void launchViewHKL ( const QStringList & arguments );
      void launchBrowser ( const QStringList & arguments );

      QString ccp4DocIndex();  // Empty if file not found
      QString ccp4Doc ( const QString & pgmName ); // Empty if not found

      void setProcPath   ( const QString & server_uri  );
      void setMasterPath ( const QString & master_path );
      void setCootPath   ( const QString & coot_path   );
      void setMGPath     ( const QString & ccp4mg_path );

      QString getProcessingGate();
      QString getMasterPath    ();
      QString getBinPath       ();

      void readSettings  ( QSettings *settings );
      void writeSettings ( QSettings *settings );

      QFont getScaledFont ( const QFont & font );
      int   getFontPointSize();
      int   getFontPixelSize();
      bool  isFontChanged();

      QString getFontSizeStyleSheet ( qreal scale );
      int     getToolButtonSize ();
      QString getToolButtonStyle();

    signals:
      void applyPreferences();

//    public slots:

    protected:
      QTabWidget    *tabs;

      QLineEdit     *server_uri_edt;
      QLineEdit     *master_path_edt;
      QLineEdit     *bin_path_edt;
      QLineEdit     *coot_path_edt;
      QLineEdit     *ccp4mg_path_edt;
      QLineEdit     *browser_path_edt;
      QPushButton   *server_uri_btn;
      QPushButton   *master_path_btn;
      QPushButton   *bin_path_btn;
      QPushButton   *coot_path_btn;
      QPushButton   *ccp4mg_path_btn;
      QPushButton   *browser_path_btn;
//      QSpinBox      *tree_width_sbx;
//      QSpinBox      *plot_width_sbx;
//      QSpinBox      *plot_height_sbx;
//      QSpinBox      *max_decor_size_sbx;
      qtx::LineEdit *font_edt;
      QSpinBox      *refresh_period_sbx;
      QCheckBox     *use_system_browser_chk;
      QCheckBox     *refresh_enabled_chk;
//      QCheckBox     *gd_ext_browsing_chk;
      bool           fontChanged;

      void    checkMacVersion();
      void    makeLayout     ();
      void    setFontSpec    ();
      QString browse ( const QString title, const QString & suggestion );

    protected slots:
      void apply();
      void close();
      void useSystemBrowserClicked();
      void refreshEnabledClicked  ();
      void browse_server_dir();
      void browse_master_dir();
      void browse_bin_dir();
      void browse_coot   ();
      void browse_ccp4mg ();
      void browse_browser();
      void choose_font   ();

  };

}

#endif // QTCOFE_PREFERENCES_H
