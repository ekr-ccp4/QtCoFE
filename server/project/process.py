#!/usr/bin/python

#  ------------------------------------------------------------------
#
#  Process module -- process running primitive. It is needed to mainly
#  update job status upon termination of the job. Operations
#     1. Forms the command
#     2. Sets job status to "running"
#     3. Starts the command and waits for it to finish
#     4. Registers generated files into job's output data
#     5. Sets job status to "finished"
#     6. Returns
#
#  process.py project_repo_dir jobId
#
#  ------------------------------------------------------------------

import os
import sys
import subprocess
from varut   import gitut, utils
from project import job, datamodel


if __name__ == "__main__":

    #  Read input parameters
    project_repo_dir = sys.argv[1]
    jobID            = int(sys.arg[2])
    job_dir          = utils.get_job_dir ( project_repo_dir,jobID ):


    #  Read job data
    result = gitut.lock ( project_repo_dir )
    if result.result != "OK":
        sys.exit(1)

    job_data = job.Job();
    result = job_data.read ( project_repo_dir,jobID )
    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        sys.exit(2)

    #  1. Form the command
    proj_data = []
    job.get_projected_data    ( proj_data,project_repo_dir,jobID )
    task = datamodel.get_task ( job_data.type )
    task.read_arguments       ( job_dir       )
    cmd = task.get_command    ( proj_data     )

    #  2. Set job status to "running"
    job_data.status = 1000  # running
    job_data.write ( project_repo_dir )
#    result = jobs.set_job_data ( project_repo_dir,job_data )
#    if result.result != "OK":
#        gitut.unlock ( project_repo_dir )
#        sys.exit(3)

    result = gitut.commit ( project_repo_dir,["."],
                            "starting job " + str(job_data.id) )
    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        sys.exit(3)


    #  3. Start job and wait to finish
    file_stdout_path = os.path.join ( job_dir,"_stdout.log" )
    file_stderr_path = os.path.join ( job_dir,"_stderr.log" )
    file_stdout = open ( file_stdout_path,'w' )
    file_stderr = open ( file_stderr_path,'w' )
    p = subprocess.call ( cmd,stdout=file_stdout,stderr=file_stderr )
    file_stdout.close()
    file_stderr.close()

    #  4. Register generated files into job's output data

    #  Lock project directory
    result = gitut.lock ( project_repo_dir )
    if result.result != "OK":
        sys.exit(4)

    job_data.data = task.make_output_data ( job_dir )

    #  5. Set job status to "finished"
    job_data.status = 2000  # finished
    job_data.write ( project_repo_dir )
#    result = jobs.set_job_data ( project_repo_dir,job_data )
#    if result.result != "OK":
#        gitut.unlock ( project_repo_dir )
#        sys.exit(3)

    result = gitut.commit ( project_repo_dir,["."],
                            "finishing job " + str(job_data.id) )
    if result.result != "OK":
        gitut.unlock ( project_repo_dir )
        sys.exit(5)

    #  6. Return
    sys.exit(0)
