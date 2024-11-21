#! /usr/bin/env python
from __future__ import print_function

"""
help       -> scons -h
compile    -> scons
clean      -> scons -c
install    -> scons install
uninstall  -> scons -c install
configure  -> scons configure prefix=/tmp/ita debug=full extraincludes=/usr/local/include:/tmp/include prefix=/usr/local
"""

import sys, os, platform, subprocess
import re

import tcolors
tcolors.initColors()    # initialize colors
from tcolors import *   # import variables

#testcolors()

import funcs

###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################
import SCons.Util

env = Environment(ENV = os.environ, toolpath=['./', './tools/', os.getcwd()+ '/tools/'])
env.Replace(CCFLAGS = [])

env['ARGS'] = funcs.makeHashTable(sys.argv)
env['HELP'] = 0
if '--help' in sys.argv or '-h' in sys.argv or 'help' in sys.argv:
  env['HELP'] = 1

# platform detection
sys_machine = platform.machine()    # x86_64
sys_arch = platform.architecture()  # ('64bit', 'ELF')
sys_release = platform.release()    # '3.16.0-4-amd64'
sys_system = platform.system()      # 'Linux'

print ('Python version                    :  '+BOLD + sys.version.replace("\n", " ") +NORMAL)
print ('Operating system                  :  '+GREEN + sys_system +NORMAL)
print ('System architecture               :  '+BOLD + sys_machine +NORMAL)
print ('Platform architecture             :  '+RED + ' '.join(sys_arch) +NORMAL)
print ('Platform release                  :  '+BOLD + sys_release +NORMAL)

str_platform = sys_system + '-' + sys_machine

## Global cache directory
## Put all project files in it so a rm -rf cache will clean up the config
if not env.__contains__('CACHEDIR'):
    env['CACHEDIR'] = os.getcwd() + '/cache-' + str_platform
if not os.path.isdir(env['CACHEDIR']):
    os.mkdir(env['CACHEDIR'])
## Avoid spreading .sconsign files everywhere - keep this line
env.SConsignFile(env['CACHEDIR']+'/scons_signatures')

cachefile=env['CACHEDIR']+'/cache.py'
# variables stored in cache file
vars = Variables(cachefile)
vars.Add( 'CACHEDIR', 'Cache directory')
vars.Add( 'PREFIX', 'Prefix for installation' )
vars.Add( 'DEBUG', 'Enable debug' )
vars.Add( 'ISCONFIGURED', 'Project configured' )
vars.Add( 'PKGCONFIG', 'pkg-config found' )
vars.Add( 'CPPPATH', 'Include directories' )
vars.Add( 'CPPDEFINES', 'Preprocessor definitions' )
vars.Add( 'CXXFLAGS', 'C++ compiler options' )
vars.Add( 'LIBPATH', 'library directories' )
vars.Add( 'LIBS', 'Libraries' )
vars.Add( 'LINKFLAGS', 'Linker flags')
vars.Add( 'SYSPLATFORM', 'System platform')
vars.Add( 'CCARCH', 'Compiler architecture')
vars.Add( 'CCPlatform', 'Compiler platform')
vars.Update(env)

Help(vars.GenerateHelpText(env))

# to avoid an error message 'how to make target configure... ?'
env.Alias('configure', None)
env.Alias('help', None)

if env['HELP']:
    funcs.display_help()
    sys.exit()

if 'configure' in sys.argv or 'conf' in sys.argv:
    env['_CONFIGURE']=1
else:
    env['_CONFIGURE']=0

#check for colorgcc
colorgcc = env.WhereIs("colorgcc")
if colorgcc and len(colorgcc) > 8:
    colorgcc = colorgcc.strip()
    env.Replace(CC = 'colorgcc')
    env.Replace(CXX = 'colorgcc')

# configure the environment if needed
if not env.__contains__('SYSPLATFORM'):
    env['SYSPLATFORM'] = '-'

if env['SYSPLATFORM'] != str_platform:
    env['SYSPLATFORM'] = str_platform
    env['_CONFIGURE']=1

if env['_CONFIGURE'] or not env.__contains__('ISCONFIGURED'):
    funcs.configure(env)
    vars.Save(cachefile, env)   # save the options in the cache

if not env['DEBUG']:
    env['CXXCOMSTR'] = BOLD+"Compiling"+NORMAL+" $SOURCE"
    env['LINKCOMSTR'] = "Linking $TARGET"

if sys.platform.startswith('win'):
    env.Tool('mingw')

build_dir = '#build-' + str_platform

Export('env')
SConscript('src/SConscript', variant_dir=build_dir, duplicate=0)
