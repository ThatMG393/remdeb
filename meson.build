project('remdeb', 'cpp', 'java',
  version: '1.0',
  default_options: ['cpp_std=c++17'])

cc = meson.get_compiler('cpp')

libdir = meson.current_source_dir() + './lib/'

dirs = ['src', 'net', 'jni', 'common']

src_files = [
  'src/main.cpp',

  'net/socket.h',
  'net/socket.cpp',

  'jni/loader.cpp',
  'jni/utils/android.h',

  'common/starter.h',
  'common/impl/debug_server.h',
  'common/impl/debug_server.cpp',
]

inc_dir = include_directories(dirs)
inc_dirs = [inc_dir]

build_type = get_option('build_type')
build_shared = get_option('build_shared') or build_type == 'android'

message('Here are the build configurations:')
message(f'  build_shared : @build_shared@')
message(f'  build_type   : @build_type@')

# pthread = cc.find_library('pthread')
jni = dependency('jni', required: build_shared and build_type == 'android')

default_deps = [jni]

if build_type == 'android'
  android_ndk = get_option('prefix')
  android_lib_dir = get_option('libdir').split(':')
  android_include_dir = get_option('includedir').split(':')

  android = cc.find_library('android', dirs: android_lib_dir)
  log = cc.find_library('log', dirs: android_lib_dir)

  android_deps = [android, log]
  android_deps += default_deps
endif

if build_shared
  if build_type == 'android'
    shared_library('remdeb', src_files,
      dependencies: android_deps, include_directories: inc_dirs)
  else
    shared_library('remdeb', src_files,
      dependencies: default_deps, include_directories: inc_dirs)
  endif
else
  executable('remdeb', src_files,
    dependencies: default_deps, include_directories: inc_dirs)
endif
