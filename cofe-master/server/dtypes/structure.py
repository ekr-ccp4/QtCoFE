#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype


class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        self.type  = "dtype_structure"   # has to be "dtype_" + filename
        self.name  = "Structure"
        self.dname = "structure"
        self.desc  = "Crystal structure"
        self.icon  = "data_structure.png"
        return

    def makeDName ( self,serialNo ):
        self.dname = "structure " + str(self.jobId) + "-" + str(serialNo)
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
