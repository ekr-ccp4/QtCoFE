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

def get_user_repo_path ( master_path,login ):
    if master_path.endswith("/"):
        return master_path + login + ".user"
    else:
        return master_path + "/" + login + ".user"

def get_project_repo_path ( master_path,login,project ):
    if master_path.endswith("/"):
        return master_path + login + ".project." + project
    else:
        return master_path + "/" + login + ".project" + project

def get_job_dir_path ( master_path,login,project,job_id ):
    return get_project_repo_path(master_path,login,project) + \
           "/job." + str(job_id)

def make_return ( action,result,message ):
    res = jsonut.jObject()
    res.action  = action
    res.result  = result
    res.message = message
    return res

def pass_return ( action,result ):
    res = result
    res.action  = action
    return res

def minimum_job_data ( name,desc,type,jobId ):
    job_data = jsonut.jObject()
    job_data.name     = name   # if empty then taken from task
    job_data.desc     = desc   # if empty then taken from task
    job_data.type     = type
    job_data.id       = jobId
    job_data.jobs     = []     # child job(s)
    job_data.data     = []
    job_data.expanded = True
    return job_data

def minimum_project_data ( name,desc ):
    project_data = jsonut.jObject()
    project_data.name        = name
    project_data.desc        = desc
    project_data.job_count   = 0
    project_data.current_job = 0;
    project_data.jobs        = [minimum_job_data(name,"Project root",
                               "task_root",project_data.job_count)]
    return project_data


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


def get_user_data(repo_dir):
#  Lock repository before calling this function

    if not os.path.isdir(repo_dir):
        result = jsonut.jObject()
        result.result   = "no_repo_dir"
        result.message  = "User data repository not found"
        result.projects = []
        return result

    result = gitut.checkout ( repo_dir,[] )
    if result.result != "OK":
        result.projects = []
        return result

    result = jsonut.jObject (
                 open(repo_dir + "/" + defs.user_data_name()).read() )

    return result

def write_user_data(repo_dir,user_data):
    file = open ( repo_dir + "/" + defs.user_data_name(),"w" )
    file.write ( user_data.to_JSON() )
    file.close ()
    return

def get_project_data(repo_dir):
#  Lock repository before calling this function

    if not os.path.isdir(repo_dir):
        result = jsonut.jObject()
        result.result  = "no_repo_dir"
        result.message = "Project data repository not found"
        return result

    result = gitut.checkout ( repo_dir,[] )
    if result.result != "OK":
        result.jobs = []
        return result

    result = jsonut.jObject (
                 open(repo_dir + "/" + defs.project_data_name()).read() )

    return result

def write_project_data(repo_dir,project_data):
    file = open ( repo_dir + "/" + defs.project_data_name(),"w" )
    file.write ( project_data.to_JSON() )
    file.close ()
    return


def write_job_data(project_repo_dir,job_data):
    file = open ( project_repo_dir + "/job." + str(job_data.id) + \
                  "/" + defs.job_data_name(),"w" )
    file.write ( job_data.to_JSON() )
    file.close ()
    return
