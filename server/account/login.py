#!/usr/bin/python

#  ------------------------------------------------------------------
#   Login (checking user credentials)
#  ------------------------------------------------------------------
#
#  Function:  login.check ( inp )
#
#  Input 'inp' is a JSON object:
#
#  {
#    "action"     : "login",
#    "master_path": path-to-master-directory,
#    "data"       : {
#                     "login" : user_login,
#                     "pwd"   : user_password (md5)
#                   }
#  }
#
#
#  Return is JSON object:
#
#  {
#    "action" : "login",
#    "result" : result_code,
#    "message": error_message
#  }
#
#  where resut_code is 'OK' if login is successful, and "rejected"
#  otherwise. In case of an error, 'result_code' contains the error
#  code with description passed in 'error_message'.
#
#  -------------------------------------------------------------------
#
#  Function:  login.get_data ( inp )
#
#  Input 'inp' is a JSON object:
#
#  {
#    "action"     : "get_user_data",
#    "master_path": path-to-master-directory,
#    "login"      : user_login,
#    "pwd"        : password (Md5)
#  }
#
#
#  Return is JSON object:
#
#  {
#    "action" : "login",
#    "result" : result_code,
#    "message": error_message,
#    "data"   : {
#                 "user_name" : full_user_name,
#                 "login"     : user_login,
#                 "email"     : user_email
#               }
#  }
#
#  where resut_code is 'OK' if login is confirmed, and "rejected"
#  otherwise. In case of an error, 'result_code' contains the error
#  code with description passed in 'error_message'. 'data' returns
#  user account data.
#
#  -------------------------------------------------------------------
#
#  Function:  login.set_data ( inp )
#
#  Input 'inp' is a JSON object:
#
#  {
#    "action"     : "get_user_data",
#    "master_path": path-to-master-directory,
#    "login"      : user_login,
#    "pwd"        : password (Md5),
#    "data"       : {
#                     "user_name" : full_user_name,
#                     "login"     : user_login,
#                     "email"     : user_email
#                     "pwd"        : password (Md5),
#                   }
#  }
#
#
#  Return is JSON object:
#
#  {
#    "action" : "login",
#    "result" : result_code,
#    "message": error_message
#  }
#
#  where resut_code is 'OK' if change is confirmed, and "rejected"
#  otherwise. In case of an error, 'result_code' contains the error
#  code with description passed in 'error_message'. 'data' returns
#  user account data.
#
#  -------------------------------------------------------------------
#
#  Function:  login.recover ( inp )
#
#  Input 'inp' is a JSON object:
#
#  {
#    "action"     : "recover_login",
#    "master_path": path-to-master-directory,
#    "data"       : {
#                     "email" : user_email
#                   }
#  }
#
#  Return is JSON object:
#
#  {
#    "action" : "recover_login",
#    "result" : result_code,
#    "message": error_message
#  }
#
#  where resut_code is 'OK' if user's e-mail have been found, account
#  recovered and new data e-mailed to user. In case of an error,
#  'result_code' contains the error code with description passed in
#  'error_message'.
#
#  ------------------------------------------------------------------
#

import sys
import os
import subprocess
import glob
import hashlib
from   varut   import jsonut, gitut, utils, defs
from   account import user


def check ( inp ):

    if inp.action != "login":
        return utils.make_return ( inp.action,"wrong_action_code",
                                              "Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.master_path(),inp.data.login )
    if not os.path.isdir(user_repo_dir):
        return utils.make_return ( inp.action,"rejected","Login rejected" )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )
    gitut.unlock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    if (user_data.login != inp.data.login) or \
       (user_data.pwd.lower() != inp.data.pwd.lower()):
        return utils.make_return ( inp.action,"rejected",
                                              "Login rejected" )

    result.result  = "OK"
    result.message = "Login approved"
    return utils.make_return ( inp.action,"OK","Login approved" )


def get_data ( inp ):

    if inp.action != "get_user_data":
        return utils.make_return ( inp.action,"wrong_action_code",
                                             "Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.master_path(),inp.login )
    if not os.path.isdir(user_repo_dir):
        return utils.make_return ( inp.action,"rejected",
                                              "Login rejected (1)" )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.make_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )
    gitut.unlock ( user_repo_dir )
    if result.result != "OK":
        result.data = ""
        return utils.pass_return ( inp.action,result )

    if (user_data.login != inp.login) or \
       (user_data.pwd.lower() != inp.pwd.lower()):
        return utils.make_return ( inp.action,"rejected",
                                              "Login rejected (2)" )

    result = utils.make_return ( inp.action,"OK","Login confirmed" )
    result.data           = jsonut.jObject()
    result.data.user_name = user_data.user_name
    result.data.login     = user_data.login
    result.data.email     = user_data.email
    return result


def set_data ( inp ):

    if inp.action != "set_user_data":
        return utils.make_return ( inp.action,"wrong_action_code",
                                              "Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.master_path(),inp.login )
    if not os.path.isdir(user_repo_dir):
        return utils.make_return ( inp.action,"rejected",
                                              "Login rejected (1)" )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )
    if result.result != "OK":
        gitut.unlock ( user_repo_dir )
        result.data = ""
        return utils.pass_return ( inp.action,result )

    if (user_data.login != inp.login) or \
        (user_data.pwd.lower() != inp.pwd.lower()):
        gitut.unlock ( repo_dir )
        return utils.make_return ( inp.action,"rejected",
                                              "Login rejected (3)" )

    user_data.user_name = inp.data.user_name
    user_data.login     = inp.data.login
    user_data.email     = inp.data.email
    if inp.data.pwd:
        user_data.pwd = inp.data.pwd

    user_data.write ( user_repo_dir )

    result = gitut.commit ( user_repo_dir,[defs.user_data_name()],
                            "update login data" )
    gitut.unlock ( user_repo_dir )

    if result.result == "OK":
        result.message = "Login data changed"

    return utils.pass_return ( inp.action,result )


def recover ( inp ):

    if inp.action != "recover_login":
        return utils.make_return ( inp.action,"wrong_action_code",
                                              "Wrong action code" )

    user_repo_list = glob.glob ( defs.master_path() + "*.user" )
    recover_list   = []

    for user_repo_dir in user_repo_list:
        if os.path.isdir(user_repo_dir):
            result = gitut.lock ( user_repo_dir )
            if result.result == "OK":
                user_data = user.User()
                res = user_data.read ( user_repo_dir )
                if res.result == "OK":
                    if user_data.email == inp.data.email:
                        pwd = utils.make_id()
                        recover_list += [[user_data.login,pwd]]
                        user_data.pwd = hashlib.md5(pwd).hexdigest()
                        user_data.write ( user_repo_dir )
                        gitut.commit ( user_repo_dir,[defs.user_data_name()],
                                                     "update login data" )
                gitut.unlock ( user_repo_dir )

    if len(recover_list) ==0:
        return utils.make_return ( inp.action,"nothing_found",
                                  "No accounts with given e-mail found" )

    letter = \
      "Subject: your CCP4 Cloud account(s)\n" + \
      "This letter is sent by CCP4 Cloud as a response for request " + \
      "to recover lost user login names and/or passwords. The " + \
      "following account(s) have been recovered:\n\n"

    for acc in recover_list:
        letter += "Login name  : " + acc[0] + "\n" + \
                  "New password: " + acc[1] + "\n" + \
                  "~~~~~~~~~~~~~~~~~\n\n"

    letter += "Please keep these data for your referece.\n"

    p = subprocess.Popen ( ['sendmail',inp.data.email],
                           stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE )
    p.communicate ( input=letter )
    if p.wait() != 0:
        return utils.make_return ( inp.action,"mail_error",
                   "Recovery e-mail could not be sent, rc=" + \
                   str(p.returncode) )

    return utils.make_return ( inp.action,"OK",
                               "Recovery e-mail has been sent" )


#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#

if __name__ == "__main__":

  if len(sys.argv) < 4:
      print " Call like './login.py login password email'";
      sys.exit ( 1 )

  pwd = hashlib.md5(sys.argv[2]).hexdigest()

  inp = jsonut.jObject (
     '{ "action"      : "login",'
     '  "master_path" : "./",'
     '  "data"        : {'
     '        "login" : "' + sys.argv[1] + '",'
     '        "pwd"   : "' + pwd + '"'
     '                  }'
     '}' )

  print inp.action
  print inp.data.login
  print inp.data.pwd

  defs.set_master_path ( defs.test_master_path() )

  print '============================================================'

  result = check ( inp )
  print ( result.to_JSON() )

  print '============================================================'

  inp1 = jsonut.jObject (
     '{ "action"      : "get_user_data",'
     '  "master_path" : "./",'
     '  "login" : "' + sys.argv[1] + '",'
     '  "pwd"   : "' + pwd + '"'
     '}' )

  result = get_data ( inp1 )
  print ( result.to_JSON() )

  print '============================================================'

  inp2 = jsonut.jObject (
     '{ "action"      : "recover_login",'
     '  "master_path" : "./",'
     '  "data"        : {'
     '     "email" : "' + sys.argv[3] + '"'
     '                  }'
     '}' )

  result = recover ( inp2 )
  print ( result.to_JSON() )

  sys.exit(0)

