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
class QLabel;
class QLineEdit;
class QSpinBox;
class QCheckBox;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

namespace qtx  {
  class LineEdit;
}

namespace qtCOFE  {

  class Preferences : public QDialog  {
  Q_OBJECT

    public:
      int      treeWidth;
      int      plotHeight;
      int      plotWidth;
      QString  serverUri;        //!< path to local processing scripts
      QString  projectsPath;     //!< path to local project directories
      QString  ccp4Path;
      QString  browserPath;
      QString  exportDir;
      QString  workingDir;       //!< not stored in preferences
      QString  macVersion;
      QFont    logFont;
      int      refreshPeriod;
      int      refreshMode;
      bool     useSystemBrowser;
      bool     oldMac;

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

      void setServerUri    ( const QString & server_uri  );
      void setProjectsPath ( const QString & projects_path );
      void setCCP4Path     ( const QString & ccp4_path   );

      QString getProcessingGate();
      QString getProjectsPath  ();

      void readSettings  ( QSettings *settings );
      void writeSettings ( QSettings *settings );

      QFont getScaledFont   ( const QFont & font );
      int   getFontPointSize();
      int   getFontPixelSize();
      int   getFontHeight   ();
      bool  isFontChanged   ();

      QString getFontSizeStyleSheet ( qreal scale );
      int     getToolButtonSize ();
      QString getToolButtonStyle();

    signals:
      void applyPreferences();

    protected:
      QTabWidget    *tabs;

      QLineEdit     *server_uri_edt;
      QLineEdit     *projects_path_edt;
      QLineEdit     *ccp4_path_edt;
      QLineEdit     *browser_path_edt;
      QPushButton   *server_uri_btn;
      QPushButton   *projects_path_btn;
      QPushButton   *ccp4_path_btn;
      QPushButton   *browser_path_btn;
      QSpinBox      *tree_width_sbx;
      QSpinBox      *plot_width_sbx;
      QSpinBox      *plot_height_sbx;
      qtx::LineEdit *font_edt;
      QComboBox     *refresh_mode_cmb;
      QLabel        *refresh_lbl;
      QSpinBox      *refresh_period_sbx;
      QCheckBox     *use_system_browser_chk;
      bool           fontChanged;

      void    checkMacVersion();
      void    makeLayout     ();
      void    setFontSpec    ();
      QString browse    ( const QString & title,
                          const QString & suggestion );
      void    launchApp ( const QString     & relPath,
                          const QStringList & arguments,
                          const QString     & msgTitle,
                          const QString     & msgBody );

    protected slots:
      void apply();
      void close();
      void useSystemBrowserClicked();
      void refreshModeChanged     ( int index );
      void browse_server_dir  ();
      void browse_projects_dir();
      void browse_ccp4        ();
      void browse_browser     ();
      void choose_font        ();

  };

}

#endif // QTCOFE_PREFERENCES_H
