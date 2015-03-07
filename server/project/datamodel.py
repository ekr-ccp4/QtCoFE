#!/usr/bin/python

#  ------------------------------------------------------------------
#   Getting list of data types and tasks
#  ------------------------------------------------------------------
#
#  Functions:  datamodel.get_data_model ( inp )
#
#  Input 'inp' is neglected but an argument needs to be provided for
#  consistency in sys.argv[1]:
#
#  {
#    "action"      : action_key, ("get_data_model")
#    "login"       : user_login,
#    "pwd"         : password,  (md5)
#    "master_path" : master_path,
#    "project"     : project-id,
#  }
#
#  Output is a list of JSON object with descriptions of all data types:
#
#  {
#    "action"     : action-key,
#    "result"     : result-key,
#    "message"    : error-desc,
#    "dtypes"     : [
#       { "type": type1, "name": name1, "desc": desc1, "icon": icon1 },
#       { "type": type2, "name": name2, "desc": desc2, "icon": icon2 },
#       { "type": type3, "name": name3, "desc": desc3, "icon": icon3 },
#       ..............................
#    ],
#    "sections" : {
#       { "id": id1, "name": name1, "desc": desc1, "order": order1 },
#       { "id": id2, "name": name2, "desc": desc2, "order": order2 },
#       ..............................
#    ],
#    "tasks"    : [
#       { "type": type1, "name": name1, "section": section1,
#         "desc": desc1, "icon": icon1, "order"  : order1,
#         "input_dtypes" : [ type1,type2,... ],
#         "output_dtypes": [ type1,type2,... ]
#       },
#       { "type": type2, "name": name2, "section": section2,
#         "desc": desc2, "icon": icon2, "order"  : order2,
#         "input_dtypes" : [ type1,type2,... ],
#         "output_dtypes": [ type1,type2,... ]
#       },
#       ..............................
#    ]
#  }
#
#  ------------------------------------------------------------------
#

import os
import glob
from   varut import jsonut, utils
from   project import task
import dtypes

def dtype_list():

    dirName =  os.path.dirname(os.path.abspath(__file__ + "/../")) + "/dtypes/"
    dtl     = glob.glob ( dirName + "*.py" )

    for i in range(len(dtl)):
        dtl[i] = dtl[i].replace(dirName,"").replace(".py","")

    dtl.remove ( "__init__" )

    return dtl



def tasktype_list():

    dirName =  os.path.dirname(os.path.abspath(__file__ + "/../")) + "/tasks/"
    tlist   = glob.glob ( dirName + "*.py" )

    for i in range(len(tlist)):
        tlist[i] = tlist[i].replace(dirName,"").replace(".py","")

    tlist.remove ( "__init__" )

    return tlist



def section_list():

    return [f for f in dir(task) if f.startswith("section")]



def get_data_model(inp):

    if inp.action != "get_data_model":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    data_model = utils.make_return ( inp.action,"OK","OK" )

    dtlist = dtype_list()
    data_model.dtypes = []

    for dt in dtlist:
        dtype = __import__("dtypes."+dt)
        data_model.dtypes.append ( getattr(dtype,dt).DType(-1) )

    data_model.sections = \
     [getattr(task,f)() for f in dir(task) if f.startswith("section_")]

    tlist = tasktype_list()
    data_model.tasks = []

    for t in tlist:
        tsk = __import__("tasks."+t)
        data_model.tasks.append ( getattr(tsk,t).Task() )

    return data_model

#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#
#
if __name__ == "__main__":
    import sys

#    import inspect, os
#    print inspect.stack()[0][1]
#    print os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
#    print os.path.dirname(os.path.abspath(__file__))

    inp = jsonut.jObject ( '{ "action" : "get_data_model" }' )

    result = get_data_model ( inp )
    print ( result.to_JSON() )

    sys.exit(0)
