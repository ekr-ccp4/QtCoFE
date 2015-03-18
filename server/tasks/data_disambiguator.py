#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data disambiguator task
#  ------------------------------------------------------------------

#  Functions:  data_disambiguator.Task.run ( inp )
#
#  Input 'inp' is JSON object:
#
#  {
#    "action"      : action-key, // "disambiguate"
#    "login"       : user-login,
#    "pwd"         : password,  (md5)
#    "master_path" : master-path,
#    "project"     : project-id,
#    "data"        :  {
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

class Task(task.Task):

    def __init__(self):

        self.type    = "task_disambiguator"
        self.name    = "Data select"
        self.desc    = "Data disambiguator"
        self.section = -1  # does not appear in task dialog
        self.order   = -2  # position within section for GUI
        self.icon    = "task_disambiguator.png"

        self.inp_data = []
        self.out_data = []

        return


    def run(self,inp):

        if inp.action != "disambiguate":
            return utils.make_return ( inp.action,
                          "wrong_action_code","Wrong action code" )

        return utils.make_return ( inp.action,"Ok","Ok" )


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

    print "\nData disambiguator task:\n\n" + T.to_JSON()

    sys.exit(0)
