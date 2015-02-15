#!/usr/bin/python

#  ------------------------------------------------------------------
#  Task class template. Tasks and DTypes represent the data model.
#  ------------------------------------------------------------------

from varut  import jsonut, utils
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

    def __init__(self):

        self.type    = "task_template"
        self.name    = "Task"
        self.desc    = "Template task"
        self.section = section_none().id
        self.order   = 0  # position within section for GUI
        self.icon    = "task_template.png"

        #   Input/output data lists contain data types which is
        # required/produced by the given task. Each data type
        # corresponds to a list (vector) of actual data. Full
        # data lists are kept in 'data' field of the corresponding
        # Job classes.
        #   Data list requirements are specified by modification letter
        # and number:
        #  EN : the corresponding data list must contain exactly N
        #       data entities
        #  UN : the corresponding data list may contain up to N
        #       data entities (inclusive of N)
        #  GN : the corresponding data list must contain more than N
        #       data entities (N exclusive)
        self.inp_data = [
                         [dummy.DType().type,"G",-1]
                        ]
        self.out_data = [
                         [any  .DType().type,"G",-1]
                        ]

        return

    def run(self,inp):
        return utils.make_return ( inp.action,"OK","OK" )



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
