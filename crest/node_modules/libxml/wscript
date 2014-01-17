from os import path
from os import getcwd
from subprocess import call

cwd = getcwd()

def configure(conf):
  pass

def build(bld):
    call([path.join(cwd, 'clean.sh')])
    call([path.join(cwd, 'build-waf.sh')])
    call([path.join(cwd, 'copy.sh')])