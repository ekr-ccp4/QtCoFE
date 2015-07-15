#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

import os
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
                         [sequence.DType(-1).type ,"E",1,"*"],
                         [hkl.DType(-1).type      ,"G",1,hkl.subtypeAnomalous()]
                        ]
        self.out_data = [
                         [structure.DType(-1).type,"G",0,"*"]
                        ]

        self.executable = "mad"   # program to run
        self.arguments  = []      # list of arguments

        return


    def make_output_data ( self,job_dir,job_id ):
        #  This function should return data array for the corresponding
        # Job class with references to task output data in job_dir
        # after the corresponding job completes

        dout = []

        n    = 1
        while n > 0:
            fname_map = "structure_" + str(n) + ".mtz"
            fname_xyz = "structure_" + str(n) + ".mtz"
            if os.path.isfile(os.path.join(job_dir,fname_map)) and \
               os.path.isfile(os.path.join(job_dir,fname_xyz)):
                structure_data = structure.DType(job_id)
                structure_data.addFile ( fname_xyz,[] )
                structure_data.addFile ( fname_map,["FWT","PHWT"] )
                structure_data.addFile ( fname_map,["DELFWT","PHDELWT"] )
                structure_data.makeDName ( n )
                dout.append ( structure_data )
                n = n + 1
            else:
                n = 0

        return [dout]

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


