import sys, os, platform, subprocess
import re
from tcolors import *

def display_help():
    print()
    print(BOLD + "  Configure options")
    print("--------------------------------------------------" + NORMAL)
    print(BOLD + "* prefix        " + NORMAL + ": installation prefix (/usr/local)")
    print(BOLD + "* debug         " + NORMAL + ": enable debug (debug=1 or debug=full)" + NORMAL)
    print()
    return


import re
def makeHashTable(args):
    table = { }
    for arg in args:
        if len(arg) > 1:
            lst=arg.split('=')
            if len(lst) < 2:
                continue
            key=lst[0]
            value=lst[1]
            if len(key) > 0 and len(value) >0:
                table[key] = value
    return table


def check_pkgconfig(env):
    print("Checking for pkg-config           : ", end = '')
    pkg_config = env.WhereIs("pkg-config")
    if not pkg_config:
        pkg_config = ""
    if len(pkg_config):
        pkg_config = pkg_config.strip()
        print(GREEN+pkg_config+NORMAL, end = '')
        pkgcver = os.popen("pkg-config --version").read().strip()
        print(BOLD+' version '+pkgcver+NORMAL)
        env['PKGCONFIG'] = 1
        return 1
    else:
        print(RED+'not found'+NORMAL)
        env['PKGCONFIG'] = 0
        return 0

def pkgconfig_checklib(env, lib):
    if not env['PKGCONFIG']:
        return false
    print("Checking for " + lib + " (pkg-config): ", end = '')
    if subprocess.call('pkg-config --exists ' + lib, shell=1) != 0:
        print(RED + 'no' + NORMAL)
        return false
    ver = subprocess.output('pkg-config --modversion ' + lib)
    print(GREEN + 'yes' + NORMAL + ' version ' + ver)
    return true

def check_library(conf, lib, hdr):
    ret = 1
    if not conf.CheckLib(lib):
        ret = 0
    if not conf.CheckHeader(hdr):
        ret = 0
    return ret

def configure(env):
    # unset existing variables
    if env.__contains__('PREFIX'):
        env.__delitem__('PREFIX')
    if env.__contains__('DEBUG'):
        env.__delitem__('DEBUG')
    if env.__contains__('ISCONFIGURED'):
        env.__delitem__('ISCONFIGURED')
    if env.__contains__('PKGCONFIG'):
        env.__delitem__('PKGCONFIG')
    if env.__contains__('MINGWBUILD'):
        env.__delitem__('MINGWBUILD')
    if env.__contains__('CPPPATH'):
        env.__delitem__('CPPPATH')
    if env.__contains__('CPPDEFINES'):
        env.__delitem__('CPPDEFINES')
    if env.__contains__('CXXFLAGS'):
        env.__delitem__('CXXFLAGS')
    if env.__contains__('LIBPATH'):
        env.__delitem__('LIBPATH')
    if env.__contains__('LIBS'):
        env.__delitem__('LIBS')
    if env.__contains__('LINKFLAGS'):
        env.__delitem__('LINKFLAGS')
    
    print()
    print ('Enable debug                      : ', end = '')
    if env['ARGS'].get('debug', 1):
        debuglevel = env['ARGS'].get('debug', "full")
        print (GREEN + 'yes ' + NORMAL + debuglevel)
        env.Append(CPPDEFINES = ['DEBUG'])
        env['DEBUG'] = 1
        if (debuglevel == "full"):
            env.Append(CXXFLAGS   = ['-g3'])
        else:
            env.Append(CXXFLAGS   = ['-g'])
    else:
        print ('no ')
        env.Append(CXXFLAGS = ['-O2'] )
        env.Append(CPPDEFINES = ['NDEBUG', 'NO_DEBUG'])
        env['DEBUG'] = 0
    
    check_pkgconfig(env)

    # User-specified prefix
    if env['ARGS'].get('prefix', None):
        env['PREFIX'] = env['ARGS'].get('prefix', None)
    else:
        if sys.platform.startswith('win'):
            env['PREFIX'] = '../install'
        else:
            env['PREFIX'] = '/usr/local'
    print ('installation prefix               :  '+BOLD + env['PREFIX'] +NORMAL)
    env.Append(CPPPATH = ['.', '../include'])
    env.Append(LIBPATH = ['.', '../libs'])
    env['ISCONFIGURED']=1


def install(env, binary_files):
  if not os.path.isdir(env['PREFIX']):
    os.mkdir(env['PREFIX'])

  prefix = env['PREFIX']

  if sys.platform.startswith('win'):
    binary = env.Install(prefix, binary_files)
    env.Alias('install', binary)
  else:
    bin_path   = prefix + "/bin"
    icon_path = prefix + "/share/pixmaps"
    dotdesktop_path = prefix + '/share/applications'

    binary = env.Install(bin_path, binary_files)
    #dotdesktop = env.Install(dotdesktop_path, dotdesktop_files)
    #icon = env.Install(icon_path, icon_files)

    env.Alias('install', binary)
    env.Alias('install', dotdesktop)
    env.Alias('install', icon)
