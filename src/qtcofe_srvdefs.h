// $Id: qtcofe_srvdefs.h $
// =================================================================
//
//    06.11.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtcofe_srbdefs <server definitions>
//       ~~~~~~~~~
//  **** Project :  CCP4 CoFE Project
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
// =================================================================
//

#ifndef QTCOFE_SRVDEFS_H
#define QTCOFE_SRVDEFS_H

#define qtCOFE_SERVER_ProcGate                "cofe_proc_gate.py"

#define qtCOFE_SERVER_ACT_Login               "login"
#define qtCOFE_SERVER_ACT_RecoverLogin        "recover_login"
#define qtCOFE_SERVER_ACT_CreateNewUser       "create_new_user"
#define qtCOFE_SERVER_ACT_GetListOfProjects   "get_list_of_projects"
#define qtCOFE_SERVER_ACT_AddProject          "add_project"
#define qtCOFE_SERVER_ACT_SetProjectDesc      "set_project_desc"
#define qtCOFE_SERVER_ACT_DeleteProject       "delete_project"
#define qtCOFE_SERVER_ACT_UnlockProject       "unlock_project"
#define qtCOFE_SERVER_ACT_GetUserData         "get_user_data"
#define qtCOFE_SERVER_ACT_SetUserData         "set_user_data"
#define qtCOFE_SERVER_ACT_GetListOfJobs       "get_list_of_jobs"
#define qtCOFE_SERVER_ACT_AddJob              "add_job"
#define qtCOFE_SERVER_ACT_DelJob              "del_job"
#define qtCOFE_SERVER_ACT_RunJob              "run_job"
#define qtCOFE_SERVER_ACT_GetReportURI        "get_report_uri"
#define qtCOFE_SERVER_ACT_GetDataModel        "get_data_model"
#define qtCOFE_SERVER_ACT_ImportFile          "import_file"
#define qtCOFE_SERVER_ACT_SetData             "set_data"


#define  qtCOFE_TASK_DataImport     "task_dataimport"
#define  qtCOFE_TASK_DataSelection  "task_dataselection"

#endif // QTCOFE_SRVDEFS_H
