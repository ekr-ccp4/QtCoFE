#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype


class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        self.type  = "dtype_xyz"       # has to be "dtype_" + filename
        self.name  = "XYZ"
        self.dname = "xyz"
        self.desc  = "XYZ Coordinates"
        self.icon  = "data_xyz.png"
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

    print "\n JSON:\n" + D.to_JSON()

    sys.exit(0)
