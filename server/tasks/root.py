#!/usr/bin/python

#  ------------------------------------------------------------------
#  Project root task
#  ------------------------------------------------------------------

from project import task
from dtypes  import dummy

class Task(task.Task):

    def __init__(self):

        self.type    = "task_root"
        self.name    = "$project_name$"  # to be substituted
        self.desc    = "Project root"
        self.section = ""  # not to show in task list
        self.order   = -1  # position within section for GUI
        self.icon    = "task_root.png"

        self.input_dtypes  = []
        self.input_dmodes  = []
        self.output_dtypes = [dummy.DType().type]
        self.output_dmodes = ["E0"              ]

        return

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
