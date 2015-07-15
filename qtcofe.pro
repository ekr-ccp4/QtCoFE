#-------------------------------------------------
#
# Project created by QtCreator 2011-07-06T00:39:42
#
#-------------------------------------------------

QT         += core gui xml xmlpatterns svg webkit network

TARGET      = qtcofe
TEMPLATE    = app

MOC_DIR     = ./moc
OBJECTS_DIR = ./obj
RCC_DIR     = ./moc
DESTDIR     = ./bin
ICON        = ./src/images/qtcofe.icns
CONFIG     += x86_64
#CONFIG     -= app_bundle

QMAKE_CFLAGS_RELEASE   -= -O1
QMAKE_CFLAGS_RELEASE   -= -O2
QMAKE_CFLAGS_RELEASE   += -O3
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += \
    qjson/QJsonArray.cpp \
    qjson/QJsonDocument.cpp \
    qjson/QJsonObject.cpp \
    qjson/QJsonParseError.cpp \
    qjson/QJsonParser.cpp \
    qjson/QJsonValue.cpp \
    qjson/QJsonValueRef.cpp \
    qtx/qtx_lineedit.cpp \
    qtx/qtx_table.cpp  \
    qtx/qtx_tree.cpp  \
    qtx/qtx_utils.cpp  \
    src/qtcofe.cpp \
    src/qtcofe_mainwindow.cpp \
    src/qtcofe_base.cpp \
    src/qtcofe_preferences.cpp \
    src/qtcofe_page.cpp \
    src/qtcofe_page_login.cpp \
    src/qtcofe_server.cpp \
    src/qtcofe_page_account.cpp \
    src/qtcofe_page_newuser.cpp \
    src/qtcofe_pages.cpp \
    src/qtcofe_page_prjlist.cpp \
    src/qtcofe_session.cpp \
    src/qtcofe_page_project.cpp \
    src/qtcofe_job.cpp \
    src/qtcofe_project_tree.cpp \
    src/qtcofe_datamodel.cpp \
    src/qtcofe_dialog_data.cpp \
    src/qtcofe_dialog_import.cpp \
    src/qtcofe_dialog_task.cpp \
    src/qtcofe_dialogs.cpp \
    src/qtcofe_jsrvgate.cpp \
    src/qtcofe_jsrview.cpp \
    qtx/qtx_printutils.cpp

RESOURCES += \
    src/qtcofe.qrc

OTHER_FILES += \
    src/qtcofe.rc \
    cofe-master/server/account/login.py \
    cofe-master/server/account/create_new_user.py \
    cofe-master/server/account/projects.py \
    cofe-master/server/account/user.py \
    cofe-master/server/cofe_proc_gate.py \
    cofe-master/server/varut/jsonut.py \
    cofe-master/server/varut/gitut.py \
    cofe-master/server/varut/utils.py \
    cofe-master/server/varut/defs.py \
    cofe-master/server/project/jobs.py \
    cofe-master/server/project/job.py \
    cofe-master/server/project/tree.py \
    cofe-master/server/project/task.py \
    cofe-master/server/project/dtype.py \
    cofe-master/server/tasks/root.py \
    cofe-master/server/tasks/dataimport.py \
    cofe-master/server/tasks/dataprocessing.py \
    cofe-master/server/tasks/building.py \
    cofe-master/server/tasks/mad.py \
    cofe-master/server/tasks/sad.py \
    cofe-master/server/tasks/mr.py \
    cofe-master/server/tasks/refinement.py \
    cofe-master/server/dtypes/dummy.py \
    cofe-master/server/dtypes/any.py \
    cofe-master/server/dtypes/xyz.py \
    cofe-master/server/dtypes/structure.py \
    cofe-master/server/dtypes/sequence.py \
    cofe-master/server/dtypes/images.py \
    cofe-master/server/dtypes/hkl.py \
    cofe-master/server/dtypes/abcd.py \
    cofe-master/server/project/datamodel.py \
    cofe-master/server/dtypes/__init__.py \
    cofe-master/server/tasks/__init__.py \
    cofe-master/server/tasks/disambiguator.py \
    cofe-master/server/process.py \
    cofe-master/bin/mad \
    cofe-master/server/varut/mtz.py

HEADERS += \
    qjson/QJsonArray.h \
    qjson/QJsonDocument.h \
    qjson/QJsonObject.h \
    qjson/QJsonParseError.h \
    qjson/QJsonParser.h \
    qjson/QJsonRoot.h \
    qjson/QJsonValue.h \
    qjson/QJsonValueRef.h \
    qtx/qtx_lineedit.h \
    qtx/qtx_table.h  \
    qtx/qtx_tree.h   \
    qtx/qtx_utils.h   \
    src/qtcofe_defs.h \
    src/qtcofe_mainwindow.h \
    src/qtcofe_base.h \
    src/qtcofe_preferences.h \
    src/qtcofe_page.h \
    src/qtcofe_page_login.h \
    src/qtcofe_server.h \
    src/qtcofe_page_account.h \
    src/qtcofe_page_newuser.h \
    src/qtcofe_pages.h \
    src/qtcofe_page_prjlist.h \
    src/qtcofe_session.h \
    src/qtcofe_srvdefs.h \
    src/qtcofe_page_project.h \
    src/qtcofe_job.h \
    src/qtcofe_project_tree.h \
    src/qtcofe_datamodel.h \
    src/qtcofe_dialog_data.h \
    src/qtcofe_dialog_import.h \
    src/qtcofe_dialog_task.h \
    src/qtcofe_dialogs.h \
    src/qtcofe_jsrvgate.h \
    src/qtcofe_jsrview.h \
    qtx/qtx_printutils.h

