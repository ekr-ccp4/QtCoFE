// $Id: qtcofe_page_project.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_project <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::ProjectPage
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_PAGE_PROJECT_H
#define QTCOFE_PAGE_PROJECT_H

#include "qtcofe_page.h"

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE

//namespace qtx  {
//  class LineEdit;
//}

class QJsonObject;

namespace qtCOFE  {

  class ProjectTree;

  class ProjectPage : public Page  {
  Q_OBJECT

    public:
      ProjectPage ( Preferences    *prefs,
                    Session        *ssn,
                    DataModel      *dm,
                    QWidget        *parent = 0,
                    Qt::WindowFlags flags  = 0 );
      virtual ~ProjectPage();

      virtual PAGE_ID pageID() { return PAGE_Project; }

      virtual void makeLayout();

      void loadProject();

    protected:
      QSplitter   *hsplitter;
      ProjectTree *jobTree;
      QString      currentProject;

      void project_query ( QJsonObject & jsonData, QString action );
      void save_project_state();

    protected slots:
      void addJob ( int jobID );
      void delJob ( int jobID, int nextCrJobID );
      void switchJob ( int jobID );

  };

}

#endif // QTCOFE_PAGE_PROJECT_H
