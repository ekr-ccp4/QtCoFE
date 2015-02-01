#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type template
#  ------------------------------------------------------------------
#

from varut import jsonut


class DType(jsonut.jObject):

    def __init__(self):
        self.type = "dtype_template"
        self.name = "Template"
        self.desc = "Data type template"
        self.icon = "dtype_template.png"
        self.file = ""
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
