#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data import task
#  ------------------------------------------------------------------

from project import task
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
