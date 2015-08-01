#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type template
#  ------------------------------------------------------------------
#

from varut import jsonut

def type():  return "dtype_dtype"    # has to be "dtype_" + filename

class DType(jsonut.jObject):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(json_str)
        if not json_str:
            self.type     = type()    # has to be "dtype_" + filename
            self.subtype  = "."       # default 'basic' subtype
            self.order    = 0         # for ordered output
            self.name     = "Template"
            self.dname    = "template"
            self.desc     = "Data type template"
            self.icon     = "dtype_template.png"
            self.jobId    = job_id;
            self.dataId   = "0-0"
            self.files    = []  # may be a multiple-file data type
            self.metadata = [] # list of lists per file item
        return

    def makeDataId ( self,serialNo ):
        self.dataId = str(self.jobId) + "-" + str(serialNo)
        return

    def setFile ( self,fname ):
        self.files = [fname]
        return

    def setMeta ( self,key,value ):
        meta = jsonut.jObject()
        meta.key      = key;
        meta.value    = value
        self.metadata = [meta]
        return

    def addFile ( self,fname ):
        self.files.append ( fname )
        return

    def addMeta ( self,key,value ):
        meta = jsonut.jObject()
        meta.key      = key;
        meta.value    = value
        self.metadata.append ( meta )
        return

    def getMeta ( self,key,default_value=None ):
        for m in self.metadata:
            if m.key == key:
                if m.value == None:
                    return default_value
                else:
                    return m.value
        return default_value

    def makeDName ( self,serialNo ):
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
