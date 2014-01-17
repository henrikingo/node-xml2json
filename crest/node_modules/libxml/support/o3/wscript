srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):  	
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')   
  conf.add_os_flags('LDFLAGS','LINKFLAGS')
  conf.env.append_value('CCFLAGS', ['-O3', '-msse2', '-ffast-math', '-fno-strict-aliasing'])
  conf.env.append_value('CXXFLAGS', ['-O3', '-msse2', '-ffast-math', '-fno-strict-aliasing'])

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')  
  obj.target = 'o3'
  obj.source = 'hosts/node-o3/sh_node.cc hosts/node-o3/sh_node_libs.cc'
  
  obj.includes = """
    include
    hosts
    modules
    deps
  """
  
  obj.lib = 'xml2'
