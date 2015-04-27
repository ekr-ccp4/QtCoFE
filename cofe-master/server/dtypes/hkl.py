#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        self.type  = "dtype_hkl"   # has to be "dtype_" + filename
        self.name  = "HKL"
        self.dname = "hkl"
        self.desc  = "Reflections"
        self.icon  = "data_merged_mtz.png"
        return

    def makeDName ( self,serialNo ):
        if len(self.files) > 0:
            self.dname = "[" + str(self.jobId) + "-" + str(serialNo) + \
                          "] " + self.files[0]
            for i in range(len(self.metadata[0])):
                if i == 0:
                    self.dname += "/"
                else:
                    self.dname += ":"
                self.dname += self.metadata[0][i]
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