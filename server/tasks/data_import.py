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
from project import task
from varut   import gitut, jsonut, utils
from dtypes  import dummy, any

class Task(task.Task):

    def __init__(self):

        self.type    = "task_import"
        self.name    = "Data import"
        self.desc    = "Data import"
        self.section = task.section_data().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_import.png"

        self.input_dtypes  = [dummy.DType().type]
        self.output_dtypes = [any  .DType().type]

        return


    def run(self,inp):

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

        job_data = utils.get_job_data ( project_repo_dir,inp.data.job_id )

        if hasattr(job_data,"result"):
            gitut.unlock ( project_repo_dir )
            return pass_return ( inp.action,job_data )

        job_dir = utils.get_job_dir_path ( defs.master_path(),inp.login,
                                           inp.project,inp.data.job_id )

        try:
            shutil.copy ( inp.data.file_path,job_dir )
        except IOError,err:
            gitut.unlock ( project_repo_dir )
            return utils.make_return ( inp.action,"file_copy_error",
                                       "File copy error: " + err )

#        if file_ext == ".seq":

        return utils.make_return ( inp.action,"OK","OK" )

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
