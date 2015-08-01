#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

def type(): return "dtype_images"     # has to be "dtype_" + filename

class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        if not json_str:
            self.type  = type()      # has to be "dtype_" + filename
            self.order = 2           # 2nd in output data lists
            self.name  = "Images"
            self.dname = "images"
            self.desc  = "Diffraction images"
            self.icon  = "data_image.png"
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
