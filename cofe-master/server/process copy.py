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
import subprocess
import pyrvapi
from   varut   import gitut, utils, defs
from   project import job, datamodel, jobs


#  Read input parameters
project_repo_dir = sys.argv[1]
defs.set_master_path ( sys.argv[2] )
jobID   = int(sys.argv[3])

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


bin_dir = defs.bin_path()
job_dir = utils.get_job_dir ( project_repo_dir,jobID )

#  Read job data
result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    file_deb.close()
    sys.exit(defs.process_cant_get_lock_1())

job_data = job.Job();
result   = job_data.read ( project_repo_dir,jobID )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    file_deb.close()
    sys.exit(defs.process_cant_read_job())

#  1. Form the command
projected_data = []
job.get_projected_data    ( projected_data,project_repo_dir,jobID )
task = datamodel.get_task ( job_data.type )
task.read_arguments       ( job_dir       )
cmd = [os.path.join(bin_dir,task.executable)] + task.get_command(projected_data)

# save command in job directory for reference
file_cmd_path = os.path.join ( job_dir,"_command.sh" )
file_cmd = open ( file_cmd_path,'w' )
for i in range(len(cmd)):
    if i > 0:
        file_cmd.write ( "    \\ \n      " )
    file_cmd.write ( cmd[i] )
file_cmd.write ( "\n" )
file_cmd.close()

#  2. Set job status to "running"
job_data.status = defs.job_running()  # running
job_data.write ( project_repo_dir )
result = jobs.set_job_data ( project_repo_dir,job_data )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    sys.exit(defs.process_cant_update_tree_1())

result = gitut.commit ( project_repo_dir,["."],
                        "running job " + str(job_data.id) )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    file_deb.close()
    sys.exit(defs.process_cant_commit_1())


#  3. Start job and wait to finish
file_stdout_path = os.path.join ( job_dir,"_stdout.log" )
file_stderr_path = os.path.join ( job_dir,"_stderr.log" )
file_stdout = open ( file_stdout_path,'w' )
file_stderr = open ( file_stderr_path,'w' )

#  initiate report document
file_deb.write ( " rvapi_init_document\n" )

pyrvapi.rvapi_init_document ( "report",os.path.join(job_dir,"html"),
                              job_data.desc,1,7,
                              defs.jsrview_path(),None,None,"task.tsk",
                              "i2.xml" )

pyrvapi.rvapi_add_tab ( "logfile","Log file",True );
pyrvapi.rvapi_append_content ( file_stdout_path,True,"logfile" );
pyrvapi.rvapi_flush();

file_deb.write ( " start job\n" )
os.chdir ( job_dir )
msg = ""
try:
    p = subprocess.call ( cmd,stdout=file_stdout,stderr=file_stderr )
except OSError as e:
    msg = "OSError > " + str(e.errno) + "\n" + \
          "OSError > " + e.strerror   + "\n" + \
          "OSError > " + e.filename   + "\n"
except:
    msg = "Error > " + sys.exc_info()[0] + "\n"

if msg:
    file_deb.write ( msg )

file_deb.write ( " finish job\n" )

file_stdout.close()
file_stderr.close()

#  4. Register generated files into job's output data

#  Lock project directory
result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    file_deb.close()
    sys.exit(defs.process_cant_get_lock_1())

file_deb.write ( " make output data\n" )
#job_data.data = task.make_output_data ( job_dir,job_data.id )
task.make_output_data ( job_dir,job_data )

#  5. Set job status to "finished"
file_deb.write ( " set job status 'done'\n" )
job_data.status = defs.job_done()  # finished
job_data.write ( project_repo_dir )

result = jobs.set_job_data ( project_repo_dir,job_data )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    file_deb.close()
    sys.exit(defs.process_cant_update_tree_2())

file_deb.write ( " git commit\n" )

result = gitut.commit ( project_repo_dir,["."],
                        "finishing job " + str(job_data.id) )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    file_deb.close()
    sys.exit(defs.process_cant_commit_1())

file_deb.write ( " process quit\n" )

#  6. Return
file_deb.close()
sys.exit(defs.process_ok())


