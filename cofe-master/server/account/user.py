#!/usr/bin/python

#  ------------------------------------------------------------------
#  User class
#  ------------------------------------------------------------------

import os
import hashlib
from   varut  import gitut, jsonut, utils, defs

class User(jsonut.jObject):

    def __init__(self,json_str=""):
        super(User,self).__init__(json_str)

    def set_user ( self,user_name,email,login,projects,pwd ):
        self.user_name = user_name
        self.email     = email
        self.login     = login
        self.projects  = projects
        self.pwd       = hashlib.md5(pwd).hexdigest()

    def read ( self,user_repo_dir ):
    #  Lock repository before calling this function

        result = gitut.checkout ( user_repo_dir,['--',defs.user_data_name()] )
        if result.result != "OK":
            return result

        super(User,self).read_json ( open (
            os.path.join(user_repo_dir,defs.user_data_name())).read() )

        return utils.make_return ( "","OK","OK" )


    def write ( self,user_repo_dir ):
        file = open ( os.path.join(user_repo_dir,defs.user_data_name()),"w" )
        file.write  ( self.to_JSON() )
        file.close  ()
        return


#
#  ------------------------------------------------------------------
#   Tests
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    U = User()
    U.set_user ( "eugene","eugene@Mimas.local","ekr",[] )
    print "\nUser:\n\n" + U.to_JSON()

    sys.exit(0)
