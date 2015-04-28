#!/usr/bin/python

#  ------------------------------------------------------------------
#   Serving job list queries
#  ------------------------------------------------------------------
#
#  Functions:  jobs.get_list ( inp )
#              jobs.add      ( inp )
#              jobs.delete   ( inp )
#              jobs.run      ( inp )
#              jobs.get_report_uri ( inp )
#
#
#  Input 'inp' is JSON string in sys.argv[1]:
#
#  {
#    "action"      : action_key,
#    "login"       : user_login,
#    "pwd"         : password,  (md5)
#    "projects_path" : projects_path,
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
#                               Function: del ( inp )
#
#    "run_job"          : runs job, specified in "data/job" field,
#                         together with all children
#                               Function: run ( inp )
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
import subprocess
from varut   import jsonut, gitut, utils, defs
from project import job, tree, datamodel


def get_list(inp):

    if inp.action != "get_list_of_jobs":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    repo_dir = utils.get_project_repo_path ( defs.projects_path(),inp.login,
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
                             defs.projects_path(),inp.login,inp.project )

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

    task = datamodel.get_task ( job_data.type )

    try:
        job_dir = utils.get_job_dir_path ( defs.projects_path(),inp.login,
                                           inp.project,job_data.id )
        os.mkdir ( job_dir )
        report_dir = os.path.join ( job_dir,defs.report_dir_name() )
        os.mkdir ( report_dir )
        file = open ( os.path.join(report_dir,defs.report_file_name()),"w" )
        file.write ( "<html><body><h1>Job #" + str(job_data.id) + \
                     " " + task.name + "</h1><p>Report not formed yet" + \
                     "</body></html>" )
        file.close ()
    except OSError,err:
        pass
    job_data.write ( project_repo_dir );

    project_data.write ( project_repo_dir )

    task.write_arguments ( job_dir,task.executable,task.arguments )

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
                        defs.projects_path(),login,project,job.id ),
                            ignore_errors=True )

    for j in job.jobs:
        __delete_job_dirs ( j,login,project )

    return



def delete(inp):

    if inp.action != "del_job":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    project_repo_dir = utils.get_project_repo_path (
                             defs.projects_path(),inp.login,inp.project )

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



def run(inp):

    if inp.action != "run_job":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    project_repo_dir = utils.get_project_repo_path (
                             defs.projects_path(),inp.login,inp.project )

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
        return utils.pass_return ( inp.action,result )

    job_data = job.Job();
    result = job_data.read ( project_repo_dir,inp.data.job )
    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        return utils.pass_return ( inp.action,result )

    #  Set job status to "starting"
    job_data.status = defs.job_starting()  # running
    job_data.write ( project_repo_dir )

    #  Update project data accordingly
    j = __find ( project_data.jobs,job_data.id )

    if not j:
        gitut.unlock ( project_repo_dir )
        return utils.make_return ( "action","wrong_job_specs",
                                            "Wrong job id" )

    j.v[j.v.index(j.j)] = job_data
    project_data.current_job = inp.data.job
    project_data.write ( project_repo_dir )

    result = gitut.commit ( project_repo_dir,["."],
                            "starting job " + str(inp.data.job)  )

    project_data.action = inp.action
    project_data.result = result.result
    if result.result == "OK":
        project_data.message = "Job added successfully"
    else:
        project_data.message = result.message

    proc_path =  os.path.join ( os.path.dirname (
                    os.path.abspath(__file__ + "/../")),"process.py" )

    cmd = ["python",proc_path,project_repo_dir,
           defs.master_path(),str(job_data.id)]

    environ = os.environ
    if 'PYTHONPATH' in environ:
        environ['PYTHONPATH'] = defs.lib_path() + "/py2:" + environ['PYTHONPATH']
    else:
        environ['PYTHONPATH'] = defs.lib_path() + "/py2"

    subprocess.Popen ( cmd,
                       env=environ,
                       creationflags=0 )

    return project_data


def get_report_uri ( inp ):

    if inp.action != "get_report_uri":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    result = utils.make_return ( inp.action,"OK","OK" )
    result.report_uri = os.path.join ( utils.get_job_dir_path (
             defs.projects_path(),inp.login,inp.project,inp.data.job ),
             defs.report_dir_name(),defs.report_file_name() )

    return result



#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#
#
if __name__ == "__main__":
    import sys

    inp = jsonut.jObject (
       '{'
       '  "action"      : "run_job",'
       '  "login"       : "eugene",'
       '  "pwd"         : "12345",'
       '  "projects_path" : "../cofe-projects/",'
#       '  "bin_path"    : "../bin-projects/",'
       '  "project"     : "test",'
       '  "data"        :  {'
       '    "job"       : 3,'
       '    "next"      : 3'
       '  }'
       '}'
    )

    defs.set_projects_path ( defs.test_projects_path() )
#    defs.set_bin_path    ( defs.test_bin_path()    )

    result = run ( inp )
    print ( result.to_JSON() )

    sys.exit(0)
