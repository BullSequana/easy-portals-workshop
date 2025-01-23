project('easy-portals', 'c', version: '0.1')

cc = meson.get_compiler('c')
libportals_dep = cc.find_library('libportals')
libpthread_dep = cc.find_library('libpthread')

ezportals_lib = library(
  'ezportals',
  'ezportals.c',
  dependencies: [libportals_dep, libpthread_dep],
)

libezportals = declare_dependency(
  link_with: ezportals_lib,
)

ezportals_hdrs = include_directories('.', 'utils')

subdir('utils')
subdir('examples')
subdir('challenge1')
subdir('challenge2')
subdir('challenge3')
