#!/usr/bin/python

#  ------------------------------------------------------------------
#   Molecular Replacement task
#  ------------------------------------------------------------------
#

from project import task
from dtypes  import sequence, structure, xyz, hkl

class Task(task.Task):

    def __init__(self):

        self.type    = "task_mr"
        self.name    = "MR"
        self.desc    = "Molecular Replacement"
        self.section = task.section_phasing().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_mr.png"

        self.inp_data = [
                         [sequence.DType().type ,"E",1],
                         [hkl.DType().type      ,"E",1],
                         [xyz.DType().type      ,"E",1]
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

    print "\nMolecular Replacement task:\n\n" + T.to_JSON()

    sys.exit(0)
