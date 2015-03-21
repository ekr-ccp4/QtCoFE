#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import sequence, structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_mad"     # must be "task_" + filename
        self.name    = "EP-MAD"
        self.desc    = "MAD phasing"
        self.section = task.section_phasing().id
        self.order   = 2  # position within section for GUI
        self.icon    = "task_mad.png"

        self.inp_data = [
                         [sequence.DType(-1).type ,"E",1],
                         [hkl.DType(-1).type      ,"G",1]
                        ]
        self.out_data = [
                         [structure.DType(-1).type,"G",0]
                        ]

        self.executable = ""      # program to run
        self.arguments  = []      # list of arguments

        return


    def make_output_data ( job_dir ):
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


