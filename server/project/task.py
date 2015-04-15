#!/usr/bin/python

#  ------------------------------------------------------------------
#  Task class template. Tasks and DTypes represent the data model.
#  ------------------------------------------------------------------

import  os
from varut  import jsonut, utils, defs
from dtypes import dummy, any

#  constants for annotation in GUI dialogs/menus
def section_none():
    r = jsonut.jObject()
    r.id    = "tasksect_none"
    r.desc  = "Dummy"
    r.name  = "Dummy"
    r.order = -1
    return r

def section_data():
    r = jsonut.jObject()
    r.id    = "tasksect_data"
    r.desc  = "Data utilities"
    r.name  = "Data"
    r.order = 0
    return r

def section_data_proc():
    r = jsonut.jObject()
    r.id    = "tasksect_dataproc"
    r.desc  = "Data processing"
    r.name  = "Data processing"
    r.order = 1
    return r

def section_phasing():
    r = jsonut.jObject()
    r.id    = "tasksect_phasing"
    r.desc  = "Phasing"
    r.name  = "Phasing"
    r.order = 2
    return r

def section_building():
    r = jsonut.jObject()
    r.id    = "tasksect_building"
    r.desc  = "Model building"
    r.name  = "Model building"
    r.order = 3
    return r

def section_refinement():
    r = jsonut.jObject()
    r.id    = "tasksect_refinement"
    r.desc  = "Refinement"
    r.name  = "Refinement"
    r.order = 4
    return r

def section_validation():
    r = jsonut.jObject()
    r.id    = "tasksect_validation"
    r.desc  = "Validation"
    r.name  = "Validation"
    r.order = 5
    return r

def section_utilities():
    r = jsonut.jObject()
    r.id    = "tasksect_utilities"
    r.desc  = "Utilities"
    r.name  = "Utilities"
    r.order = 6
    return r



class Task(jsonut.jObject):

    #  Template task description class

    def __init__(self):

        self.type    = "task_task"       # must be "task_" + filename
        self.name    = "Task"            # for job tree and data dialogs
        self.desc    = "Template task"   # for data dialogs
        self.section = section_none().id # for task dialog
        self.order   = 0                 # position within the section
        self.icon    = "task_template.png"

        #   Input/output data lists contain data types which is
        # required/produced by the given task. Each data type
        # corresponds to a list (vector) of actual data. Actual
        # data lists are kept in the 'data' field of the corresponding
        # Job classes.
        #   Data list requirements are specified by the modification
        # letter and number:
        #  E : the corresponding data list must contain exactly N
        #      data entities
        #  U : the corresponding data list may contain up to N
        #      data entities (inclusive of N)
        #  G : the corresponding data list must contain more than N
        #      data entities (N exclusive)
        self.inp_data = [
                         [dummy.DType().type,"G",-1]
                        ]
        self.out_data = [
                         [any  .DType().type,"G",-1]
                        ]

        self.executable = "taskpgm"      # program to run
        self.arguments  = [              # list of arguments
          { "keyword": "-keyword",   # may be empty
            "value"  : "value",      # always a string, typed as below
            "type"   : "type",       # string, int, real, bool, dtype_xxxx
            "dtype"  : "meta",       # metadata for dtype_xxxx values
            "name"   : "Parameter",  # name for display
            "section": "_main",      # _main, _hidden or any custom (advanced)
            "order"  : 1             # order within parameter display section
          }    # continue the list as necessary
        ]

        return


    def make_output_data ( self,job_dir ):
        #  This function should return data array for the corresponding
        # Job class with references to task output data in job_dir
        # after the corresponding job completes
        return [[any.DType()]]


    def write_arguments ( self,job_dir,executable,arguments ):
        #  Lock repository before calling this function
        file = open ( os.path.join(job_dir,
                                   defs.job_arguments_name()),"w" )
        A = jsonut.jObject()
        A.executable = executable
        A.arguments  = arguments
        file.write ( A.to_JSON() )
        file.close ()
        return


    def read_arguments ( self,job_dir ):
        #  Lock repository before calling this function
        A = jsonut.jObject ( open ( os.path.join ( job_dir,
                                    defs.job_arguments_name() ) )
                                  .read() )
        self.executable = A.executable
        self.arguments  = A.arguments
        return


    def get_command ( self,proj_data ):

        cmd = []

        for i in range(len(self.arguments)):
            if arguments[i].keyword:
                cmd = cmd + [arguments[i].keyword]
            elif arguments[i].value:
                cmd = cmd + [arguments[i].value]

        return cmd


#    def run(self,inp):
#        return utils.make_return ( inp.action,"OK","OK" )



#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    T = Task()
    print "\nTemplate task:\n\n" + T.to_JSON()

    sys.exit(0)
