#!/usr/bin/python

#  ------------------------------------------------------------------
#  Project Tree class
#  ------------------------------------------------------------------

import os
from varut   import gitut, jsonut, utils, defs
from project import job

class Tree(jsonut.jObject):

    def __init__ ( self,json_str="" ):
        super(Tree,self).__init__(json_str)

    def set_minimal_data ( self,name,desc ):
        self.name        = name   # project name
        self.desc        = desc   # project description
        self.job_count   = 0
        self.current_job = 0;
        job_data = job.Job();
        job_data.set_minimal_data ( name,"Project root","task_root",
                                    -1,self.job_count )
        job_data.status  = defs.job_done()
        self.jobs        = [job_data]
        return;


    def read ( self,project_repo_dir ):
    #  Lock repository before calling this function

        if not os.path.isdir(project_repo_dir):
            return utils.make_return ( "","no_project_repo_dir",
                                  "Project data repository not found" )

        result = gitut.checkout ( project_repo_dir,[] )
        if result.result != "OK":
            result.jobs = []
            return result

        super(Tree,self).read_json ( open (
            os.path.join ( project_repo_dir,
                           defs.project_data_name())).read() )

        return utils.make_return ( "","OK","OK" )



    def write ( self,project_repo_dir ):
        file = open ( os.path.join(project_repo_dir,
                      defs.project_data_name()),"w" )
        file.write ( self.to_JSON() )
        file.close ()
        return


#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    T = Tree()
    T.set_minimal_data ( "project-name","project-desc" )
    print "\nTree:\n\n" + T.to_JSON()

    sys.exit(0)
