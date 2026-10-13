# VulkanGraphics

A Vulkan graphics engine written in modern C++20, built as a hands-on way to learn computer graphics. It uses [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp) (the C++ bindings for Vulkan) with [GLFW](https://www.glfw.org/) for windowing and input, and [Meson](https://mesonbuild.com/) as the build system.

> **Status:** Early-stage learning project, under active development.

<!-- Add a screenshot or GIF here once you have one:
![Screenshot](docs/screenshot.png)
-->

## Features

- Vulkan rendering via Vulkan-Hpp, using the dynamic dispatch loader
- Window and surface management with GLFW
- Swapchain and render target handling
- Graphics pipeline abstraction
- Per-frame resource management
- Resource layer for buffers, images, textures, meshes, materials, and shader programs
- Sampler library for reusing texture samplers
- Image loading with [stb_image](https://github.com/nothings/stb)

## Project Structure

```
VulkanGraphics/
├── include/        # Header files
├── shaders/        # GLSL shaders (built as part of the Meson build)
├── src/
│   ├── core/       # Window, Vulkan context, sampler library
│   ├── render/     # Renderer, swapchain, pipeline, render targets, frame resources, render objects
│   ├── resources/  # Buffer, Image, Texture, Mesh, Material, ShaderProgram, utilities
│   ├── Application.cpp
│   ├── main.cpp
│   └── stb_image.cpp
├── textures/       # Texture assets (symlinked into the build directory)
├── meson.build     # Build configuration
├── .clang-format   # Code style
└── .gitignore
```

## Requirements

- A C++20-capable compiler (GCC, Clang)
- [Meson](https://mesonbuild.com/) and [Ninja](https://ninja-build.org/)
- Vulkan SDK / development headers and loader
- GLFW 3
- A Vulkan-capable GPU with up-to-date drivers
- A GLSL-to-SPIR-V compiler such as `glslc` (part of the Vulkan SDK) for the shaders

> The build script creates the `textures` symlink with `ln`, so a Linux or macOS-style environment (or WSL on Windows) is expected.

### Installing dependencies

**Ubuntu / Debian**

```bash
sudo apt install build-essential meson ninja-build libvulkan-dev libglfw3-dev glslc vulkan-validationlayers
```

**Arch Linux**

```bash
sudo pacman -S base-devel meson ninja vulkan-headers vulkan-icd-loader glfw shaderc vulkan-validation-layers
```

**Fedora**

```bash
sudo dnf install meson ninja-build vulkan-headers vulkan-loader-devel glfw-devel glslc vulkan-validation-layers
```

## Building

```bash
git clone https://github.com/sumit-proj-hub/VulkanGraphics.git
cd VulkanGraphics

meson setup build
meson compile -C build
```

## Running

Run the executable from the build directory so it can find the symlinked `textures` folder and compiled shaders:

```bash
cd build
./Graphics
```

## Build Notes

- The project is compiled with `-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1`, so Vulkan functions are loaded dynamically at runtime.
- `-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1` is set, so Vulkan-Hpp structs are initialized with C++20 designated initializers.
- For a release build, configure with `meson setup build --buildtype=release`.

## Code Style

Formatting is defined in `.clang-format`. To format the sources:

```bash
clang-format -i src/**/*.cpp include/**/*.hpp
```

## Roadmap

<!-- Replace with your own plans, for example: -->
- [ ] Camera controls
- [ ] Lighting (Phong / PBR)
- [ ] Model loading
- [ ] Shadow mapping

## Resources

- [Vulkan Tutorial](https://vulkan-tutorial.com/)
- [Vulkan Specification](https://registry.khronos.org/vulkan/specs/latest/html/vkspec.html)
- [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp)
- [GLFW Documentation](https://www.glfw.org/documentation.html)

## License

<!-- Add a LICENSE file to the repo and name it here, e.g. MIT. -->
No license specified yet.
