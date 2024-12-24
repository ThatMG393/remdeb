project('remdeb', 'cpp', 'java',
  version: '1.0',
  default_options: ['cpp_std=c++17'])

dirs = ['src', 'net', 'jni', 'common']

src_files = [
  'src/main.cpp',

  'net/packet/packet.hpp',
  'net/socket/socket_server.cpp',

  'jni/loader.cpp',
  'jni/utils/android.cpp',

  'common/debug_server.cpp'
]

inc_dirs = include_directories(dirs)

build_type = get_option('build_type')
build_shared = get_option('build_shared') or build_type == 'android'

message('Here are the build configurations:')
message(f'  build_shared : @build_shared@')
message(f'  build_type   : @build_type@')

deps = [
  dependency('log', required: (get_option('build_type') == 'android')),
  dependency('jni', required: build_shared)
]

message('Checking for dependencies...')
foreach dep : deps
  if dep.found()
   message(dep.name() + ' found!')
  else
    message(dep.name() + ' not found!')
  endif
endforeach

if build_shared
  if build_type == 'android'
    shared_library('remdeb', src_files,
      dependencies: deps, include_directories: inc_dirs, link_args: ['-static-libstdc++'])
  else
    shared_library('remdeb', src_files,
      dependencies: deps, include_directories: inc_dirs)
  endif
else
  executable('remdeb', src_files,
    dependencies: deps, include_directories: inc_dirs)
endif