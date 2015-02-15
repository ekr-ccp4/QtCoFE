#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import sequence, structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_ep_mad"
        self.name    = "EP-MAD"
        self.desc    = "MAD phasing"
        self.section = task.section_phasing().id
        self.order   = 1  # position within section for GUI
        self.icon    = "task_ep.png"

        self.inp_data = [
                         [sequence.DType().type ,"E",1],
                         [hkl.DType().type      ,"G",2]
                        ]
        self.out_data = [
                         [structure.DType().type,"G",0]
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


