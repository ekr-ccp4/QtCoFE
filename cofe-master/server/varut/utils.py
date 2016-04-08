#!/usr/bin/python

#
#  ------------------------------------------------------------------
#   Various utils
#  ------------------------------------------------------------------
#

import os
import string
import random
import defs
import gitut
import jsonut


def get_user_repo_path ( projects_path,login ):
    return os.path.join ( projects_path,login + ".user" )


def get_project_repo_path ( projects_path,login,project ):
    return os.path.join ( projects_path,login + ".project." + project )


def get_job_dir_path ( projects_path,login,project,job_id ):
    return os.path.join ( get_project_repo_path(projects_path,login,project),
                          "job." + str(job_id) )

def get_job_dir ( project_repo_path,job_id ):
    return os.path.join ( project_repo_path,"job." + str(job_id) )

def get_data_file ( any_job_dir,job_id,filename ):
    return os.path.normpath (
                os.path.join ( 
                     any_job_dir,"../job." + str(job_id),filename
                             )
                            )


#def set_pythonpath():


def make_return ( action,result,message ):
    res = jsonut.jObject()
    res.action  = action
    res.result  = result
    res.message = message
    return res


def pass_return ( action,result ):
    res = result
    res.action = action
    return res


def add_return ( action,result_obj,data ):
    data.action  = action
    data.result  = result_obj.result
    data.message = result_obj.message
    return data



def make_id ( size=6, chars=string.ascii_uppercase + string.digits ):
    return ''.join(random.choice(chars) for _ in range(size))



def make_repo ( repo_dir,data_name,repo_data ):

    if os.path.isdir(repo_dir):
        return make_return ( "make_repo","repo_exists",
                             "Repo already exists" )

    os.mkdir ( repo_dir )
    rc = gitut.git ( repo_dir,['init'] )
    if rc != "OK":
        return make_return ( "make_repo","git_error_init",rc )

    file = open ( os.path.join(repo_dir,data_name),"w" )
    file.write ( repo_data )
    file.close ()

    rc = gitut.git ( repo_dir,['add','.'] )
    if rc != "OK":
        return make_return ( "make_repo","git_error_add",rc )

    rc = gitut.git ( repo_dir,['commit','-m',"initialisation"] )
    if rc != "OK":
        return make_return ( "make_repo","git_error_commit",rc )

    return make_return ( "make_repo","OK","Repo successfully created" )

