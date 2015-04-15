#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type template
#  ------------------------------------------------------------------
#

from varut import jsonut


class DType(jsonut.jObject):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(json_str)
        self.type  = "dtype_dtype"  # has to be "dtype_" + filename
        self.name  = "Template"
        self.desc  = "Data type template"
        self.icon  = "dtype_template.png"
        self.jobId = job_id;
        self.files = []  # may be a multiple-file data type
        self.metadata = [] # list of lists per file item
        return

    def setFile ( self,fname,meta ):
        self.files    = [fname]
        self.metadata = [meta]
        return

    def addFile ( self,fname,meta ):
        self.files   .append ( fname )
        self.metadata.append ( meta  )
        return

#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    print " argv[0]=" + sys.argv[0]

    D = DType()

    print "\ntype : " + D.type
    print "name : " + D.name
    print "desc : " + D.desc
    print "icon : " + D.icon

    print "\n JSON:\n" + D.to_JSON()

    sys.exit(0)
