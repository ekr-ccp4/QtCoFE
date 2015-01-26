// $Id: qtcofe_page_prjlist.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_prjlist <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::ProjectListPage
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_PROJECTLISTPAGE_H
#define QTCOFE_PROJECTLISTPAGE_H

#include "qtcofe_page.h"
#include "qtcofe_server.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

class QJsonObject;

namespace qtx  {
  class Tree;
}

namespace qtCOFE  {

  class ProjectListPage : public Page  {
  Q_OBJECT

    public:
      ProjectListPage ( DataModel      *dm,
                        QWidget        *parent = 0,
                        Qt::WindowFlags flags  = 0 );
      virtual ~ProjectListPage();

      virtual PAGE_ID pageID() { return PAGE_ProjectList; }

      virtual void makeLayout();
      void  fillProjectList();

      QString getCrProjectPath();

    signals:
      void open_project();

    protected:
      qtx::Tree   *projectTree;
      QPushButton *openBtn;
      QPushButton *addNewBtn;
      QPushButton *insertNewBtn;
      QPushButton *editDescBtn;
      QPushButton *deleteBtn;

      void addProjectName     ( const QJsonObject   & obj,
                                QTreeWidgetItem     *item,
                                const QString & crPrjName,
                                QTreeWidgetItem *& crItem );

      void project_query      ( QJsonObject & jsonData, QString action );

      void getCrProjectPath   ( QString & path, QString & name );
      bool isUniqueName       ( const QString name );
      bool getNewProjectSpecs ( const QString title,
                                const QString prompt,
                                const QString name_prompt,
                                const QString desc_prompt,
                                const QString ok_text,
                                const QString cancel_text,
                                QString     & name,
                                QString     & desc );

    protected slots:
      void crProjectChanged ( QTreeWidgetItem * current,
                              QTreeWidgetItem * previous );
      void openBtnClicked         ();
      void addProjectBtnClicked   ();
      void insertProjectBtnClicked();
      void editDescBtnClicked     ();
      void deleteProjectBtnClicked();

  };

}

#endif // QTCOFE_PROJECTLISTPAGE_H
