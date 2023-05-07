
exe = executable('benchmark-polyvec', 'polyvec.cpp',
  # cpp_args: ['-fno-rtti'],
  include_directories: include_directories(
    'decoupled',
    'common',
  ),
  cpp_args: [
    # '-pg',
    # '-fconcepts-diagnostics-depth=3',
    # '-fanalyzer',
    # '-fsanitize=address',
  ],
  link_args: [
    # '-pg',
  ],
  dependencies: [
    dependency('fmt'),
    dep,
  ]
)

exe = executable('benchmark-polyarray', 'polyarray.cpp',
  include_directories: include_directories(
    'decoupled',
    'common',
  ),
  dependencies: [
    dependency('fmt'),
    dep,
  ]
)

exe = executable('benchmark-variant', 'variant.cpp',
  include_directories: include_directories(
    'decoupled',
    'common',
  ),
  dependencies: [
    dependency('fmt'),
    dep
  ]
)

exe = executable('benchmark-eventpp', 'eventpp.cpp',
  include_directories: include_directories(
    'eventpp/include',
    'inherited',
    'common',
  ),
  dependencies: [
    dependency('fmt'),
  ]
)

exe = executable('benchmark-inheritance', 'inheritance.cpp',
  include_directories: include_directories(
    'eventpp/include',
    'inherited',
    'common',
  ),
  dependencies: [
    dependency('fmt'),
    dep
  ]
)

