// $Id: qtcofe_page_prjlist.cpp $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_page_prjlist <implementation>
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

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QTreeWidgetItemIterator>

#include "qtcofe_page_prjlist.h"
#include "qtcofe_preferences.h"
#include "qtcofe_session.h"
#include "qtcofe_dialogs.h"
#include "qtcofe_srvdefs.h"
#include "qtcofe_defs.h"
#include "qtx/qtx_tree.h"
#include "qjson/QJsonObject.h"
#include "qjson/QJsonValue.h"
#include "qjson/QJsonArray.h"


qtCOFE::ProjectListPage::ProjectListPage ( DataModel      *dm,
                                           QSettings      *stn,
                                           QWidget        *parent,
                                           Qt::WindowFlags flags )
                       : Page(dm,stn,parent,flags)  {
  projectTree  = NULL;
  openBtn      = NULL;
  addNewBtn    = NULL;
  insertNewBtn = NULL;
  editDescBtn  = NULL;
  deleteBtn    = NULL;
}

qtCOFE::ProjectListPage::~ProjectListPage() {
}

void qtCOFE::ProjectListPage::makeLayout()  {
QGridLayout *gbox;
QLabel      *label;
//QFrame      *hline;
int          row;

  setContentsMargins ( 100,0,75,0 );

  gbox = new QGridLayout();
  row  = 0;
  gbox->addWidget ( new QLabel("<i>&nbsp;</i>"),row,0,1,1 );

  label = new QLabel("My Projects");
  label->setStyleSheet ( "font-style: italic; font-weight: bold; " +
                         preferences->getFontSizeStyleSheet(1.5) );
  gbox->addWidget ( label,row++,0,1,3 );

  projectTree = new qtx::Tree();
  projectTree->setColumnCount(2);
  projectTree->setHeaderLabels ( QStringList() << "Project name"
                                               << "Description" );
  projectTree->setMinimumHeight ( 250 );
  projectTree->setMaximumHeight ( 250 );
  gbox->addWidget ( projectTree,row,0,6,1 );
  gbox->addWidget ( new QLabel("<i>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>"),
                    row,1,3,1 );

  openBtn = makeDarkButton ( "Open selected project" );
  gbox->addWidget ( openBtn,row++,2,1,1 );

  addNewBtn = makeLightButton ( "Add new project" );
  gbox->addWidget ( addNewBtn,row++,2,1,1 );

  insertNewBtn = makeLightButton ( "Insert new project" );
  gbox->addWidget ( insertNewBtn,row++,2,1,1 );

  editDescBtn = makeLightButton ( "Edit description" );
  gbox->addWidget ( editDescBtn,row++,2,1,1 );

  deleteBtn = makeLightButton ( "Delete project" );
  gbox->addWidget ( deleteBtn,row++,2,1,1 );

  gbox->addWidget ( new QLabel("<i>&nbsp;</i>"),row++,0,1,1 );
  gbox->addWidget ( new QLabel("<i>&nbsp;</i>"),row++,0,1,1 );
  gbox->addWidget ( new QLabel("<i>&nbsp;</i>"),row++,0,1,1 );
  gbox->addWidget ( new QLabel("<i>&nbsp;</i>"),row,0,1,1 );

  gbox->setColumnStretch ( 0,100 );
  gbox->setColumnStretch ( 1,0 );
  gbox->setColumnStretch ( 2,0 );
//  gbox->setColumnStretch ( 3,1 );

  gbox->setAlignment ( Qt::AlignLeft | Qt::AlignCenter );
  gbox->setVerticalSpacing ( 8 );
  setLayout ( gbox );

  connect ( openBtn,SIGNAL(clicked()),
            this,SLOT(openBtnClicked()) );
  connect ( addNewBtn,SIGNAL(clicked()),
            this,SLOT(addProjectBtnClicked()) );
  connect ( insertNewBtn,SIGNAL(clicked()),
            this,SLOT(insertProjectBtnClicked()) );
  connect ( editDescBtn,SIGNAL(clicked()),
            this,SLOT(editDescBtnClicked()) );
  connect ( deleteBtn,SIGNAL(clicked()),
            this,SLOT(deleteProjectBtnClicked()) );

  connect ( projectTree,
      SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
      this,SLOT(crProjectChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );

}

void qtCOFE::ProjectListPage::addProjectName (
                                    const QJsonObject   & obj,
                                    QTreeWidgetItem     *item,
                                    const QString & crPrjName,
                                    QTreeWidgetItem *& crItem )  {
QString          pname = obj.value("name").toString();
QString          pdesc = obj.value("desc").toString();
QTreeWidgetItem *item1 = projectTree->addTreeItem ( item,pname );

  item1->setText ( 1,pdesc );

  if (pname==crPrjName)
    crItem = item1;

  if (obj.contains("projects"))  {
    QJsonArray prjs = obj.value("projects").toArray();
    int n = prjs.count();
    for (int i=0;i<n;i++)
      addProjectName ( prjs[i].toObject(),item1,crPrjName,crItem );
  }

}

void qtCOFE::ProjectListPage::fillProjectList()  {
QJsonObject jsonData;
  project_query ( jsonData,qtCOFE_SERVER_ACT_GetListOfProjects );
}

void qtCOFE::ProjectListPage::project_query ( QJsonObject & jsonData,
                                              QString action )  {

  if (!projectTree)  return;

  Server      server(preferences,session,this);
  QJsonObject jsonReply;
  SERVER_RC   rc = server.call ( action,jsonData,jsonReply );

  projectTree->clear();
  projectTree->setColumnCount ( 2 );
  projectTree->setSelectionBehavior ( QAbstractItemView::SelectRows );
  projectTree->setSelectionMode  ( QAbstractItemView::SingleSelection );
  projectTree->setHeaderLabels   ( QStringList() << "Project name"
                                                 << "Description" );
  projectTree->setAlternatingRowColors ( true );

  if (rc!=SERVER_RC_Ok)  {
    server.makeErrorMessage(this);
  } else if (!jsonReply.keys().contains("result",Qt::CaseInsensitive))  {
    QMessageBox::information ( this,"JSON Reply",
      "Result key is missed. This is a program error, please "
      "report to developers" );
  } else  {
    bool areProjects =
              jsonReply.keys().contains("projects",Qt::CaseInsensitive);
    if (jsonReply.value("result").toString()!="OK")  {
      QString message = "Operation failed, result code is <b>" +
              jsonReply.value("result").toString() + "</b>:<p><i>" +
              jsonReply.value("message").toString() + "</i>.";
      if (!areProjects)
        message.append ( "<p>List of projects could not be read." );
      QMessageBox::information ( this,"Error",message );
    }
    if (areProjects)  {
      QJsonArray prjs = jsonReply.value("projects").toArray();
      int  n = prjs.count();
      if (n>0)  {
        if (jsonReply.keys().contains("current_project",
                                      Qt::CaseInsensitive))
          session->projectID =
                         jsonReply.value("current_project").toString();
        QTreeWidgetItem *crItem = NULL;
        for (int i=0;i<n;i++)
          addProjectName ( prjs[i].toObject(),NULL,
                           session->projectID,crItem );
        if (crItem)
          projectTree->setCurrentItem ( crItem );
        session->projectPath = getCrProjectPath();
      }
    }
  }

  projectTree->setFullWidth();

}


QString qtCOFE::ProjectListPage::getCrProjectPath()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          path;

  while (item)  {
    path = "/" + item->text(0) + path;
    item = item->parent();
  }

  return path;

}


void qtCOFE::ProjectListPage::getCrProjectPath ( QString & path,
                                                 QString & name )  {
QTreeWidgetItem *item = projectTree->currentItem();

  path.clear();
  name.clear();

  if (item)  {
    name = item->text(0);
    item = item->parent();
  }

  while (item)  {
    path = "/" + item->text(0) + path;
    item = item->parent();
  }

}

bool qtCOFE::ProjectListPage::isUniqueName ( const QString name )  {
QTreeWidgetItemIterator it(projectTree);
bool                    good = true;

  while (*it && good)  {
    good = ((*it)->text(0)!=name);
    ++it;
  }

  return good;

}

void qtCOFE::ProjectListPage::crProjectChanged (
                                        QTreeWidgetItem * current,
                                        QTreeWidgetItem * previous )  {
UNUSED_ARGUMENT(previous);
bool isDefault;

  if (!current)  return;

  isDefault = current->text(0) == "default";
  editDescBtn->setVisible ( !isDefault );
  deleteBtn  ->setVisible ( !isDefault );

  if (session->projectID!=current->text(0))  {
    session->projectID = current->text(0);
    session->jobID     = -1;
  }

}

void qtCOFE::ProjectListPage::openBtnClicked()  {

  if (projectTree)  {
//    QString  path,name;
//    getCrProjectPath ( path,name );
//    preferences->masterDataPath = path + "/" + name;
    emit open_project();
  }

}

bool qtCOFE::ProjectListPage::getNewProjectSpecs (
                                             const QString title,
                                             const QString prompt,
                                             const QString name_prompt,
                                             const QString desc_prompt,
                                             const QString ok_text,
                                             const QString cancel_text,
                                             QString     & name,
                                             QString     & desc )  {
qtCOFE::Text2Dialog *t2dlg = new qtCOFE::Text2Dialog ( this,title );
bool                 done  = false;

  name.clear();
  desc.clear();

  t2dlg->setPrompt ( prompt );

  // project names start with a letter, can have digits, "-" and "_",
  // ane be no longer than 16 characters
  t2dlg->setInput1 ( name_prompt,"","^[a-zA-Z]{1}[\\w,-]{,15}" );
  t2dlg->setInput2 ( desc_prompt,"","" );

  t2dlg->setOkButton     ( ok_text    ,qtCOFE_Ok_icon );
  t2dlg->setCancelButton ( cancel_text,qtCOFE_Cancel_icon );

  while (!done)
    if (t2dlg->exec()==QDialog::Accepted)  {
      name = t2dlg->getInput1();
      desc = t2dlg->getInput2();
      if (name.isEmpty() && (name_prompt!="#"))
        QMessageBox::information ( this,"Empty Project Name",
              "<b>Error:</b> <i>project name cannot be empty</i>.<p>"
              "Please try again." );
      else if (!isUniqueName(name))
        QMessageBox::information ( this,"Duplicate Project Name",
              "<b>Error:</b> <i>project '"+name+"' already exists</i>.<p>"
              "Please choose a different name." );
      else
        done = true;
    } else
      break;

  delete t2dlg;

  return done;

}

void qtCOFE::ProjectListPage::addProjectBtnClicked()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          name,desc;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
          "Please first select a project in the list." );
  } else if (getNewProjectSpecs ( "Add New Project",
                  "<i>Please type name and description of "
                  "new project to be added after the selected one:</i>",
                  "Project name","Description",
                  "Add","Cancel",name,desc ))  {
    QJsonObject jsonData;
    if (item->parent())
          jsonData.insert ( "parent",item->parent()->text(0) );
    else  jsonData.insert ( "parent",QString("") );
    jsonData.insert ( "sibling",item->text(0) );
    jsonData.insert ( "name"   ,name   );
    jsonData.insert ( "desc"   ,desc   );
    project_query   ( jsonData,qtCOFE_SERVER_ACT_AddProject );
  }

}

void qtCOFE::ProjectListPage::insertProjectBtnClicked()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          name,desc;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
                      "Please first select a project in the list." );
  } else if (getNewProjectSpecs ( "Insert New Project",
                "<i>Please type name and description of "
                "new project to be added as a child of the "
                "selected one:</i>",
                "Project name","Description",
                "Insert","Cancel",name,desc ))  {
    QJsonObject jsonData;
    jsonData.insert ( "parent" ,item->text(0) );
    jsonData.insert ( "sibling",QString("")   );
    jsonData.insert ( "name"   ,name          );
    jsonData.insert ( "desc"   ,desc          );
    project_query   ( jsonData,qtCOFE_SERVER_ACT_AddProject );
  }

}

void qtCOFE::ProjectListPage::deleteProjectBtnClicked()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          name,desc,prompt;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
                      "Please first select a project in the list." );
  } else  {

    if (item->childCount()>0)
      prompt = "<div align='right'><i>Please confirm deletion of the "
               "selected project<br><b><u>and all its child projects"
               "</u></b>:</i></div>";
    else
      prompt = "<div align='right'><i>Please confirm deletion of the "
               "selected project:</i></div>";

    if (getNewProjectSpecs ( "Delete Project",prompt,
              "#","#","Delete","Cancel",name,desc ))  {
      QTreeWidgetItem *parent = item->parent();
      QTreeWidgetItem *next;
      if (parent)  {
        int n = parent->indexOfChild ( item );
        if (n<parent->childCount()-1)  next = parent->child(n+1);
        else if (n>0)                  next = parent->child(n-1);
                                 else  next = parent;
      } else  {
        int n = projectTree->indexOfTopLevelItem ( item );
        if (n<projectTree->topLevelItemCount()-1)
                       next = projectTree->topLevelItem(n+1);
        else if (n>0)  next = projectTree->topLevelItem(n-1);
        else  {
          QMessageBox::information ( this,"Can not delete",
                      "It is not possible to delete all projects" );
          return;
        }
      }
      QJsonObject jsonData;
      jsonData.insert ( "name",item->text(0) );
      jsonData.insert ( "next",next->text(0) );
      project_query   ( jsonData,qtCOFE_SERVER_ACT_DeleteProject );
    }

  }

}

void qtCOFE::ProjectListPage::unlockCurrentProject()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          name,desc,prompt,prjname;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
                      "Please first select a project in the list." );
  } else  {

    prjname = item->text(0);
    prompt = "<div align='right'><i>Please confirm unlocking "
             "current project '" + prjname + "':</i></div>";

    if (getNewProjectSpecs ( "Unlock Project",prompt,
                             "#","#","Unlock","Cancel",name,desc ))  {
      QJsonObject jsonData;
      jsonData.insert ( "name",prjname );
      jsonData.insert ( "next",prjname );
      project_query   ( jsonData,qtCOFE_SERVER_ACT_UnlockProject );
      prompt = "<div align='right'><i>Project '" + prjname +
               "' is now unlocked.</i></div>";
      getNewProjectSpecs ( "Project unlocked",prompt,
                           "#","#","","Ok",name,desc );
    }

  }

}


void qtCOFE::ProjectListPage::editDescBtnClicked()  {
QTreeWidgetItem *item = projectTree->currentItem();
QString          name,desc;

  if (!item)  {
    QMessageBox::information ( this,"No Selection",
                      "Please first select a project in the list." );
  } else if (getNewProjectSpecs ( "Edit Project Description",
              "&nbsp;<i>Please type new description for the selected "
              "project:</i>","#","","Set","Cancel",name,desc ))  {
    QJsonObject jsonData;
    jsonData.insert ( "name",item->text(0) );
    jsonData.insert ( "desc",desc          );
    project_query   ( jsonData,qtCOFE_SERVER_ACT_SetProjectDesc );
  }

}
