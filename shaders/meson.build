slangc = find_program('slangc', required: true)

shader_sources = [
  'shader.slang'
]

compiled_shaders = []

foreach s : shader_sources
  name = s.split('.')[0]

  compiled_shaders += custom_target(
    name + '_spv',
    input : s,
    output : name + '.spv',
    command : [
      slangc, '@INPUT@',
      '-target', 'spirv',
      '-profile', 'spirv_1_4',
      '-emit-spirv-directly',
      '-fvk-use-entrypoint-name',
      '-entry', 'vertMain',
      '-entry', 'fragMain',
      '-o', '@OUTPUT@'
    ],
    build_by_default : true
  )
endforeach
