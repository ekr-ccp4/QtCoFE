#!/usr/bin/python

#  ------------------------------------------------------------------
#   Refinement task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_refinement"   # must be "task_" + filename
        self.name    = "Refinement"
        self.desc    = "Refinement"
        self.section = task.section_refinement().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_refinement.png"

        self.inp_data = [
                         [hkl.DType(-1).type      ,"E",1,"*"],
                         [structure.DType(-1).type,"E",1,"*"]
                        ]
        self.out_data = [
                         [structure.DType(-1).type,"E",1,"*"]
                        ]

        self.executable = ""      # program to run
        self.arguments  = []      # list of arguments

        return


    def make_output_data ( self,job_dir,job_id ):
        #  This function should return data array for the corresponding
        # Job class with references to task output data in job_dir
        # after the corresponding job completes
        return [[any.DType()]]

#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    T = Task()

    print "\nExperimental phasing task:\n\n" + T.to_JSON()

    sys.exit(0)
