#!/usr/bin/python

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
from varut   import gitut, utils, defs
from project import job, datamodel, jobs


#  Read input parameters
project_repo_dir = sys.argv[1]
defs.set_master_path ( sys.argv[2] )
jobID   = int(sys.argv[3])

bin_dir = defs.bin_path()
job_dir = utils.get_job_dir ( project_repo_dir,jobID )

#  Read job data
result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    sys.exit(defs.process_cant_get_lock_1())

job_data = job.Job();
result   = job_data.read ( project_repo_dir,jobID )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    sys.exit(defs.process_cant_read_job())

#  1. Form the command
proj_data = []
job.get_projected_data    ( proj_data,project_repo_dir,jobID )
task = datamodel.get_task ( job_data.type )
task.read_arguments       ( job_dir       )
cmd = [os.path.join(bin_dir,task.executable)] + task.get_command(proj_data)

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
    sys.exit(defs.process_cant_commit_1())


#  3. Start job and wait to finish
file_stdout_path = os.path.join ( job_dir,"_stdout.log" )
file_stderr_path = os.path.join ( job_dir,"_stderr.log" )
file_stdout = open ( file_stdout_path,'w' )
file_stderr = open ( file_stderr_path,'w' )

#  initiate report document
pyrvapi.rvapi_init_document ( "report",os.path.join(job_dir,"html"),
                              job_data.desc,1,7,
                              defs.jsrview_path(),None,None,None )
pyrvapi.rvapi_add_tab ( "logfile","Log file",True );
pyrvapi.rvapi_append_content ( file_stdout_path,True,"logfile" );
pyrvapi.rvapi_flush();


os.chdir ( job_dir )
p = subprocess.call ( cmd,stdout=file_stdout,stderr=file_stderr )
file_stdout.close()
file_stderr.close()

#  4. Register generated files into job's output data

#  Lock project directory
result = gitut.lock ( project_repo_dir )
if result.result != "OK":
    sys.exit(defs.process_cant_get_lock_1())

job_data.data = task.make_output_data ( job_dir,job_data.id )

#  5. Set job status to "finished"
job_data.status = defs.job_done()  # finished
job_data.write ( project_repo_dir )

result = jobs.set_job_data ( project_repo_dir,job_data )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    sys.exit(defs.process_cant_update_tree_2())

result = gitut.commit ( project_repo_dir,["."],
                        "finishing job " + str(job_data.id) )
if result.result != "OK":
    gitut.unlock ( project_repo_dir )
    sys.exit(defs.process_cant_commit_1())

#  6. Return
sys.exit(defs.process_ok())


