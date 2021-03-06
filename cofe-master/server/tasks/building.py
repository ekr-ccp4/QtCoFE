#!/usr/bin/python

#  ------------------------------------------------------------------
#   Model building task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import sequence, structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_building"     # must be "task_" + filename
        self.name    = "Model building"
        self.desc    = "model building"
        self.section = task.section_building().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_building.png"

        self.inp_data = [
                         [sequence.DType(-1).type ,"E",1,"*"],
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

    print "\nModel building task:\n\n" + T.to_JSON()

    sys.exit(0)


