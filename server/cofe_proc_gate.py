#!/usr/bin/python

#  ------------------------------------------------------------------
#    COFE processing gate (request dispatcher)
#  ------------------------------------------------------------------
#
#  Input is JSON string in sys.argv[1]:
#
#  {
#    "action"     : action-key,
#    "login"      : login-name,
#    "pwd"        : password,
#    "master_path": path-to-master-directory,
#    "session"    : session-id,
#    "project"    : project-id,
#    "data"  : {
#                ... request-specific fields ...
#              }
#  }
#
#
#  Output is JSON string printed in stdout:
#
#  {
#    "action" : action-key,
#    "result" : result-key,
#    "message": return-message
#    ... request-specific fields ...
#  }
#
#  ------------------------------------------------------------------
#

import os
import sys
from varut   import jsonut, defs, utils
from account import create_new_user, login, projects
from project import jobs, datamodel

#print sys.argv[0] + '  ' + sys.argv[1]

inpdata = jsonut.jObject ( sys.argv[1] )

if not defs.is_master_path():
    defs.set_master_path ( inpdata.master_path )

if not os.path.isdir(defs.master_path()):
    print (
        utils.make_return ( inpdata.action,"wrong_master_path",
            "Master Path either does not exist or is not a directory"
                   ).to_JSON() )
    sys.exit(0)

def unknown_action(inp):
    return utils.make_return ( inp.action,"unknown_action_code",
                        "Unknown action code '" + inp.action + "'" )

action = {
    "login"                  : login.check,
    "recover_login"          : login.recover,
    "get_user_data"          : login.get_data,
    "set_user_data"          : login.set_data,
    "create_new_user"        : create_new_user.do,
    "get_list_of_projects"   : projects.get_list,
    "add_project"            : projects.add,
    "set_project_desc"       : projects.set_desc,
    "delete_project"         : projects.delete,
    "get_list_of_jobs"       : jobs.get_list,
    "add_job"                : jobs.add,
    "del_job"                : jobs.delete,
    "get_data_model"         : datamodel.get_data_model
}


print ( action.get ( inpdata.action,unknown_action )(inpdata).to_JSON() )

sys.exit(0)

