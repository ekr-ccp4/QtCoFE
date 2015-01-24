#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_ep"
        self.name    = "EP"
        self.desc    = "Experimental phasing"
        self.section = task.section_phasing().id
        self.order   = 1  # position within section for GUI
        self.icon    = "task_ep.png"

        self.input_dtypes  = [hkl.DType().type]
        self.output_dtypes = [structure.DType().type]

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


