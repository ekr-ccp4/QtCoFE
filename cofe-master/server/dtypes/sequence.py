#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype


class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        self.type  = "dtype_sequence"    # has to be "dtype_" + filename
        self.name  = "Sequence"
        self.dname = "sequence"
        self.desc  = "Macromolecular sequence"
        self.icon  = "data_sequence.png"
        return


    def makeDName ( self,serialNo ):
        if len(self.files) > 0:
            self.dname = "[" + str(self.jobId) + "-" + str(serialNo) + \
                         "] " + self.files[0]
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
