project('remdeb', 'cpp', 'java',
 version: '1.0',
 default_options: ['cpp_std=c++17'])

cc = meson.get_compiler('cpp')

sources = {
 'common': [
   'net/socket/socket.h',
   'net/socket/socket.cpp',
   'net/socket/socket_server.h',
   'net/socket/socket_server.cpp',
   'net/socket/packet_poller_socket.h',
   'net/socket/packet_poller_socket.cpp',
   'net/packet/packet.h',
   'impl/common/starter.h',
   'impl/server/debug_server.h',
   'impl/server/debug_server.cpp',
   'impl/client/debug_client.h',
   'impl/client/debug_client.cpp',
   'impl/common/packets.h',
 ],
 'android': ['jni/loader.cpp', 'jni/utils/android.h'],
 'standalone': ['src/main.cpp']
}

include_dirs = ['net', 'impl']

build_type = get_option('build_type')
build_executable = get_option('build_executable')

deps = [dependency('threads')]
link_args = []
src_files = sources['common']

if build_type == 'android'
 ndk_root = get_option('ndk_root')
 android_arch = get_option('android_arch')
 android_api = 21

 if ndk_root == '' or android_arch == ''
   error('Missing NDK options. Use: -Dndk_root=... -Dandroid_arch=...')
 endif

 sysroot = '@0@/toolchains/llvm/prebuilt/linux-@1@/sysroot'.format(ndk_root, host_machine.cpu_family())
 abi = android_arch == 'arm' ? 'armeabi-v7a' : 'arm64-v8a'
 
 lib_dirs = [
   '@0@/usr/lib/@1@-linux-androideabi'.format(sysroot, android_arch),
   '@0@/usr/lib/@1@-linux-androideabi/@2@'.format(sysroot, android_arch, android_api),
   '@0@/sources/cxx-stl/llvm-libc++/libs/@1@'.format(ndk_root, abi)
 ]

 deps += [
   dependency('jni'),
   cc.find_library('android', dirs: lib_dirs),
   cc.find_library('log', dirs: lib_dirs),
   cc.find_library('c', dirs: lib_dirs),
   cc.find_library('dl', dirs: lib_dirs),
   cc.find_library('m', dirs: lib_dirs),
   cc.find_library('c++_static', dirs: lib_dirs, static: true)
 ]

 link_args += [
   '-static-libstdc++',
   '-Wl,--whole-archive',
   '@0@/libc++_static.a'.format(lib_dirs[2]),
   '-Wl,--no-whole-archive'
 ]

 include_dirs += ['jni']
 src_files += sources['android']
endif

lib = shared_library('remdeb', src_files,
   include_directories: include_directories(include_dirs),
   dependencies: deps,
   link_args: link_args)
 
if build_executable
  executable(
    'remdeb_app',
    sources['standalone'],
    link_with: lib
  )
endif
