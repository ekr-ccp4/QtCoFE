#!/usr/bin/python

#  ------------------------------------------------------------------
#   Serving project list queries
#  ------------------------------------------------------------------
#
#  Functions:  projects.get_list ( inp )
#              projects.add      ( inp )
#              projects.set_desc ( inp )
#              projects.delete   ( inp )
#
#
#  Input 'inp' is JSON string in sys.argv[1]:
#
#  {
#    "action"      : action_key,
#    "login"       : user_login,
#    "pwd"         : password,  (md5)
#    "projects_path" : projects_path,
#    "data"        :  {
#        "parent"  : project_parent,
#        "sibling" : project_sibling,
#        "name"    : project_name,
#        "desc"    : project_desc
#    }
#  }
#
#  Action key values:
#
#    "get_list_of_projects" : just returns the project tree; "data"
#                             block is ignored.
#                               Function: get_list ( inp )
#
#    "add_project"          : adds new project "name" with description
#                             "desc" after project "sibling" as a child
#                             to project "parent". Empty "parent" means
#                             root, empty "sibling" will put new
#                             project as 1st child of "parent".
#                               Function: add ( inp )
#
#    "set_project_desc"     : changes description of porject 'project_name'
#                             to 'project_desc'
#                               Function: set_desc ( inp )
#
#    "delete_project"       : deletes project "nnnnnn" in given path
#
#  "project_path" is unix-style path, made of slash-separated project
#  names.
#
#  Output is, in all cases, the JSON description of project tree,
#  printed in stdout, e.g.,:
#
#  {
#    "action" : action_key,
#    "result" : result_key,
#    "message": error_desc,
#    "projects": [
#       { "name": name1, "desc": desc1,
#         "projects: []
#       },
#       { "name": name2, "desc": desc2,
#         "projects": [
#             { "name": "name21", "desc": desc21,
#               "projects: []
#             },
#             { "name": "name22", "desc": desc22,
#               "projects": [
#                   { "name": "name221", "desc": desc221,
#                     "projects: []
#                   }
#                ]
#             }
#          ]
#       },
#       { "name": "name3", "desc": desc3,
#         "projects: []
#       }
#    ],
#  }
#
#  where 'nameI' is name of Ith project, and value of "current" gives
#  name of currently selected project.
#
#  ------------------------------------------------------------------
#

import os
import shutil
from account import user
from project import tree
from varut   import jsonut, gitut, utils, defs


def get_list(inp):

    if inp.action != "get_list_of_projects":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    repo_dir = utils.get_user_repo_path ( defs.projects_path(),inp.login )

    result = gitut.lock ( repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result    = user_data.read ( repo_dir )
    gitut.unlock(repo_dir)

    return utils.add_return ( inp.action,result,user_data )


def __find ( projects,name ):

    for p in projects:
        if p.name == name:
            result = jsonut.jObject();
            result.p = p
            result.v = projects
            return result
        elif len(p.projects) > 0:
            result = __find ( p.projects,name )
            if result:
                return result

    return None


def __add ( projects,parent,sibling ):

    if len(projects) == 0:
        p = jsonut.jObject();
        projects.append(p);
        return p

    if not parent:
        if not sibling:
            p = jsonut.jObject();
            projects.insert ( 0,p );
            return p
        k = len(projects)
        for i in range(len(projects)):
            if projects[i].name == sibling:
                k = i+1
                break
        p = jsonut.jObject();
        projects.insert ( k,p );
        return p

    p = None
    for pp in projects:
        if pp.name == parent:
            p = __add ( pp.projects,"",sibling )
        elif len(pp.projects) > 0:
            p = __add ( pp.projects,parent,sibling )
        if p:
            break

    return p


def add(inp):

    if inp.action != "add_project":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.projects_path(),inp.login )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )

    if result.result != "OK":
        gitut.unlock ( user_repo_dir )
        return utils.add_return ( inp.action,result,user_data )

    project_repo_dir = utils.get_project_repo_path (
                            defs.projects_path(),inp.login,inp.data.name )
    if os.path.isdir(project_repo_dir):
        gitut.unlock ( user_repo_dir )
        user_data.action  = inp.action
        user_data.result  = "duplicate_project_name"
        user_data.message = "Project name '" + inp.data.name + "' already exists"
        return user_data

    p = __add ( user_data.projects,inp.data.parent,inp.data.sibling )

    if not p:
        gitut.unlock ( user_repo_dir )
        return utils.make_return ( inp.action,
            "wrong_project_specs",
            "Wrong parent or sibling project names" )

    p.name     = inp.data.name
    p.desc     = inp.data.desc
    p.projects = []

    user_data.write ( user_repo_dir )

    result = gitut.commit ( user_repo_dir,[defs.user_data_name()],
                            "add project " + p.name )
    if result.result == "OK":
        project_data = tree.Tree()
        project_data.set_minimal_data ( inp.data.name,inp.data.desc )
        result = utils.make_repo ( project_repo_dir,
                     defs.project_data_name(),project_data.to_JSON() )

    user_data.action = inp.action
    user_data.result = result.result
    if result.result == "OK":
        user_data.current_project = inp.data.name
        user_data.message = "Project added successfully"
    else:
        user_data.message = result.message

    gitut.unlock ( user_repo_dir )

    return user_data



def set_desc(inp):

    if inp.action != "set_project_desc":
        return utils.make_return ( inp.action,
                             "wrong_action_code","Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.projects_path(),inp.login )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )

    if result.result != "OK":
        gitut.unlock ( user_repo_dir )
        return utils.add_return ( inp.action,result,user_data )

    p = __find ( user_data.projects,inp.data.name )
    if not p:
        gitut.unlock(user_repo_dir)
        user_data.action  = inp.action
        user_data.result  = "project_not_found"
        user_data.message = "Project " + inp.data.name + " not found."
        return user_data

    project_repo_dir = utils.get_project_repo_path (
                            defs.projects_path(),inp.login,inp.data.name )

    result = gitut.lock ( project_repo_dir )
    if result.result != "OK":
        gitut.unlock ( user_repo_dir )
        return utils.pass_return ( inp.action,result )

    project_data = tree.Tree()
    result = project_data.read ( project_repo_dir )

    if result.result != "OK":
        gitut.unlock ( user_repo_dir    )
        gitut.unlock ( project_repo_dir )
        return utils.add_return ( inp.action,result,user_data )

    p.p.desc          = inp.data.desc
    project_data.desc = inp.data.desc

    user_data.write    ( user_repo_dir    )
    project_data.write ( project_repo_dir )

    result_u = gitut.commit ( user_repo_dir,[defs.user_data_name()],
                         "change description for project " + p.p.name )
    result_p = gitut.commit ( project_repo_dir,[defs.project_data_name()],
                         "change project description" )

    user_data.action = inp.action
    user_data.current_project = inp.data.name
    user_data.result = ""
    if result_u.result !="OK":
        user_data.result  = result_u.result
        user_data.message = result_u.message
    if result_p.result !="OK":
        user_data.result  += ":"  + result_p.result
        user_data.message += "; " + result_p.message
    if not user_data.result:
        user_data.result  = "OK"
        user_data.message = "Project description changed successfully"

    gitut.unlock ( user_repo_dir    )
    gitut.unlock ( project_repo_dir )

    return user_data



def __delete_project_dirs ( project,login ):

    shutil.rmtree ( utils.get_project_repo_path (
                        defs.projects_path(),login,project.name ),
                            ignore_errors=True )

    for p in project.projects:
        __delete_project_dirs ( p,login )

    return



def delete(inp):

    if inp.action != "delete_project":
        return utils.make_return ( inp.action,"wrong_action_code",
                                              "Wrong action code" )

    user_repo_dir = utils.get_user_repo_path ( defs.projects_path(),inp.login )

    result = gitut.lock ( user_repo_dir )
    if result.result != "OK":
        return utils.pass_return ( inp.action,result )

    user_data = user.User()
    result = user_data.read ( user_repo_dir )

    if result.result != "OK":
        gitut.unlock ( user_repo_dir )
        return utils.add_return ( inp.action,result,user_data )

    p = __find ( user_data.projects,inp.data.name )
    if not p:
        gitut.unlock(user_repo_dir)
        user_data.action  = inp.action
        user_data.result  = "project_not_found"
        user_data.message = "Project " + inp.data.name + " not found."
        return user_data

    __delete_project_dirs ( p.p,inp.login )
    p.v.remove ( p.p )

    user_data.write ( user_repo_dir )
    result = gitut.commit ( user_repo_dir,[defs.user_data_name()],
                            "delete project " + inp.data.name + \
                            " and its children" )

    user_data.action          = inp.action
    user_data.current_project = inp.data.next
    if result.result !="OK":
        user_data.result  = result.result
        user_data.message = result.message
    else:
        user_data.result  = "OK"
        user_data.message = "Project deleted successfully"

    gitut.unlock(user_repo_dir)

    return user_data


#
#  ------------------------------------------------------------------
#   Use example
#  ------------------------------------------------------------------
#

if __name__ == "__main__":
    import sys

    inp = jsonut.jObject (
       '{ "action"      : "get_list_of_projects",'
       '  "projects_path" : "../cofe-projects/",'
       '  "login"       : "eugene"'
       '}' )

    print inp.action
    print inp.projects_path
    print inp.login

    defs.set_projects_path ( defs.test_projects_path() )

    result = get_list ( inp )
    print ( result.to_JSON() )

    sys.exit(0)
