#!/usr/bin/python

#  ------------------------------------------------------------------
#   EP phasing task
#  ------------------------------------------------------------------
#

import time
import os
from   project import task
from   dtypes  import sequence, structure, hkl
from   varut   import utils

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


    def run(self,projected_data,job_data,job_dir,file_stdout,file_stderr):

        file_stdout.write ( " This is Auto-MAD Dummy\n" )

        file_stdout.write ( " projected_data.length = " + str(len(projected_data)) + "\n\n" )

        seq_data = task.select_projected_data ( projected_data,
                                                sequence.type(),"" );
        hkl_data = task.select_projected_data ( projected_data,
                                   hkl.type(),hkl.subtypeAnomalous() );

        if not seq_data:
            file_stdout.write ( " NO SEQUENCE DATA SELECTED!\n\n" )
        else:
            file_stdout.write ( " SEQUENCE (" + seq_data[0].files[0]+"):\n" )
            file_stdout.write ( "===============================================================\n" )
            seq_file= open ( utils.get_data_file(job_dir,seq_data[0].jobId,seq_data[0].files[0]),"r" )
            file_stdout.write ( seq_file.read() + "\n" )
            file_stdout.write ( "===============================================================\n" )
            file_stdout.write ( "\n" )
            seq_file.close()

        if not hkl_data:
            file_stdout.write ( " NO REFLECTION DATA SELECTED!\n\n" )
        else:
            file_stdout.write ( " REFLECTION DATA:\n" )
            file_stdout.write ( "===============================================================\n" )
            for ds in hkl_data:
                file_stdout.write ( ds.files[0] + "  " + ds.getDataSetName() )
                meanCol = ds.getMeanColumns()
                if meanCol[2] != "X":
                    file_stdout.write ( " mean:" + meanCol[0] )
                if ds.subtype == hkl.subtypeAnomalous():
                    file_stdout.write ( "  anomalous signal"    )
                else:
                    file_stdout.write ( "  no anomalous signal" )
                anomCol = ds.getAnomalousColumns()
                if anomCol[4] != "X":
                    file_stdout.write ( " " + anomCol[0] + ":" + anomCol[2] )
                file_stdout.write ( "\n" )
            file_stdout.write ( "===============================================================\n" )


        time.sleep ( 5 )
        for i in range(1,4):
            file_out = open ( "structure_" + str(i) + ".mtz",'w' )
            file_out.write ( "Rubbish" )
            file_out.close()
            file_out = open ( "structure_" + str(i) + ".pdb",'w' )
            file_out.write ( "Rubbish" )
            file_out.close()

        return


    def make_output_data ( self,job_dir,job_data ):
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
                structure_data = structure.DType(job_data.id)
                structure_data.addFile ( fname_xyz )
                structure_data.addFile ( fname_map )
                structure_data.addFile ( fname_map )
                structure_data.makeDName ( n )
                dout.append ( structure_data )
                n = n + 1
            else:
                n = 0

        job_data.data = [dout]

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


