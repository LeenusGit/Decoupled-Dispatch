project('Decoupled Dispatch', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++20',
  ])

inc = include_directories('src')

dep = declare_dependency(
  include_directories: inc
)

subdir('example' / 'simple')
subdir('example' / 'readme')
subdir('test')
