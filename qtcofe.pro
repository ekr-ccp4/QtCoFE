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
    src/qtcofe_dialogs.cpp

RESOURCES += \
    src/qtcofe.qrc

OTHER_FILES += \
    src/qtcofe.rc \
    server/account/login.py \
    server/account/create_new_user.py \
    server/account/projects.py \
    server/account/user.py \
    server/cofe_proc_gate.py \
    server/varut/jsonut.py \
    server/varut/gitut.py \
    server/varut/utils.py \
    server/varut/defs.py \
    server/project/jobs.py \
    server/project/job.py \
    server/project/tree.py \
    server/project/task.py \
    server/project/dtype.py \
    server/tasks/root.py \
    server/tasks/data_import.py \
    server/tasks/data_processing.py \
    server/tasks/model_building.py \
    server/tasks/phasing_ep_mad.py \
    server/tasks/phasing_ep_sad.py \
    server/tasks/phasing_mr.py \
    server/tasks/refinement.py \
    server/dtypes/dummy.py \
    server/dtypes/any.py \
    server/dtypes/xyz.py \
    server/dtypes/structure.py \
    server/dtypes/sequence.py \
    server/dtypes/images.py \
    server/dtypes/hkl.py \
    server/dtypes/abcd.py \
    server/project/datamodel.py \
    server/dtypes/__init__.py \
    server/tasks/__init__.py \
    server/tasks/data_disambiguator.py

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
    src/qtcofe_dialogs.h

