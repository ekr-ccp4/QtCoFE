// $Id: qtcofe_defs.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_defs <global definitions>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_DEFS_H
#define QTCOFE_DEFS_H

#define qtCOFE_app_title             "qtCoFE 1.00"
#define qtCOFE_app_date              "06/01/2015"
#define qtCOFE_app_spec              qtCOFE_app_title" "qtCOFE_app_date

#define qtCOFE_Company               "CCP4"
#define qtCOFE_Application           "cofe"

#define qtCOFE_QtPlugins_Directory   "QtPlugins"
#define qtCOFE_QtPlugins_RepPath     "../"
#define qtCOFE_CCP4_Env              "CCP4"

#define UNUSED_ARGUMENT(x) (void)x

#define qtCOFE_app_icon              ":/images/qtcofe.png"

#define qtCOFE_icon_base             ":/images/"

#define qtCOFE_prjlist_icon          ":/images/list.png"
#define qtCOFE_account_icon          ":/images/account.png"
#define qtCOFE_unlock_icon           ":/images/unlock.png"
#define qtCOFE_configure_icon        ":/images/configure.png"
#define qtCOFE_help_icon             ":/images/help.png"
#define qtCOFE_logout_icon           ":/images/logout.png"
#define qtCOFE_exit_icon             ":/images/exit.png"

#define qtCOFE_Ok_icon               ":/images/ok.png"
#define qtCOFE_Ok_Amber_icon         ":/images/ok_amber.png"
#define qtCOFE_Cancel_icon           ":/images/cancel.png"
#define qtCOFE_Close_icon            ":/images/close.png"
#define qtCOFE_Plus_icon             ":/images/plus.png"
#define qtCOFE_Minus_icon            ":/images/minus.png"
#define qtCOFE_Param_icon            ":/images/parameters.png"
#define qtCOFE_Run_icon              ":/images/run.png"
#define qtCOFE_View_icon             ":/images/view.png"
#define qtCOFE_Data_icon             ":/images/data.png"

/*
#define qtRV_print_icon            ":/images/print.png"
#define qtRV_refresh_icon          ":/images/refresh.png"
#define qtRV_refresh_manual_icon   ":/images/refresh_manual.png"
#define qtRV_exportpdf_icon        ":/images/exportpdf.png"
#define qtRV_copy_icon             ":/images/copy.png"
#define qtRV_copy_button_icon      ":/images/copy_button.png"
#define qtRV_close_icon            ":/images/close.png"
#define qtRV_close_tab_icon        ":/images/closetab.png"
#define qtRV_find_icon             ":/images/find.png"
#define qtRV_find_forward_icon     ":/images/find_forward.png"
#define qtRV_find_backward_icon    ":/images/find_backward.png"
#define qtRV_go_forward_icon       ":/images/go_forward.png"
#define qtRV_go_back_icon          ":/images/go_back.png"
#define qtRV_help_program_icon     ":/images/help_program.png"
#define qtRV_help_ccp4_icon        ":/images/help_ccp4.png"
#define item_expanded_icon         ":/images/item_expanded.png"
#define item_collapsed_icon        ":/images/item_collapsed.png"
#define item_inactive_icon         ":/images/item_inactive.png"
#define item_data_icon             ":/images/data.png"


//#ifdef Q_OS_MAC
//#define  qtRV_Doc_Directory        "docs/"
//#else
//#define  qtRV_Doc_Directory        "html/"
//#endif
*/

#define  qtCOFE_DefaultRefreshPeriod  6000
#define  qtCOFE_SmartRefresh          0
#define  qtCOFE_ConstantRefresh       1
#define  qtCOFE_ManualRefresh         2


#define  qtCOFE_Doc_Directory        "html/"

#define  qtCOFE_SET_WinState         "win_state"
#define  qtCOFE_SET_WinSize          "win_size"
#define  qtCOFE_SET_WinPos           "win_pos"
#define  qtCOFE_SET_RefreshPeriod    "refresh_period"
#define  qtCOFE_SET_PrefWinSize      "pref_win_size"
#define  qtCOFE_SET_PrefWinPos       "pref_win_pos"
#define  qtCOFE_SET_TreeWidth        "tree_width"
#define  qtCOFE_SET_PlotWidth        "plot_width"
#define  qtCOFE_SET_PlotHeight       "plot_height"
//#define  qtCOFE_SET_MaxDecorSize     "max_decor_size"
#define  qtCOFE_SET_UseSystemBrowser "use_system_browser"
#define  qtCOFE_SET_RefreshMode      "refresh_mode"
#define  qtCOFE_SET_gdExtSelection   "gd_ext_selection"
#define  qtCOFE_SET_AppDir           "app_dir"
#define  qtCOFE_SET_ServerUri        "server_uri"
#define  qtCOFE_SET_CootPath         "coot_path"
#define  qtCOFE_SET_MGPath           "ccp4mg_path"
#define  qtCOFE_SET_ViewHKLPath      "viewhkl_path"
#define  qtCOFE_SET_BrowserPath      "browser_path"
#define  qtCOFE_SET_ExportDir        "export_dir"
//#define  qtCOFE_SET_LogFind          "log_find"
//#define  qtCOFE_SET_LogFindCase      "log_find_case"
//#define  qtCOFE_SET_LogFindWords     "log_find_words"
#define  qtCOFE_SET_LogFont          "log_font"
#define  qtCOFE_SET_JSRV_WinState    "jsrv_win_state"
#define  qtCOFE_SET_JSRV_WinSize     "jsrv_win_size"
#define  qtCOFE_SET_JSRV_WinPos      "jsrv_win_pos"
#define  qtCOFE_SET_JSRV_ImageFile   "jsrv_image_file"

// session settings
#define  qtCOFE_SET_LoginName        "login_name"
#define  qtCOFE_SET_ProjectsPath     "projects_path"
//#define  qtCOFE_SET_BinPath          "bin_path"
#define  qtCOFE_SET_ProjectID        "project_id"
#define  qtCOFE_SET_ProjectPath      "project_path"
#define  qtCOFE_SET_Password         "magic_key"
#define  qtCOFE_SET_JobID            "job_id"
#define  qtCOFE_SET_KeepLogged       "keep_logged"
#define  qtCOFE_SET_LoggedIn         "logged_in"

// job status
#define  qtCOFE_JOB_Idle             0
#define  qtCOFE_JOB_Starting         100
#define  qtCOFE_JOB_Running          1000
#define  qtCOFE_JOB_Done             2000

// jsrview commands
#define  qtCOFE_JSRV_rvGateName                "rvGate"
#define  qtCOFE_JSRV_COMMAND_Copy              "{copy}"
#define  qtCOFE_JSRV_COMMAND_Export            "{export}"
#define  qtCOFE_JSRV_COMMAND_Coot              "{coot}"
#define  qtCOFE_JSRV_COMMAND_CCP4MG            "{ccp4mg}"
#define  qtCOFE_JSRV_COMMAND_ViewHKL           "{viewhkl}"
#define  qtCOFE_JSRV_COMMAND_Display           "{display}"
#define  qtCOFE_JSRV_COMMAND_Browser           "{browser}"
#define  qtCOFE_JSRV_par_del                   "*"

/*
#define  qtCOFE_SET_ImageFile        "image_file"

#define  XRT_Report                "report"
#define  XRT_Resuls                "xrt:results"
#define  XRT_Title                 "xrt:title"
#define  XRT_Section               "xrt:section"
#define  XRT_KeyText               "xrt:keytext"
#define  XRT_Table                 "xrt:table"
#define  XRT_Graph                 "xrt:graph"
#define  XRT_Files                 "xrt:files"
#define  XRT_Decorations           "xrt:decorations"
#define  XRT_Plot                  "xrt:plot"
#define  XRT_PlotLine              "plotline"
#define  XRT_Data                  "xrt:data"
#define  XRT_File                  "xrt:file"
#define  XRT_Attr_Title            "title"
#define  XRT_Attr_Select           "select"
#define  XRT_Attr_Name             "name"
#define  XRT_Attr_Transpose        "transpose"
#define  XRT_Attr_Folded           "folded"
#define  XRT_Attr_XCol             "xcol"
#define  XRT_Attr_YCol             "ycol"
#define  XRT_Attr_Xmin             "xmin"
#define  XRT_Attr_Xmax             "xmax"
#define  XRT_Attr_Ymin             "ymin"
#define  XRT_Attr_Ymax             "ymax"
#define  XRT_Attr_Xint             "xint"
#define  XRT_Attr_Yint             "yint"
#define  XRT_Attr_Colour           "colour"
#define  XRT_Attr_Key              "key"
#define  XRT_Attr_Type             "type"
#define  XRT_Attr_LineStyle        "linestyle"
#define  XRT_Attr_Path             "path"


#define  FTYPE_XYZ       "xyz"
#define  FTYPE_XYZ_MAP   "xyz:map"
#define  FTYPE_SUMMARY   "summary"
#define  FTYPE_HKL_HKL   "hkl:hkl"
#define  FTYPE_HKL_MAP   "hkl:map"
#define  FTYPE_TEXT      "text"
#define  FTYPE_HTML      "html"
#define  FTYPE_COOT_VIEW "coot_view"
#define  FTYPE_OTHER     "other"


#define  COMMAND_Copy              "{copy}"
#define  COMMAND_Export            "{export}"
#define  COMMAND_Coot              "{coot}"
#define  COMMAND_CCP4MG            "{ccp4mg}"
#define  COMMAND_ViewHKL           "{viewhkl}"
#define  COMMAND_Display           "{display}"
#define  COMMAND_Browser           "{browser}"
*/

#endif // QTCOFE_DEFS_H
