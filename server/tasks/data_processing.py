#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data processing task
#  ------------------------------------------------------------------

from project import task
from dtypes  import images, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_hkl"
        self.name    = "Data processing"
        self.desc    = "Data processing"
        self.section = task.section_data_proc().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_hkl.png"

        self.inp_data = [
                         [images.DType(-1).type,"G",0]
                        ]
        self.out_data = [
                         [hkl   .DType(-1).type,"E",1]
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

    print "\nData processing task:\n\n" + T.to_JSON()

    sys.exit(0)


