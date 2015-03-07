#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import sequence, structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_ep_sad"
        self.name    = "EP-SAD"
        self.desc    = "Experimental phasing"
        self.section = task.section_phasing().id
        self.order   = 1  # position within section for GUI
        self.icon    = "task_ep.png"

        self.inp_data = [
                         [sequence.DType(-1).type ,"E",1],
                         [hkl.DType(-1).type      ,"E",1]
                        ]
        self.out_data = [
                         [structure.DType(-1).type,"G",0]
                        ]

        return

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


