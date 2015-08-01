#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

def type(): return "dtype_dummy"    # has to be "dtype_" + filename

class DType(dtype.DType):  # for data import task input and root output

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        if not json_str:
            self.type  = type()   # has to be "dtype_" + filename
            self.name  = "Dummy"
            self.dname = "dummy"
            self.desc  = "No data required"
            self.icon  = "dtype_dummy.png"
        return


#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    D = DType()

    print "\ntype : " + D.type
    print "name : " + D.name
    print "desc : " + D.desc
    print "icon : " + D.icon

    sys.exit(0)
