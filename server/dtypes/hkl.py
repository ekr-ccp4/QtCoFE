#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        self.type = "dtype_hkl"   # has to be "dtype_" + filename
        self.name = "HKL"
        self.desc = "Reflections"
        self.icon = "data_merged_mtz.png"
        self.columns = []
        return

    def addColumn ( self,colname ):
        self.columns.append ( colname )
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
