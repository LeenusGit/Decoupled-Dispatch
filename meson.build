project('observer', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++20',
  ])

exe = executable('observer', 'observer.cpp',
  # cpp_args: ['-fno-rtti'],
  install : true,
  dependencies: dependency('fmt')
  )

test('basic', exe)
