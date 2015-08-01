#!/usr/bin/python2.7

#  ------------------------------------------------------------------
#   COFE processing gate (request dispatcher)
#  ------------------------------------------------------------------
#
#  Input is JSON string in sys.argv[1]:
#
#  {
#    "action"       : action-key,
#    "login"        : login-name,
#    "pwd"          : password,
#    "projects_path": path-to-projects-directory,
# ##    "bin_path"     : path-to-bin-directory,
#    "session"      : session-id,
#    "project"      : project-id,
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
from tasks   import dataimport, disambiguator

#print sys.argv[0] + '  ' + sys.argv[1]

inpdata = jsonut.jObject ( sys.argv[1] )

if not defs.is_projects_path():
    defs.set_projects_path ( inpdata.projects_path )

if not os.path.isdir(defs.projects_path()):
    print (
        utils.make_return ( inpdata.action,"wrong_projects_path",
            "Projects Path either does not exist or is not a directory"
                   ).to_JSON() )
    sys.exit(0)


if not defs.is_master_path():
   defs.set_master_path ( os.path.dirname(os.path.abspath(__file__+"/../")) )

#print defs.bin_path()

if not os.path.isdir(defs.bin_path()):
    print (
        utils.make_return ( inpdata.action,"wrong_bin_path",
            "Path to executables either does not exist or is not a directory"
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
    "unlock_project"         : projects.unlock,
    "get_list_of_jobs"       : jobs.get_list,
    "add_job"                : jobs.add,
    "del_job"                : jobs.delete,
    "run_job"                : jobs.run,
    "get_report_uri"         : jobs.get_report_uri,
    "get_data_model"         : datamodel.get_data_model,
    "import_file"            : dataimport.import_file,
    "set_data"               : disambiguator.run
}


ret_string = action.get ( inpdata.action,unknown_action )(inpdata).to_JSON();

# write return to file in master directory to aid debugging
file = open ( os.path.join(defs.master_path(),"requests/_" + inpdata.action + "_last_in.txt"),"w" )
file.write ( sys.argv[1] )
file.close ()

file = open ( os.path.join(defs.master_path(),"requests/_" + inpdata.action + "_last_out.txt"),"w" )
file.write ( ret_string )
file.close ()

#print ( action.get ( inpdata.action,unknown_action )(inpdata).to_JSON() )
print ret_string

sys.exit(0)

