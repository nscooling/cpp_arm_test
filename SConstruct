# ----------------------------------------------------------------------------
#
Help("""
Type: 'scons' to build the project
      'scons --rtos' to build with the RTOS / FeabhOS enabled
      'scons -c' to clean
""")

# ----------------------------------------------------------------------------
import os

# Project name - this is the name of the 
# output (.hex/.elf) file
#
proj_name = 'Application'

# get string of absolute of cwd
project_root = Dir('.').srcnode().abspath

# ----------------------------------------------------------------------------
# Output (variant) directory
#
vdir = 'build/debug/'

# ----------------------------------------------------------------------------
# build with `scons --rtos` for feabhOS.
#
AddOption(
    '--rtos',
    action  = 'store_true',
    help    = 'rtos build',
    default = False)

# ----------------------------------------------------------------------------
# Cross-compiler set-up.  This configuration assumes the cross-compiler
# is in the PATH.  If not, you will have to hard-code the compiler path; for
# example:
# 
# import os
# path = [ '/bin', '/usr/bin', '/usr/local/gcc-arm-none-eabi-7-2017-q4-major/bin' ]
# env = Environment(ENV = { 'PATH' : path })
# 
import os
env = Environment( ENV = os.environ )

env.Replace(AS         = "arm-none-eabi-as")
env.Replace(AR         = "arm-none-eabi-ar")
env.Replace(CC         = "arm-none-eabi-gcc")
env.Replace(CXX        = "arm-none-eabi-g++")
env.Replace(LINK       = "arm-none-eabi-g++")     # predefined is 'arm-none-eabi-gcc'
env.Replace(OBJCOPY    = "arm-none-eabi-objcopy")
env.Replace(PROGSUFFIX = ".elf")
env.Replace(RANLIB     = "arm-none-eabi-ranlib")
env.Replace(SIZE       = "arm-none-eabi-size")

# ----------------------------------------------------------------------------
# Processor-specific compiler flags
#
cortex_m4_flags = [
    '-mcpu=cortex-m4',
    '-mthumb',
    '-mfloat-abi=soft',
    ]

# ----------------------------------------------------------------------------
# Global macro definitions
#

env.Append(CPPDEFINES = [
    'DEBUG',
    'USE_FULL_ASSERT',
    'STM32F407xx',
    'TRACE',
    # 'OS_USE_TRACE_SEMIHOSTING_DEBUG',  semi-hosting options mutually exclusive
    'OS_USE_TRACE_SEMIHOSTING_STDOUT',
    'OS_USE_SEMIHOSTING',
    'TRACE_ENABLED'
])

# preprocessor directives for conditional compilation 
if GetOption('rtos'):
    env.Append(CPPDEFINES = ['RTOS'])

# ----------------------------------------------------------------------------
# C language-specific compiler flags
# 
env.Append(CFLAGS = [
    '-std=gnu11',
])

# ----------------------------------------------------------------------------
# C++ language-specific compiler flags
#
env.Append(CXXFLAGS = [
    '-std=gnu++17',
    '-fabi-version=0',
    '-fno-use-cxa-atexit',
    '-fno-threadsafe-statics',
])

# ----------------------------------------------------------------------------
# General compilation flags
#
env.Append(CCFLAGS = [
    '-Og',
    '-fmessage-length=0',
    '-fsigned-char',
    '-ffunction-sections',
    '-fdata-sections',
    # '-ffreestanding',
    '-fno-move-loop-invariants',
    '-fstack-usage',
    # '-fdebug-prefix-map="%s"='%(project_root),
    '-Wall',
    '-Wextra',
    '-Wconversion',
    '-Wsign-conversion',
    # '-Werror',
    '-g3',
    '-MMD',
    '-MP'] + cortex_m4_flags)


# ----------------------------------------------------------------------------
# Linker flags
#

env.Append(LINKFLAGS = cortex_m4_flags + [
    '-Tldscripts/libs.ld',
    '-Tldscripts/mem.ld',
    '-Tldscripts/sections.ld',
    '-nostartfiles',
    '-Xlinker',
    '--gc-sections',
    '-Xlinker',                 # -Xlinker option
    '--print-memory-usage',     # Print used size, total size and used size of memory regions created with the MEMORY command.
    '-u', '_printf_float',
    '-u', '_scanf_float',
    '-Wl,-Map,"%s.map"'%(vdir + proj_name),   # put map file in build directory
    '--specs=nano.specs',
    '--specs=rdimon.specs',                 # needed for semi-hosting
    '-Wl,--build-id',
    ])


# ----------------------------------------------------------------------------
# Construct the source code directories and include
# paths for all sub-directories
#
sources  = []
includes = []

SConscript(dirs = 'drivers', exports = ['env', 'sources', 'includes'])
SConscript(dirs = 'system',  exports = ['env', 'sources', 'includes'])

if GetOption('rtos'):
    SConscript(dirs = 'middleware', exports = ['env', 'sources', 'includes'])

env['CPPPATH'] = includes

# -----------------------------------------------------------------------------------
# Build everything
#
env.VariantDir(vdir, 'src', duplicate=0)
target_name = vdir + proj_name + '.elf'

# use this if not using srxc/SConscript
prg = env.Program(target=target_name, source=sources+Glob(vdir+'/*.cpp'))

# use this if using src/SConscript
# of env.program() line above
# SConscript(dirs = 'src', variant_dir = vdir, 
#            exports = ['env', 'sources', 'proj_name', 'vdir'], duplicate = 0)
#

# ----------------------------------------------------------------------------
# Hex file builder
# Invokes: Cross ARM GNU Create Flash Image
# 
def hex_generator(source, target, env, for_signature):
    return '$OBJCOPY -O ihex %s %s'%(source[0], target[0])

env.Append(BUILDERS = {
    'Obj_copy': Builder(
        generator = hex_generator,
        suffix = '.hex',
        src_suffix = '.elf'
    )
})

# ----------------------------------------------------------------------------
# File size builder
# Invokes: Cross ARM GNU Print Size
# 
def size_generator(source, target, env, for_signature):
    return '$SIZE --format=berkeley %s > %s.bsz'%(source[0],vdir + proj_name)

def sysv_size_generator(source, target, env, for_signature):
    return '$SIZE --format=SysV -x %s > %s.ssz'%(source[0],vdir + proj_name)

env.Append(BUILDERS = {
    'Size': Builder(
        generator = size_generator,
        src_suffix = '.elf'
    )
})

env.Append(BUILDERS = {
    'SysV_Size': Builder(
        generator = sysv_size_generator,
        src_suffix = '.elf'
    )
})

env.Obj_copy(prg)
env.Size(prg)
# env.SysV_Size(prg)
