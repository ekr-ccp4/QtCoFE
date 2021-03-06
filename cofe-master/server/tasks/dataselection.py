#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data selection task
#  ------------------------------------------------------------------

#  Functions:  dataselection.Task.run ( inp )
#
#  Input 'inp' is JSON object:
#
#  {
#    "action"      : action-key, // "set_data"
#    "login"       : user-login,
#    "pwd"         : password,  (md5)
#    "projects_path" : projects-path,
#    "project"     : project-id,
#    "data"        : {
#       "job_id     : job-id
#       "data_list" : [
#           { "type" : data-type,
#             "selection" : [
#               { "jobId": jobId,  // data owner
#                 "outNo": outNo,  // data type position in owner's output array
#                 "setNo": setNo   // data position in data array
#               },
#               ....................
#             ]
#           },
#           .....................
#       ]
#    }
#  }
#
#  Output is JSON object with job data
#
#  {
#    "action"    : action-key,
#    "result"    : result-key,
#    "message"   : error-desc,
#    "job"       : {
#        "data"  : [data1,data2,...]
#        ... other job data
#    }
#  }
#
#  where 'dataN' are objects made by dtypes.*.DType() functions
#

import os
import shutil
from project import task,  job, jobs
from varut   import gitut, jsonut, utils, defs

class Task(task.Task):

    def __init__(self):

        self.type    = "task_dataselection"  # must be "task_" + filename
        self.name    = "Data Selection"
        self.desc    = "Data Selection"
        self.section = -1  # does not appear in task dialog
        self.order   = -2  # position within section for GUI
        self.icon    = "task_dataselection.png"

        self.inp_data = []
        self.out_data = []

        self.executable = ""      # program to run
        self.arguments  = []      # list of arguments

        return


    def run(self,inp):   # This function substitutes the data selection
                         # job and is specific to this task.
                         # This task is interactive; other jobs are run
                         # in the background. Another exception is
                         # Data Import.

        if inp.action != "set_data":
            return utils.make_return ( inp.action,
                          "wrong_action_code","Wrong action code" )

        #  Get project repository
        project_repo_dir = utils.get_project_repo_path (
                             defs.projects_path(),inp.login,inp.project )

        if not os.path.isdir(project_repo_dir):
            return utils.make_return ( inp.action,"repo_does_not_exist",
                             "Project repository '" + inp.project + \
                             "' does not exist" )

        # Lock project repository
        result = gitut.lock ( project_repo_dir )
        if result.result != "OK":
            return utils.pass_return ( inp.action,result )

        # Read (data selection) job metadata
        job_data = job.Job()
        job_data.read ( project_repo_dir,inp.data.job_id )
        if hasattr(job_data,"result"):
            gitut.unlock ( project_repo_dir )
            return pass_return ( inp.action,job_data )

        # Loop over input data
        data_list = inp.data.data_list
        for i in range(len(data_list)):
            selection = data_list[i].selection
            for j in range(len(selection)):
                jdata = job.Job()
                jdata.read ( project_repo_dir,selection[j].jobId )
                if hasattr(jdata,"result"):
                    gitut.unlock ( project_repo_dir )
                    return jdata.result
                outNo = selection[j].outNo
                if outNo >= len(jdata.data):
                    return utils.make_return ( inp.action,
                                "wrong_data_index","Wrong data index" )
                if jdata.data[outNo][0].type != data_list[i].type:
                    return utils.make_return ( inp.action,
                            "wrong_data_type","Wrong data type" )
                setNo = selection[j].setNo
                if setNo >= len(jdata.data[outNo]):
                    return utils.make_return ( inp.action,
                                "wrong_set_number","Wrong set number" )
                job_data.add_data ( jdata.data[outNo][setNo] )


        # Write job data to project repository
        job_data.status = defs.job_done()
        job_data.write ( project_repo_dir )

        # Write job data to project tree
        result = jobs.set_job_data ( project_repo_dir,job_data )

        # Unlock repository
        gitut.unlock   ( project_repo_dir )

        result.action = inp.action
        result.job    = job_data

        return result



def run(inp):
    T = Task()
    return T.run ( inp )

#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    T = Task()

    print "\nData selection task:\n\n" + T.to_JSON()

    sys.exit(0)
