// $Id: qtcofe_project_tree.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_project_tree <interface>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//  **** Classes :  qtCOFE::ProjectTree
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_PROJECT_TREE_H
#define QTCOFE_PROJECT_TREE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QToolButton;
QT_END_NAMESPACE

namespace qtx  {
  class Tree;
}

class QJsonObject;

namespace qtCOFE  {

  class Preferences;
  class Session;
  class DataModel;

  class ProjectTree : public QWidget  {
  Q_OBJECT

    public:
      ProjectTree ( DataModel      *dm,
                    QWidget        *parent = 0,
                    Qt::WindowFlags flags  = 0 );
      virtual ~ProjectTree();

      virtual void makeLayout();

      void makeTree ( QJsonObject & prjData );

      QJsonObject * getTreeData();

      int  currentJobId();
      int  parentJobId ();
      int  siblingJobId();

      void deleteCurrentJob();

      // from current node
      void getProjectedData ( QStringList              & dtypes,
                              QList<QTreeWidgetItem *> & nodes );

      void addProjectedData ( QTreeWidgetItem          * node,
                              QStringList              & dtypes,
                              QList<QTreeWidgetItem *> & nodes );

    signals:
      void add_new_job   ( int parentId );
      void del_job       ( int jobId, int nextCrJobId );
      void view_job_data ( int parentId );
      void crjob_changed ( int jobId  );

    protected:
      Preferences  *preferences;
      Session      *session;
      DataModel    *dataModel;
      qtx::Tree    *jobTree;
      QToolButton  *add_btn;
      QToolButton  *del_btn;
      QToolButton  *data_btn;

      void addJob ( const QJsonObject & obj,
                    QTreeWidgetItem   * item,
                    QTreeWidgetItem  *& crItem,
                    int               & maxID );

    protected slots:
      void addBtnClicked();
      void delBtnClicked();
      void dataBtnClicked();
      void crJobChanged ( QTreeWidgetItem * current,
                          QTreeWidgetItem * previous );

  };

}

#endif // QTCOFE_PROJECT_TREE_H
