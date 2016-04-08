#!/usr/bin/python

#  ------------------------------------------------------------------
#   Molecular Replacement task
#  ------------------------------------------------------------------
#

import time
import os
from   project import task
from   dtypes  import sequence, structure, xyz, hkl
from   varut   import utils, defs

def molrep_pdb   ():  return "molrep.pdb"
def molrep_script():  return "molrep.script"
def refmac_pdb   ():  return "xyz.pdb"
def refmac_mtz   ():  return "map.mtz"
def refmac_cif   ():  return "refmac.cif"
def refmac_script():  return "refmac.script"

class Task(task.Task):

    def __init__(self):

        self.type    = "task_molrep"       # must be "task_" + filename
        self.name    = "Molrep"
        self.desc    = "Molecular Replacement"
        self.section = task.section_phasing().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_molrep.png"

        self.inp_data = [
                         [hkl.DType(-1).type      ,"E",1,"*"],
                         [xyz.DType(-1).type      ,"E",1,"*"]
                        ]
        self.out_data = [
                         [structure.DType(-1).type,"G",0,"*"]
                        ]

        self.executable = ""      # program to run
        self.arguments  = []      # list of arguments

        return



    def run(self,projected_data,job_data,job_dir,file_stdout,file_stderr):
    
        #  just in case if job is run repeatedly
        if os.path.isfile(molrep_pdb()): 
            os.remove(molrep_pdb())

        import pyrvapi
        from   reports import pyrvapi_logparse_molrep as rvapi_parser

        file_stdout.write ( " This is Molrep Dummy\n" )

        file_stdout.write ( " projected_data.length = " + str(len(projected_data)) + "\n\n" )

        hkl_data = task.select_projected_data ( projected_data,
                                                hkl.type(),"" );
        xyz_data = task.select_projected_data ( projected_data,
                                                xyz.type(),"" );

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

        if not xyz_data:
            file_stdout.write ( " NO XYZ DATA SELECTED!\n\n" )
        else:
            file_stdout.write ( " XYZ (" + xyz_data[0].files[0]+"):\n" )
            file_stdout.write ( "===============================================================\n" )
            xyz_file= open ( utils.get_data_file(job_dir,xyz_data[0].jobId,xyz_data[0].files[0]),"r" )
            file_stdout.write ( xyz_file.read() + "\n" )
            file_stdout.write ( "===============================================================\n" )
            file_stdout.write ( "\n" )
            xyz_file.close()

        rvrow = 0
        pyrvapi.rvapi_add_text (
            "<font size='+1'><b><i>Molecular Replacement with Molrep</i></b></font>",
            defs.report_page_id(),rvrow,0,1,1 )
        pyrvapi.rvapi_add_text ( "&nbsp;",defs.report_page_id(),
            rvrow+1,0,1,1 )
        pyrvapi.rvapi_add_panel ( 'molrep_report',defs.report_page_id(),
                                  rvrow+2,0,1,1 )
        pyrvapi.rvapi_flush()
        rvrow += 3

        scr_file = open ( os.path.join(job_dir,molrep_script()),"w" )
        ds = hkl_data[0]
        hklin = utils.get_data_file(job_dir,ds.jobId,ds.files[0])
        scr_file.write (
            "labin F=" + ds.getMeta("Fmean",None)    + \
            " SIGF="   + ds.getMeta("sigFmean",None) + "\n" + \
            "file_f "  + hklin + "\n" + \
            "file_m "  + utils.get_data_file(job_dir,xyz_data[0].jobId,xyz_data[0].files[0]) + "\n"
        )
        scr_file.close()

        scr_file = open ( os.path.join(job_dir,molrep_script()),"r" )
        file_stdout.write ( scr_file.read() + "\n" )
        file_stdout.write ( "===============================================================\n" )
        file_stdout.write ( "\n" )
        scr_file.close()

        log_parser = rvapi_parser.pyrvapi_logparse_molrep ( 'molrep_report' )

        scr_file = open ( os.path.join(job_dir,molrep_script()),"r" )
        msg = self.call ( "molrep",["-i"],job_dir,scr_file,
                         file_stdout,file_stderr,log_parser=log_parser )
        scr_file.close()
        
        if os.path.isfile(molrep_pdb()):
            # there's a chance for solution, run refmac
    
            pyrvapi.rvapi_add_text (
                "<font size='+1'><b><i>Calculate Map with Refmac</i></b></font>",
                defs.report_page_id(),rvrow,0,1,1 )
            pyrvapi.rvapi_add_text ( "&nbsp;",defs.report_page_id(),
                rvrow+1,0,1,1 )
            pyrvapi.rvapi_flush()
            rvrow += 2


            scr_file = open ( os.path.join(job_dir,refmac_script()),"w" )
            ds = hkl_data[0]
            scr_file.write (            
               "make check NONE\n" +
               "make -\n" +
               "    hydrogen ALL -\n" +
               "    hout NO -\n" +
               "    peptide NO -\n" +
               "    cispeptide YES -\n" +
               "    ssbridge YES -\n" +
               "    symmetry YES -\n" +
               "    sugar YES -\n" +
               "    connectivity NO -\n" +
               "    link NO\n" +
               "refi -\n" +
               "    type REST -\n" +
               "    resi MLKF -\n" +
               "    meth CGMAT -\n" +
               "    bref ISOT\n" +
               "ncyc 0\n" +
               "scal -\n" +
               "    type SIMP -\n" +
               "    LSSC -\n" +
               "    ANISO -\n" +
               "    EXPE\n" +
               "solvent YES\n" +
               "weight -\n" +
               "    AUTO\n" +
               "monitor MEDIUM -\n" +
               "    torsion 10.0 -\n" +
               "    distance 10.0 -\n" +
               "    angle 10.0 -\n" +
               "    plane 10.0 -\n" +
               "    chiral 10.0 -\n" +
               "    bfactor 10.0 -\n" +
               "    bsphere 10.0 -\n" +
               "    rbond 10.0 -\n" +
               "    ncsr 10.0\n" +
               "    labin  FP=" + ds.getMeta("Fmean",None) +
                         " SIGFP=" + ds.getMeta("sigFmean",None ) + " -\n" +
               "       FREE=" + ds.getMeta("free",None ) + "\n" +
               "labout  FC=FC FWT=FWT PHIC=PHIC PHWT=PHWT DELFWT=DELFWT PHDELWT=PHDELWT FOM=FOM\n" +
               "PNAME CoFE\n" +
               "DNAME\n" +
               "RSIZE 80\n" +
               "EXTERNAL WEIGHT SCALE 10.0\n" +
               "EXTERNAL USE MAIN\n" +
               "EXTERNAL DMAX 4.2\n" +
               "END\n"
            )
            scr_file.close()
    
            scr_file = open ( os.path.join(job_dir,refmac_script()),"r" )
            file_stdout.write ( "\n\n===============================================================\n" )
            file_stdout.write ( scr_file.read() + "\n" )
            file_stdout.write ( "===============================================================\n" )
            file_stdout.write ( "\n" )
            scr_file.close()

            xyz_file = self.make_job_filename ( job_data,refmac_pdb() )
            map_file = self.make_job_filename ( job_data,refmac_mtz() )

            scr_file = open ( os.path.join(job_dir,refmac_script()),"r" )
            msg = self.call ( "refmac5",
                              ["XYZIN" ,os.path.join(job_dir,molrep_pdb()),
                               "XYZOUT",os.path.join(job_dir,xyz_file),
                               "HKLIN" ,hklin,
                               "HKLOUT",os.path.join(job_dir,map_file),
                               "LIBOUT",os.path.join(job_dir,refmac_cif()),
                               ],
                              job_dir,scr_file,file_stdout,file_stderr )
            scr_file.close()
            
            pyrvapi.rvapi_add_data ( "structure_btn",
                          "Structure and electron density",
                          "../" + xyz_file,  # always relative to job_dir
                          "xyz",defs.report_page_id(),rvrow,0,1,1,1 );
            pyrvapi.rvapi_append_to_data ( "structure_btn",
                          "../" + map_file,  # always relative to job_dir
                          "hkl:map" );
            pyrvapi.rvapi_flush();

        return


    def make_output_data ( self,job_dir,job_data ):
        #  This function should return data array for the corresponding
        # Job class with references to task output data in job_dir
        # after the corresponding job completes
        
        dout = []

        xyz_file = self.make_job_filename ( job_data,refmac_pdb() )
        map_file = self.make_job_filename ( job_data,refmac_mtz() )
        if os.path.isfile(os.path.join(job_dir,map_file)) and \
           os.path.isfile(os.path.join(job_dir,xyz_file)):
            structure_data = structure.DType(job_data.id)
            structure_data.addFile ( xyz_file )
            structure_data.addFile ( map_file )
            structure_data.setMeta ( "Fmap11" ,"DELFWT"   )
            structure_data.addMeta ( "PHmap11","PHDELFWT" )
            structure_data.addMeta ( "Fmap21" ,"FWT"      )
            structure_data.addMeta ( "PHmap21","PHFWT"    )
            structure_data.addMeta ( "FC"     ,"FC"       )
            structure_data.addMeta ( "FOM"    ,"FOM"      )
            structure_data.addMeta ( "PHIC"   ,"PHIC"     )
            structure_data.makeDName ( 1 )
            dout.append ( structure_data )

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

    print "\nMolecular Replacement task:\n\n" + T.to_JSON()

    sys.exit(0)
