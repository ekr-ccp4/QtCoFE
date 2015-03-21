#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data import task
#  ------------------------------------------------------------------

#  Functions:  data_import.Task.run ( inp )
#
#  Input 'inp' is JSON object:
#
#  {
#    "action"      : action-key, // "import_file"
#    "login"       : user-login,
#    "pwd"         : password,  (md5)
#    "master_path" : master-path,
#    "project"     : project-id,
#    "data"        :  {
#        "file_path" : file-path,
#        "job_id   : job-id
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
from dtypes  import dummy, any, sequence, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_dataimport"   # must be "task_" + filename
        self.name    = "Data import"
        self.desc    = "Data import"
        self.section = task.section_data().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_dataimport.png"

        self.inp_data = []
        self.out_data = []

        self.executable = ""      # program to run
        self.arguments  = []      # list of arguments

        return


    def run(self,inp):   # This function substitutes the data import
                         # job and is specific to data import task.
                         # This task is interactive and takes no data
                         # from its parent job; other jobs use data
                         # stored in Job classes and are run in the
                         # background. Another exception is
                         # Disambiguator.

        if inp.action != "import_file":
            return utils.make_return ( inp.action,
                          "wrong_action_code","Wrong action code" )

        file_ext = os.path.splitext(inp.data.file_path)[-1].lower()
        if not file_ext in [".seq",".mtz"]:
            return utils.make_return ( inp.action,
                          "unknown_file_type","Unknown File Type" )

        project_repo_dir = utils.get_project_repo_path (
                             defs.master_path(),inp.login,inp.project )

        if not os.path.isdir(project_repo_dir):
            return utils.make_return ( inp.action,"repo_does_not_exist",
                             "Project repository '" + inp.project + \
                             "' does not exist" )

        result = gitut.lock ( project_repo_dir )
        if result.result != "OK":
            return utils.pass_return ( inp.action,result )

        job_data = job.Job();
        job_data.read ( project_repo_dir,inp.data.job_id )
        if hasattr(job_data,"result"):
            gitut.unlock ( project_repo_dir )
            return pass_return ( inp.action,job_data )

        job_dir = utils.get_job_dir_path ( defs.master_path(),inp.login,
                                           inp.project,inp.data.job_id )

        try:
            shutil.copy ( inp.data.file_path,job_dir )
        except IOError,err:
            gitut.unlock ( project_repo_dir )
            result = utils.make_return ( inp.action,"file_copy_error",
                                             "File copy error: " + err )
            result.job = job_data
            return utils.pass_return ( inp.action,job_data )

        if file_ext == ".seq":
            seq      = sequence.DType(inp.data.job_id)
            seq.file = os.path.basename ( inp.data.file_path )
            job_data.set_data ( [seq] )
        elif file_ext == ".mtz":
            hkl_data      = hkl.DType(inp.data.job_id)
            hkl_data.file = os.path.basename ( inp.data.file_path )
            hkl_data.addColumn ( "F(+)" )
            hkl_data.addColumn ( "F(-)" )
            job_data.add_data ( hkl_data )
            hkl_data      = hkl.DType(inp.data.job_id)
            hkl_data.file = os.path.basename ( inp.data.file_path )
            hkl_data.addColumn ( "F_peak(+)" )
            hkl_data.addColumn ( "F_peak(-)" )
            job_data.add_data ( hkl_data )
            hkl_data      = hkl.DType(inp.data.job_id)
            hkl_data.file = os.path.basename ( inp.data.file_path )
            hkl_data.addColumn ( "F_ref(+)" )
            hkl_data.addColumn ( "F_ref(-)" )
            job_data.add_data ( hkl_data )

        job_data.write ( project_repo_dir )

        result = jobs.set_job_data ( project_repo_dir,job_data )

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

    print "\nData import task:\n\n" + T.to_JSON()

    sys.exit(0)
