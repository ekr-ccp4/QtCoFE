#!/usr/bin/python

#  ------------------------------------------------------------------
#   Data type definitions
#  ------------------------------------------------------------------
#

from project import dtype

def type            (): return "dtype_hkl"
def subtypeRegular  (): return "regular"
def subtypeAnomalous(): return "anomalous"

class DType(dtype.DType):

    def __init__(self,job_id,json_str=""):
        super(DType,self).__init__(job_id,json_str)
        if not json_str:
            self.type  = type()   # has to be "dtype_" + filename
            self.order = 3        # 3rd in output data lists
            self.name  = "HKL"
            self.dname = "hkl"
            self.desc  = "Reflections"
            self.icon  = "data_merged_mtz.png"
        return

    def importMTZDataset ( self,mtzDataset ):

        self.setFile ( mtzDataset.MTZ )

        self.setMeta ( "project"   ,mtzDataset.PROJECT  )
        self.addMeta ( "crystal"   ,mtzDataset.CRYSTAL  )
        self.addMeta ( "dataset"   ,mtzDataset.DATASET  )
        self.addMeta ( "hm"        ,mtzDataset.HM       )
        self.addMeta ( "cell_a"    ,str(mtzDataset.DCELL[0]) )
        self.addMeta ( "cell_b"    ,str(mtzDataset.DCELL[1]) )
        self.addMeta ( "cell_c"    ,str(mtzDataset.DCELL[2]) )
        self.addMeta ( "cell_alpha",str(mtzDataset.DCELL[3]) )
        self.addMeta ( "cell_beta" ,str(mtzDataset.DCELL[4]) )
        self.addMeta ( "cell_gamma",str(mtzDataset.DCELL[5]) )
        self.addMeta ( "wavelength",str(mtzDataset.DWAVEL)   )
        self.addMeta ( "h"         ,mtzDataset.H        )
        self.addMeta ( "k"         ,mtzDataset.K        )
        self.addMeta ( "l"         ,mtzDataset.L        )
        self.addMeta ( "free"      ,mtzDataset.FREE     )

        self.subtype = subtypeRegular()
        if mtzDataset.Ipm is None:
            self.addMeta ( "I+",None )
        else:
            self.subtype = subtypeAnomalous()
            self.addMeta ( "I+"   ,mtzDataset.Ipm.plus.value  )
            self.addMeta ( "sigI+",mtzDataset.Ipm.plus.sigma  )
            self.addMeta ( "I-"   ,mtzDataset.Ipm.minus.value )
            self.addMeta ( "sigI-",mtzDataset.Ipm.minus.sigma )

        if mtzDataset.Fpm is None:
            self.addMeta ( "F+",None )
        else:
            self.subtype = subtypeAnomalous()
            self.addMeta ( "F+"   ,mtzDataset.Fpm.plus.value  )
            self.addMeta ( "sigF+",mtzDataset.Fpm.plus.sigma  )
            self.addMeta ( "F-"   ,mtzDataset.Fpm.minus.value )
            self.addMeta ( "sigF-",mtzDataset.Fpm.minus.sigma )

        if mtzDataset.Imean is None:
            self.addMeta ( "Imean",None )
        else:
            self.addMeta ( "Imean"   ,mtzDataset.Imean.value  )
            self.addMeta ( "sigImean",mtzDataset.Imean.sigma  )

        if mtzDataset.Fmean is None:
            self.addMeta ( "Fmean",None )
        else:
            self.addMeta ( "Fmean"   ,mtzDataset.Fmean.value  )
            self.addMeta ( "sigFmean",mtzDataset.Fmean.sigma  )

        return

    def getDataSetName ( self ):
        return self.getMeta ( "project","unknown" ) + "/" + \
               self.getMeta ( "crystal","unknown" ) + "/" + \
               self.getMeta ( "dataset","unknown" )

    def getColumnNames ( self ):
        return self.getMeta ( "Imean"   ,"" ) + " " + \
               self.getMeta ( "sigImean","" ) + " " + \
               self.getMeta ( "Fmean"   ,"" ) + " " + \
               self.getMeta ( "sigFmean","" ) + " " + \
               self.getMeta ( "I+"      ,"" ) + " " + \
               self.getMeta ( "sigI+"   ,"" ) + " " + \
               self.getMeta ( "I-"      ,"" ) + " " + \
               self.getMeta ( "sigI-"   ,"" ) + " " + \
               self.getMeta ( "F+"      ,"" ) + " " + \
               self.getMeta ( "sigF+"   ,"" ) + " " + \
               self.getMeta ( "F-"      ,"" ) + " " + \
               self.getMeta ( "sigF-"   ,"" ) + " " + \
               self.getMeta ( "free"    ,"" )


    def getMeanColumns ( self ):
        #  returns column names as either
        #       [Imean,sigImean,"I"]   or
        #       [Fmean,sigFmean,"F"]   or
        #       [None,None,"X"]
        dataCol = self.getMeta ( "Imean",None )
        if dataCol == None:
            dataCol = self.getMeta ( "Fmean",None )
            if dataCol == None:
                return [None,None,"X"]
            else:
                return [dataCol,self.getMeta("sigFmean",None),"F"]
        else:
            return [dataCol,self.getMeta("sigImean",None),"I"]


    def getAnomalousColumns ( self ):
        #  returns column names as either
        #       [I+,sigI+,I-,sigI-,"I"]   or
        #       [F+,sigF+,F-,sigF-,"F"]   or
        #       [None,None,None,None,"X"]
        colNameP = self.getMeta ( "I+",None )
        colNameM = self.getMeta ( "I-",None )
        if (colNameP == None) or (colNameM == None):
            colNameP = self.getMeta ( "F+",None )
            colNameM = self.getMeta ( "F-",None )
            if (colNameP == None) or (colNameM == None):
                return [None,None,None,None,"X"]
            else:
                return [colNameP,self.getMeta("sigF+",None),
                        colNameM,self.getMeta("sigI-",None),"F"]
        else:
          return [colNameP,self.getMeta("sigI+",None),
                  colNameM,self.getMeta("sigI-",None),"I"]


    def makeDName ( self,serialNo ):

        if serialNo > 0:
            self.makeDataId ( serialNo )

        attr = ""
        if self.subtype == subtypeAnomalous():
            asign = "A "
            for i in range(len(self.metadata)):
                if ((self.metadata[i].key=="I+") or       \
                    (self.metadata[i].key=="I-")) and     \
                   (self.metadata[i].value!=None):
                    attr += self.metadata[i].value + ":"
            if not attr:
                for i in range(len(self.metadata)):
                    if ((self.metadata[i].key=="F+") or     \
                        (self.metadata[i].key=="F-")) and   \
                       (self.metadata[i].value!=None):
                        attr += self.metadata[i].value + ":"
        else:
            asign = "R "
            for i in range(len(self.metadata)):
                if (self.metadata[i].key=="Imean") and   \
                   (self.metadata[i].value!=None):
                    attr += self.metadata[i].value + ":"
            if not attr:
                for i in range(len(self.metadata)):
                    if (self.metadata[i].key=="Fmean") and  \
                       (self.metadata[i].value!=None):
                        attr += self.metadata[i].value + ":"

        if len(self.files) > 0:
            if serialNo > 0:
                self.dname = "[" + self.dataId + "]" + asign + self.files[0]
            else:
                self.dname = self.files[0]

        if attr:
            self.dname += "/" + attr[:-1]

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
