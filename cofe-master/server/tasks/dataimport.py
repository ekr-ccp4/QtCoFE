#!/usr/bin/python

#  ------------------------------------------------------------------
#  Data import task
#  ------------------------------------------------------------------

#  Functions:  data_import.Task.run ( inp )
#
#  Input 'inp' is JSON object:
#
#  {
#    "action"      : action-key, // "import_file"
#    "login"       : user-login,
#    "pwd"         : password,  (md5)
#    "projects_path" : projects-path,
#    "project"     : project-id,
#    "data"        :  {
#        "file_path" : file-path,
#        "job_id   : job-id
#    }
#  }
#
#  Output is JSON object with job data
#
#  {
#    "action"    : action-key,
#    "result"    : result-key,
#    "message"   : error-desc,
#    "job"       : {
#        "data"  : [data1,data2,...]
#        ... other job data
#    }
#  }
#
#  where 'dataN' are objects made by dtypes.*.DType() functions
#

import os
import shutil
from   project import task,  job,    jobs
from   varut   import gitut, jsonut, utils,    defs, mtz
from   dtypes  import dummy, any,    sequence, hkl,  xyz

class Task(task.Task):

    def __init__(self):

        self.type    = "task_dataimport"   # must be "task_" + filename
        self.name    = "Data import"
        self.desc    = "Data import"
        self.section = task.section_data().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_dataimport.png"

        self.inp_data = []
        self.out_data = []

        self.executable = "dataimport"  # script to run
        self.arguments  = []            # list of arguments

        return


    def import_file ( self,inp ):
                         # This function makes 1st stage of data import
                         # job and is specific to data import task.
                         # This task is interactive and takes no data
                         # from its parent job; other jobs use data
                         # stored in Job classes and are run in the
                         # background.

        if inp.action != "import_file":
            return utils.make_return ( inp.action,
                          "wrong_action_code","Wrong action code" )

        file_ext = os.path.splitext(inp.data.file_path)[-1].lower()
        if not file_ext in [".seq",".mtz",".pdb",".ent",".cif"]:
            return utils.make_return ( inp.action,
                          "unknown_file_type","Unknown File Type" )

        project_repo_dir = utils.get_project_repo_path (
                             defs.projects_path(),inp.login,inp.project )

        if not os.path.isdir(project_repo_dir):
            return utils.make_return ( inp.action,"repo_does_not_exist",
                             "Project repository '" + inp.project + \
                             "' does not exist" )

        result = gitut.lock ( project_repo_dir )
        if result.result != "OK":
            return utils.pass_return ( inp.action,result )

        job_data = job.Job()
        job_data.read ( project_repo_dir,inp.data.job_id )
        if hasattr(job_data,"result"):
            gitut.unlock ( project_repo_dir )
            return pass_return ( inp.action,job_data )

        job_dir = utils.get_job_dir_path ( defs.projects_path(),inp.login,
                                           inp.project,inp.data.job_id )

        try:
            shutil.copy ( inp.data.file_path,job_dir )
        except IOError,err:
            gitut.unlock ( project_repo_dir )
            result = utils.make_return ( inp.action,"file_copy_error",
                                             "File copy error: " + err )
            result.job = job_data
            return utils.pass_return ( inp.action,job_data )

#        datacnt = 1;  # counts all data sets for unique numbering
#        for i in range(0,len(job_data.input)):
#            datacnt += len(job_data.input[i])

        if file_ext == ".seq":
            seq = sequence.DType(inp.data.job_id)
            seq.setFile ( os.path.basename(inp.data.file_path) )
            seq.makeDName      ( -1  )
            job_data.add_input ( seq )

        elif file_ext == ".pdb" or file_ext == ".ent" or file_ext == ".cif":
            coors = xyz.DType(inp.data.job_id)
            coors.setFile ( os.path.basename(inp.data.file_path) )
            coors.makeDName    ( -1    )
            job_data.add_input ( coors )
 #           datacnt += 1

        elif file_ext == ".mtz":
            mf  = mtz.mtz_file(inp.data.file_path)
            for ds in mf:
                ds.MTZ = os.path.basename(inp.data.file_path)
                hkl_data = hkl.DType(inp.data.job_id)
                hkl_data.importMTZDataset ( ds )
                hkl_data.makeDName  ( -1       )
                job_data.add_input  ( hkl_data )
#                datacnt += 1

        job_data.status = defs.job_idle()
        job_data.write ( project_repo_dir )

        result = jobs.set_job_data ( project_repo_dir,job_data )

        gitut.unlock   ( project_repo_dir )

        result.action = inp.action
        result.job    = job_data

        return result



    def run ( self,projected_data,job_data,job_dir,file_stdout,file_stderr ):
    #  This function process imported data, stored in 'input' array,
    #  and places them in the output array 'data'.

#        import subprocess
        import pyrvapi

        def addTable ( tableId,holderId,row ):
            pyrvapi.rvapi_add_table ( tableId,"",holderId,row,0,1,1, 100 )
            pyrvapi.rvapi_set_table_style ( tableId,
                                       "table-blue","text-align:left;" )
            return


        def addTableLine ( tableId,header,tooltip,line,row ):
            pyrvapi.rvapi_put_vert_theader ( tableId,header,tooltip,row )
            pyrvapi.rvapi_put_table_string ( tableId,line,row,0 )
            pyrvapi.rvapi_shape_table_cell ( tableId,row,0,"",
                "text-align:left;width:100%;" + \
                "font-family:\"Courier\";text-decoration:none;" + \
                "font-weight:normal;font-style:normal;",
                "",1,1 );
            return row+1


        def makeHKLTable ( tableId,holderId,original_data,new_data,
                           truncation,trunc_msg,row ):
            addTable     ( tableId,holderId,row )
            r = addTableLine ( tableId,"File name",
                           "Imported file name",new_data.files[0],0 )
            r = addTableLine ( tableId,"Dataset name",
                           "Original dataset name",
                           new_data.getDataSetName(),r )
            r = addTableLine ( tableId,"Assigned name",
                           "Assigned data name",new_data.dname,r )
            r = addTableLine ( tableId,"Wavelength","Wavelength",
                           new_data.getMeta("wavelength","unspecified"),r )
            r = addTableLine ( tableId,"Space group","Space group",
                           new_data.getMeta("hm","unspecified"),r )

            cell_spec = new_data.getMeta ( "cell_a","*" )     + " " + \
                        new_data.getMeta ( "cell_b","*" )     + " " + \
                        new_data.getMeta ( "cell_c","*" )      + \
                        "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + \
                        new_data.getMeta ( "cell_alpha","*" ) + " " + \
                        new_data.getMeta ( "cell_beta" ,"*" ) + " " + \
                        new_data.getMeta ( "cell_gamma","*" )

            r = addTableLine ( tableId,"Cell","Cell parameters",
                                cell_spec,r )

            if new_data.subtype == hkl.subtypeAnomalous():
                anom = "Present"
            else:
                anom = "Not present"
            r = addTableLine ( tableId,"Anomalous scattering",
                               "Presence of anomalous data",anom,r )

            if trunc_msg:
                r = addTableLine ( tableId,"Original columns",
                    "Original data columns",
                    original_data.getColumnNames(),r )
                r = addTableLine ( tableId,"Truncation",
                    "Truncation result","Failed: " + msg + \
                    "<br>The dataset cannot be used",r )
            elif truncation == 0:
                r = addTableLine ( tableId,"Original columns",
                    "Original data columns",
                    original_data.getColumnNames(),r )
                r = addTableLine ( tableId,"Truncation",
                    "Truncation result",
                    "Was not performed due to the absence of " + \
                    "intensity data.<br>" + \
                    "The dataset will be used untruncated",r )
            else:
                r = addTableLine ( tableId,"Original columns",
                    "Original data columns",
                    original_data.getColumnNames(),r )
                r = addTableLine ( tableId,"Truncation",
                    "Truncation result",
                    "Truncated dataset will be used instead of " + \
                    "the original one.",r )
                r = addTableLine ( tableId,"Columns to be used",
                    "Data columns which will be used further on",
                    new_data.getColumnNames(),r )
            return


        # firstly sort and re-dname imported data
        job.sort_data  ( job_data.input )
        job.set_dnames ( job_data.input )

        file_stdout.write ( " DATA IMPORT\n" )

        rvrow = 0
        pyrvapi.rvapi_add_text (
            "<font size='+1'><b><i>DATA IMPORT</i></b></font>",
            defs.report_page_id(),rvrow,0,1,1 )
        pyrvapi.rvapi_add_text ( "&nbsp;",defs.report_page_id(),
            rvrow+1,0,1,1 )
        pyrvapi.rvapi_flush()
        rvrow += 2


        # 'projected_data' is ignored in this task because it deals
        # only with data imported in course of 1st stage (see function
        # 'import_file') and stored in 'input' array. Therefore, we
        # get these data directly from 'job_data' passed into this
        # function by 'process' module. Here, we do not bother with
        # repository lock and commit because th is code should run only
        # from 'process' module, where all this is taken care of.
        # Note that repository is not locked at run time, and
        # interference of concurrent processes is prevented only
        # by checking the 'status' field. If 'status' is neither 'idle'
        # nor 'finished', no new process should be launched.

        job_data.data = []
        for dt in job_data.input:
            for d in dt:

                rvsecId   = "sec_"   + str(rvrow)
                rvtableId = "table_" + str(rvrow)

                if d.type == sequence.type():
                    # simply copy over

                    file_stdout.write ( "\n SEQUENCE (" + d.files[0]+"):\n" )
                    file_stdout.write ( "="*80 + "\n" )

                    pyrvapi.rvapi_add_section ( rvsecId,
                        "Macromolecular sequence",defs.report_page_id(),
                        rvrow,0,1,1,True )

                    addTable     ( rvtableId,rvsecId,rvrow )
                    addTableLine ( rvtableId,"File name",
                                   "Imported file name",d.files[0],0 )
                    addTableLine ( rvtableId,"Assigned name",
                                   "Assigned data name",d.dname,1 )

                    lines = filter ( None,
                        (line.rstrip() for line in open(os.path.join(job_dir,d.files[0]),"r")))

                    htmlLine = ""
                    for i in range(0,len(lines)):
                        file_stdout.write ( lines[i] + "\n" )
                        if i>0:  htmlLine += "<br>"
                        htmlLine += lines[i]

                    addTableLine ( rvtableId,"Contents",
                                   "Data contents",htmlLine,2 )

                    job_data.add_data ( d )


                elif d.type == xyz.type():
                    # simply copy over

                    file_stdout.write ( "\n XYZ COORDINATES (" + d.files[0]+"):\n" )
                    file_stdout.write ( "="*80 + "\n" )

                    pyrvapi.rvapi_add_section ( rvsecId,
                        "Atomic coordinates",defs.report_page_id(),
                        rvrow,0,1,1,True )

                    addTable     ( rvtableId,rvsecId,rvrow )
                    addTableLine ( rvtableId,"File name",
                                   "Imported file name",d.files[0],0 )
                    addTableLine ( rvtableId,"Assigned name",
                                   "Assigned data name",d.dname,1 )

                    """ =======================================================================
                    lines = filter ( None,
                        (line.rstrip() for line in open(os.path.join(job_dir,d.files[0]),"r")))

                    htmlLine = ""
                    for i in range(0,len(lines)):
                        file_stdout.write ( lines[i] + "\n" )
                        if i>0:  htmlLine += "<br>"
                        htmlLine += lines[i]

                    addTableLine ( rvtableId,"Contents",
                                   "Data contents",htmlLine,2 )
                    ======================================================================== """

                    job_data.add_data ( d )


                elif d.type == hkl.type():
                    # process with ctruncate

                    outFileName = d.dataId+".mtz"
                    cmd = ["-hklin",d.files[0],"-hklout",outFileName]
                    amplitudes = ""

                    meanCols = d.getMeanColumns()
                    if meanCols[2] != "X":
                        cols = "/*/*/["
                        if meanCols[1] != None:
                            cols = cols + meanCols[0] + "," + meanCols[1]
                        else:
                            cols = cols + meanCols[0]
                            if meanCols[2] == "F":
                                amplitudes = "-amplitudes"
                        cmd = cmd + ["-colin",cols+"]"]

                    anomCols  = d.getAnomalousColumns()
                    anomalous = False
                    if anomCols[4] != "X":
                        anomalous = True
                        cols = "/*/*/["
                        for i in range(0,4):
                            if anomCols[i] != None:
                                if i > 0: cols = cols + ","
                                cols = cols + anomCols[i]
                                if anomCols[4] == "F":
                                    amplitudes = "-amplitudes"
                        cmd = cmd + ["-colano",cols+"]"]

                    if amplitudes:
                        cmd = cmd + [amplitudes]
                    cmd = cmd + ["-freein"]

                    msg = self.call ( "ctruncate",cmd,job_dir,None,
                                      file_stdout,file_stderr )
#                    file_stdout.write ( " p0 msg='" + msg + "'\n" )
                    file_stdout.flush()

                    pyrvapi.rvapi_add_section ( rvsecId,
                        "Reflection dataset "+d.dname,
                        defs.report_page_id(),rvrow,0,1,1,True )

                    if msg:
                        msg = "\n\n ctruncate failed with message:\n\n" + \
                              msg + \
                              "\n\n Dataset " + d.dname + \
                              " cannot be used.\n\n"
                        file_stdout.write ( msg )
                        file_stderr.write ( msg )
                        makeHKLTable ( rvtableId,rvsecId,d,d,-1,msg,rvrow )

                    elif not os.path.exists(outFileName):
                        file_stdout.write ( "\n\n +++ Dataset " + d.dname + \
                            "\n was not truncated and will be used as is\n\n" )
                        job_data.add_data ( d )
                        makeHKLTable ( rvtableId,rvsecId,d,d,0,"",rvrow )

                    else:
                        file_stdout.write ( "\n\n ... Dataset " + d.dname + \
                            "\n was truncated and will substitute the " + \
                            "original one\n\n" )
                        mf = mtz.mtz_file(outFileName)
                        # ctruncate should create a single dataset here
                        for ds in mf:
                            ds.MTZ = os.path.basename(outFileName)
                            hkl_data = hkl.DType(job_data.id)
                            hkl_data.importMTZDataset ( ds )
                            hkl_data.dataId = d.dataId
                            hkl_data.dname  = d.dname + "/truncated"
                            job_data.add_data  ( hkl_data )
                            makeHKLTable ( rvtableId,rvsecId,d,hkl_data,
                                           1,"",rvrow )


#ctruncate -hklin filename -hklout filename -seqin sequence file name
#          -colin column label -colano column label -colout column label
#          -nres number of residues -no-aniso turn off anisotopy correction
#          -amplitudes use amplitudes
#          -comp if set to nucleic use DNA/RNA reference curve
#          -prior force use of WILSON or FLAT prior
#          -freein retain freeR flag

                else:
                    # temporary copy over
                    job_data.add_data ( d )

                pyrvapi.rvapi_flush()
                rvrow += 1


        # Finally, all job data write and the corresponding update of
        # project data will be done in 'process' module, so we simply
        # quit here

        return

    """ ================================================================================
    
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
            meanCol = ds.getMeanColumn()
            if meanCol[1] != "X":
                file_stdout.write ( " mean:" + meanCol[0] )
            if ds.subtype == hkl.subtypeAnomalous():
                file_stdout.write ( "  anomalous signal"    )
            else:
                file_stdout.write ( "  no anomalous signal" )
            anomCol = ds.getAnomalousColumns()
            if anomCol[2] != "X":
                file_stdout.write ( " " + anomCol[0] + ":" + anomCol[1] )
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
        
    ===============================================================   """


    def get_command ( self,projected_data ):

        cmd = []

        for d in projected_data:
            cmd = cmd + [d[0].files[0]]

        return cmd


    def make_output_data ( self,job_dir,job_data ):
        return


def import_file(inp):
    T = Task()
    return T.import_file ( inp )

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
