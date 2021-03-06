#!/usr/bin/python

#  ------------------------------------------------------------------
#   Creating new user account
#  ------------------------------------------------------------------
#
#  Function:  create_new_user.do ( inp )
#
#  Input 'inp' is a JSON object:
#
#  {
#    "action"     : "create_new_user",
#    "projects_path": path-to-projects-directory,
#    "data"  : {
#                "user_name" : full_user_name,
#                "login"     : user_login,
#                "email"     : user_email
#              }
#  }
#
#
#  Return is JSON object:
#
#  {
#    "action" : "create_new_user",
#    "result" : result_code,
#    "message": error_message
#  }
#
#  where resut_code is 'OK' if new user account is successfully created
#  and temporary password is e-mailed back to user. Otherwise, this
#  field contains error code with description passed in 'error_message'.
#
#  ------------------------------------------------------------------
#

import sys
import os
#import hashlib
import subprocess
from  account import user
from  project import tree
from  varut   import jsonut, utils, defs

def do ( inp ):

    if inp.action != "create_new_user":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    default_project_data = tree.Tree()
    default_project_data.set_minimal_data ( defs.default_project_name(),
                                            "Default CCP4 project" )

    user_data = user.User()
    pwd = utils.make_id()
    user_data.set_user ( inp.data.user_name,inp.data.email,
                         inp.data.login,[default_project_data],pwd )

    outdata = utils.make_repo (
       utils.get_user_repo_path ( defs.projects_path(),inp.data.login ),
                            defs.user_data_name(),user_data.to_JSON() )

    outdata.action = inp.action
    if outdata.result == "repo_exists":
        return utils.make_return ( inp.action,
                                  "user_exists","User already exists" )
    elif outdata.result != "OK":
        return outdata

    outdata1 = utils.make_repo (
        utils.get_project_repo_path ( defs.projects_path(),
            inp.data.login,defs.default_project_name() ),
                defs.project_data_name(),
                    default_project_data.to_JSON() )
    if outdata1.result != "OK":
        return outdata1

    p = subprocess.Popen ( ['sendmail',inp.data.email],
                           stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE )
    p.communicate ( input='Subject: your CCP4 Cloud account\n' + \
        'Your CCP4 cloud account has been activated using the following data:\n \n' + \
        '  User name : ' + inp.data.user_name + "\n" + \
        '  User email: ' + inp.data.email + "\n" + \
        '  login     : ' + inp.data.login + "\n" + \
        '  password  : ' + pwd + "\n \n" + \
        'Please keep these data for your reference.\n' )
    if p.wait() != 0:
        outdata.result  = "mail_error"
        outdata.message = "Confirmation e-mail could not be sent, rc=" + str(p.returncode)
        return outdata

    outdata.result  = "OK"
    outdata.message = "Account successfully created"
    return outdata

#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#

if __name__ == "__main__":

  inp = jsonut.jObject (
     '{ "action"      : "create_new_user",'
     '  "projects_path" : "./",'
     '  "data"        : {'
     '        "user_name" : "Eugene Krissinel",'
     '        "login"     : "ekr",'
     '        "email"     : "eugene@Mimas.local"'
     '                  }'
     '}' )

  print inp.action
  print inp.data.user_name
  print inp.data.login
  print inp.data.email

  defs.set_projects_path ( defs.test_projects_path() )

  outdata = do ( inp )
  print ( outdata.to_JSON() )

  sys.exit(0)
