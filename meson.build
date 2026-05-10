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
   'src/main.cpp',
   'src/resources/Image.cpp',
   'src/resources/Buffer.cpp',
   'src/resources/ResourceUtils.cpp',
   'src/resources/Texture.cpp',
   'src/resources/ShaderProgram.cpp',
   'src/resources/Mesh.cpp',
   'src/resources/Material.cpp',
   'src/Application.cpp',
   'src/stb_image.cpp',
   'src/render/FrameResources.cpp',
   'src/render/Renderer.cpp',
   'src/render/RenderTargets.cpp',
   'src/render/Pipeline.cpp',
   'src/render/Swapchain.cpp',
   'src/render/RenderObject.cpp',
   'src/core/SamplerLibrary.cpp',
   'src/core/Window.cpp',
   'src/core/VulkanContext.cpp'
 ],
  include_directories : inc,
  dependencies : [vulkan_dep, glfw_dep],
  cpp_args : [
    '-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1',
    '-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1'
  ]
)
