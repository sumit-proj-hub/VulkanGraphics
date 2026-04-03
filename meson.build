project('Graphics', 'cpp',
  default_options : ['cpp_std=c++20']
)

inc = include_directories('include')

vulkan_dep = dependency('vulkan', required: true)
glfw_dep = dependency('glfw3', required: true)

subdir('shaders')

meson.add_postconf_script('ln', '-sf', '../textures', 'textures')

executable('Graphics',
  [
    'src/main.cpp'
  ],
  include_directories : inc,
  dependencies : [vulkan_dep, glfw_dep],
  cpp_args : [
    '-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1',
    '-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1'
  ]
)
