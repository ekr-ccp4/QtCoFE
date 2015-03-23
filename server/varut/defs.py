#!/usr/bin/python

#
#  ------------------------------------------------------------------
#   Various definitions
#  ------------------------------------------------------------------
#

#    master_path points on root of all repositories. The variable is
#  either defined here with a non-zero length, or is assigned in
#  cofe_proc_gate.py from input json object.
#    master_path must end with a slash.

__master_path__ = ""

def is_master_path():
    global __master_path__
    if __master_path__:
        return True
    else:
        return False

def set_master_path(mpath):
    global __master_path__
    __master_path__ = mpath
    return

def master_path():  return __master_path__


#  test_master_path is used in module tests by copying it into
#  master_path prior the test.
def test_master_path(): return "/Users/eugene/Projects/QtCoFE/cofe-master/"


#    bin_path points on directory with executable modules, usually
#  $CCP4/bin. The variable is either defined here with a non-zero
#  length, or is assigned in cofe_proc_gate.py from input json object.
#  bin_path must end with a slash.

__bin_path__ = ""

def is_bin_path():
    global __bin_path__
    if __bin_path__:
        return True
    else:
        return False

def set_bin_path(bpath):
    global __bin_path__
    __bin_path__ = bpath
    return

def bin_path():  return __bin_path__


#  test_bin_path is used in module tests by copying it into
#  bin_path prior the test.
def test_bin_path(): return "/Users/eugene/Projects/QtCoFE/cofe-bin/"



def user_data_name      ():  return "user.data"
def project_data_name   ():  return "project.data"
def job_data_name       ():  return "job.data"
def job_arguments_name  ():  return "job.arguments"
def default_project_name():  return "default"

