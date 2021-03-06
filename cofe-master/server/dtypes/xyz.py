#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

def type(): return "dtype_xyz"       # has to be "dtype_" + filename

class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        if not json_str:
            self.type  = type()     # has to be "dtype_" + filename
            self.order = 4          # 4th in output data lists
            self.name  = "XYZ"
            self.dname = "xyz"
            self.desc  = "XYZ Coordinates"
            self.icon  = "data_xyz.png"
        return

    def makeDName ( self,serialNo ):
        if serialNo > 0:
            self.makeDataId ( serialNo )
        if len(self.files) > 0:
            if serialNo > 0:
                self.dname = "[" + self.dataId + "] " + self.files[0]
            else:
                self.dname = self.files[0]
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
