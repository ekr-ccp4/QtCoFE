##!/usr/bin/python

#  ------------------------------------------------------------------
#
#  Process module -- the process running primitive. It is needed mainly
#  to update the job status upon termination of the job. Operations:
#     1. Forms the command
#     2. Sets job status to "running"
#     3. Starts the command and waits for it to finish
#     4. Registers generated files into job's output data
#     5. Sets job status to "finished"
#     6. Returns
#
#  process.py project_repo_dir master_dir jobId
#
#  ------------------------------------------------------------------

import os
import sys
#import subprocess
import pyrvapi
from   varut   import gitut, utils, defs
from   project import job, datamodel, jobs


#  1. Read input parameters

project_repo_dir = sys.argv[1]
defs.set_master_path ( sys.argv[2] )
jobID = int(sys.argv[3])

#  2. Identify job directory

#bin_dir = defs.bin_path()
job_dir = utils.get_job_dir ( project_repo_dir,jobID )


#  set temporary debug log
file_deb = open ( os.path.join(defs.test_projects_path(),"debug/process.log"),'w' )
file_deb.write ( " DEBUG LOG FILE\n" )
if 'PYTHONPATH' in os.environ:
    os.environ['PYTHONPATH'] =  defs.server_path()[:-1] + ":" + os.environ['PYTHONPATH']
    file_deb.write ( "PYTHONPATH = " + os.environ['PYTHONPATH'] + "\n" )
else:
    os.environ['PYTHONPATH'] =  defs.server_path()[:-1]
    file_deb.write ( "PYTHONPATH not found\n" );

file_deb.write ( " python = " + sys.executable + "\n" )

print >>file_deb, sys.path


#  3. Redefine standard streams

file_stdout_path = os.path.join ( job_dir,"_stdout.log" )
file_stderr_path = os.path.join ( job_dir,"_stderr.log" )
file_stdout = open ( file_stdout_path,'w' )
file_stderr = open ( file_stderr_path,'w' )

def close_streams():
    global file_deb,file_stdout,file_stderr
    file_stdout.close()
    file_stderr.close()
    file_deb   .close()
    return

def message ( msg ):
    file_stdout.write ( msg )
    file_deb   .write ( msg )
    return


#  4. Read job description

result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    message ( " *** ERROR: cannot lock repository (1)\n" )
    close_streams()
    sys.exit(defs.process_cant_get_lock_1())

job_data = job.Job();
result   = job_data.read ( project_repo_dir,jobID )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    message ( " *** ERROR: cannot read job description\n" )
    close_streams()
    sys.exit(defs.process_cant_read_job())


#  5. Get task description and run data

projected_data = []
job.get_projected_data    ( projected_data,project_repo_dir,jobID )
task = datamodel.get_task ( job_data.type )
if task == None:
    message ( " *** ERROR: cannot read task description for type '" + \
              job_data.type + "'\n" )
    close_streams()
    sys.exit(defs.process_cant_read_task())
task.read_arguments ( job_dir )


#  6. Initiate report document

file_deb.write ( " rvapi_init_document\n" )

pyrvapi.rvapi_init_document ( "report",os.path.join(job_dir,"html"),
                              job_data.desc,1,12,
                              defs.jsrview_path(),None,None,"task.tsk",
                              "i2.xml" )

pyrvapi.rvapi_add_header ( "<b>Job " + str(job_data.id).zfill(3) +
                           ": " + task.desc + "</b>" );

pyrvapi.rvapi_add_tab        ( defs.report_page_id(),"Report",True      );
pyrvapi.rvapi_add_tab        ( defs.log_page_id(),"Log file" ,False     );
pyrvapi.rvapi_append_content ( file_stdout_path,True,defs.log_page_id() );
pyrvapi.rvapi_add_tab        ( defs.err_page_id(),"Errors"   ,False     );
pyrvapi.rvapi_append_content ( file_stderr_path,True,defs.err_page_id() );
pyrvapi.rvapi_flush();


#  7. Set job status to "running"

job_data.status = defs.job_running()  # running
job_data.write ( project_repo_dir )
result = jobs.set_job_data ( project_repo_dir,job_data )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    message ( " *** ERROR: cannot set run status\n" )
    close_streams()
    sys.exit(defs.process_cant_update_tree_1())

result = gitut.commit ( project_repo_dir,["."],
                        "running job " + str(job_data.id) )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    message ( " *** ERROR: cannot commit to repository before run\n" )
    close_streams()
    sys.exit(defs.process_cant_commit_1())


#  8. Start job and wait to finish

file_deb.write ( " start job\n" )

os.chdir ( job_dir )
if task == None:
    message ( " MODULE " + job_data.type + " NOT FOUND\n" )
else:
    msg = ""
    try:
        task.run ( projected_data,job_data,job_dir,file_stdout,file_stderr )
    except OSError as e:
        msg = "OSError > " + str(e.errno) + "\n" + \
              "OSError > " + e.strerror   + "\n" + \
              "OSError > " + e.filename   + "\n"
    except:
        msg = "Error > " + str(sys.exc_info()[0]) + "\n"
    if msg:
        message ( msg )

file_deb.write ( " finish job\n" )


#  9. Register generated files into job's output data

#  Lock project directory
result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    message ( " *** ERROR: cannot lock repository (2)\n" )
    close_streams()
    sys.exit(defs.process_cant_get_lock_2())

file_deb.write ( " make output data\n" )
#job_data.data = task.make_output_data ( job_dir,job_data.id )
task.make_output_data ( job_dir,job_data )


#  10. Set job status to "finished"

file_deb.write ( " set job status 'done'\n" )

job_data.status = defs.job_done()  # finished
job_data.write ( project_repo_dir )

result = jobs.set_job_data ( project_repo_dir,job_data )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    message ( " *** ERROR: cannot set finish status\n" )
    close_streams()
    sys.exit(defs.process_cant_update_tree_2())

file_deb.write ( " git commit\n" )

result = gitut.commit ( project_repo_dir,["."],
                        "finishing job " + str(job_data.id) )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    message ( " *** ERROR: cannot commit to repository after run\n" )
    close_streams()
    sys.exit(defs.process_cant_commit_1())

file_deb.write ( " process quit\n" )


#  11. Return

close_streams()
sys.exit(defs.process_ok())


