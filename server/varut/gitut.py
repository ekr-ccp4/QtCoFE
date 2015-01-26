#!/usr/bin/python

#
#  ------------------------------------------------------------------
#   GIT management
#  ------------------------------------------------------------------
#

import os
import time
import subprocess
import jsonut


def git ( repo_dir,params ):
    try:
        p = subprocess.Popen ( ['git'] + params, cwd=repo_dir,
                               stdout=subprocess.PIPE )
        if p.wait() == 0:
            return "OK"
        elif "nothing to commit" in p.stdout.read():
            return "OK"
        else:
            return "fail 'git " + str(params) + "' in '" + repo_dir + \
                   "' rc=" + str(p.returncode)
    except OSError,err:
        return "exception on 'git " + " ".join(params) + "' in '" + \
               repo_dir + "': " + err


def lock ( repo_dir ):

    result = jsonut.jObject()

    rc = git ( repo_dir,['branch','__lock__'] )
    if rc.startswith("exception"):
        result.result  = "git_error"
        result.message = rc
        return rc

    n  = 0;
    while rc != "OK" and n < 2:
        time.sleep ( 1 )
        rc = git ( repo_dir,['branch','__lock__'] )
        n += 1

    if rc != "OK":
        result.result  = "git_error_lock"
        result.message = "Could not acquire a lock on repository."
        return result

    result.result  = "OK"
    result.message = "Repository locked"
    return result


def unlock ( repo_dir ):
    return git ( repo_dir,['branch','-D','__lock__'] )


def checkout ( repo_dir,files ):
# Lock repository before checking out

    result = jsonut.jObject()

    rc = git ( repo_dir,["checkout"] + files )
    if rc != "OK":
        result.result   = "git_error_checkout"
        result.message  = rc
        return result

    result.result   = "OK"
    result.message  = "Successful checkout"
    return result


def commit ( repo_dir,files,message ):
# Repository should be locked before and unlocked after this function

    result = jsonut.jObject()

    rc = git ( repo_dir,['add','-A'] + files )
    if rc != "OK":
        result.result  = "git_error_add"
        result.message = rc
        return result

    rc = git ( repo_dir,['commit','-m',message] )
    if rc != "OK":
        result.result  = "git_error_commit"
        result.message = rc
        return result

    rc = unlock ( repo_dir )
    if rc != "OK":
        result.result  = "git_error_unlock"
        result.message = rc
        return result

    result.result  = "OK"
    result.message = "Success"
    return result

#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#

#if __name__ == "__main__":
#    import sys
#    me = Object()
#    me.name = "Onur"
#    me.age = 35
#    me.dog = Object()
#    me.dog.name = "Apollo"
#
#    print(me.to_JSON())
#
#    sys.exit(0)
