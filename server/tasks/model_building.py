#!/usr/bin/python

#  ------------------------------------------------------------------
#   Model building task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import structure, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_modbuild"
        self.name    = "Model building"
        self.desc    = "model building"
        self.section = task.section_building().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_modbuild.png"

        self.input_dtypes  = [hkl.DType().type,structure.DType().type]
        self.input_dmodes  = ["E1"            ,"E1"                  ]
        self.output_dtypes = [structure.DType().type]
        self.output_dmodes = ["E1"                  ]

        return

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


