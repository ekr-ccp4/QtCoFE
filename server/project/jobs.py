#!/usr/bin/python

#  ------------------------------------------------------------------
#   Serving job list queries
#  ------------------------------------------------------------------
#
#  Functions:  jobs.get_list ( inp )
#              jobs.add      ( inp )
#              jobs.delete   ( inp )
#
#
#  Input 'inp' is JSON string in sys.argv[1]:
#
#  {
#    "action"      : action_key,
#    "login"       : user_login,
#    "pwd"         : password,  (md5)
#    "master_path" : master_path,
#    "project"     : project-id,
#    "data"        :  {
#        "parent"    : job-id,
#        "task_type" : task-type,
#        "job"       : job-id,
#        "next"      : job-id
#    }
#  }
#
#  Action key values:
#
#    "get_list_of_jobs" : just returns the job tree; "data"
#                         block is ignored.
#                               Function: get_list ( inp )
#
#    "add_job"          : adds new empty job as a child of job
#                         specified in "data/parent" field, of type
#                         given in "data/task_type".
#                               Function: add ( inp )
#
#    "del_job"          : deletes job, specified in "data/job" field,
#                         together with all children
#                               Function: add ( inp )
#
#
#  Output is, in all cases, the JSON description of project tree,
#  printed in stdout, e.g.,:
#
#  {
#    "action"     : action-key,
#    "result"     : result-key,
#    "message"    : error-desc,
#    "current_job": current-job-id,
#    "job_count"  : job-count,
#    "jobs"       : [  // tree of Job classes, cf. job.py
#       { "name": name1, "desc": desc1,
#         "task": type1, "id": id1,
#         "jobs: []
#       },
#       { "name": name2, "desc": desc2,
#         "task": type2, "id": id2,
#         "jobs": [
#             { "name": name21, "desc": desc21,
#               "task": type21, "id": id21,
#               "jobs: []
#             },
#             { "name": name22, "desc": desc22,
#               "task": type22, "id": id22,
#               "jobs": [
#                   { "name": name221, "desc": desc221,
#                     "task": type221, "id": id221,
#                     "jobs: []
#                   }
#                ]
#             }
#          ]
#       },
#       { "name": name3, "desc": desc3,
#         "task": type3, "id": id3,
#         "jobs: []
#       }
#    ],
#  }
#
#  where 'nameI' is name of Ith project, and value of "current" gives
#  name of currently selected project.
#
#  ------------------------------------------------------------------
#

import os
import shutil
from varut   import jsonut, gitut, utils, defs
from project import job, tree

def get_list(inp):

    if inp.action != "get_list_of_jobs":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    repo_dir = utils.get_project_repo_path ( defs.master_path(),inp.login,
                                             inp.project )

    if not os.path.isdir(repo_dir):
        return utils.make_return ( inp.action,"repo_does_not_exist",
                                 "Project repository '" + inp.project + \
                                 "' does not exist" )

    result = gitut.lock ( repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    project_data = tree.Tree()
    result = project_data.read ( repo_dir )
    gitut.unlock(repo_dir)

    return utils.add_return ( inp.action,result,project_data )



def __find ( jobs,id ):

    for j in jobs:
        if j.id == id:
            result = jsonut.jObject();
            result.j = j      # found job
            result.v = jobs   # parent's job vector containing the job
            return result
        elif len(j.jobs) > 0:
            result = __find ( j.jobs,id )
            if result:
                return result

    return None


def add(inp):

    if inp.action != "add_job":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    project_repo_dir = utils.get_project_repo_path (
                             defs.master_path(),inp.login,inp.project )

    if not os.path.isdir(project_repo_dir):
        return utils.make_return ( inp.action,"repo_does_not_exist",
                                 "Project repository '" + inp.project + \
                                 "' does not exist" )

    result = gitut.lock ( project_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    project_data = tree.Tree()
    result = project_data.read ( project_repo_dir )

    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        return utils.add_return ( inp.action,result,project_data )

    j = __find ( project_data.jobs,inp.data.parent )

    if not j:
        gitut.unlock ( project_repo_dir )
        return utils.make_return ( inp.action,"wrong_job_specs",
                                              "Wrong job id" )

    project_data.job_count  += 1
    job_data = job.Job();
    job_data.set_minimal_data ( "","",
                                inp.data.task_type,
                                inp.data.parent,
                                project_data.job_count )
    j.j.jobs.append ( job_data );

    project_data.current_job = job_data.id

    try:
        job_dir = utils.get_job_dir_path ( defs.master_path(),inp.login,
                                           inp.project,job_data.id )
        os.mkdir ( job_dir )
    except OSError,err:
        pass
    job_data.write ( project_repo_dir );

    project_data.write ( project_repo_dir )

    tsk = __import__("tasks."+job_data.type[5:] )
    task = getattr(tsk,job_data.type[5:]).Task()
    task.write_arguments ( job_dir )

    result = gitut.commit ( project_repo_dir,["."],
        "add job " + str(job_data.id) + \
        " to job " + str(inp.data.parent) )

    project_data.action = inp.action
    project_data.result = result.result
    if result.result == "OK":
        project_data.message = "Job added successfully"
    else:
        project_data.message = result.message

    return project_data




def __delete_job_dirs ( job,login,project ):

    shutil.rmtree ( utils.get_job_dir_path (
                        defs.master_path(),login,project,job.id ),
                            ignore_errors=True )

    for j in job.jobs:
        __delete_job_dirs ( j,login,project )

    return



def delete(inp):

    if inp.action != "del_job":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    project_repo_dir = utils.get_project_repo_path (
                             defs.master_path(),inp.login,inp.project )

    if not os.path.isdir(project_repo_dir):
        return utils.make_return ( inp.action,"repo_does_not_exist",
                                 "Project repository '" + inp.project + \
                                 "' does not exist" )

    result = gitut.lock ( project_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    project_data = tree.Tree()
    result = project_data.read ( project_repo_dir )

    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        return utils.add_return ( inp.action,result,project_data )

    j = __find ( project_data.jobs,inp.data.job )

    if not j:
        gitut.unlock ( project_repo_dir )
        return utils.make_return ( inp.action,"wrong_job_specs",
                                              "Wrong job id" )

    __delete_job_dirs ( j.j,inp.login,inp.project )
    j.v.remove ( j.j )

    project_data.current_job = inp.data.next
    project_data.write ( project_repo_dir )

    result = gitut.commit ( project_repo_dir,["."],
                            "delete job " + str(inp.data.job)  )

    project_data.action = inp.action
    project_data.result = result.result
    if result.result == "OK":
        project_data.message = "Job added successfully"
    else:
        project_data.message = result.message

    return project_data



def set_job_data ( project_repo_dir,job_data ):

    project_data = tree.Tree()
    result = project_data.read ( project_repo_dir )

    if result.result != "OK":
        return utils.add_return ( "action",result,project_data )

    j = __find ( project_data.jobs,job_data.id )

    if not j:
        return utils.make_return ( "action","wrong_job_specs",
                                            "Wrong job id" )

    j.v[j.v.index(j.j)] = job_data
    project_data.write ( project_repo_dir )
    return utils.make_return ( "action","OK","OK" )


#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#
#
#if __name__ == "__main__":
#    import sys
#
#    inp = jsonut.jObject (
#       '{ "action"      : "get_list_of_projects",'
#       '  "master_path" : "../cofe-master/",'
#       '  "login"       : "eugene"'
#       '}' )
#
#    print inp.action
#    print inp.master_path
#    print inp.login
#
#    defs.set_master_path ( defs.test_master_path() )
#
#    result = get_list ( inp )
#    print ( result.to_JSON() )
#
#    sys.exit(0)
