# vkfetch [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE) [![Build](https://github.com/Wunkolo/vkfetch/actions/workflows/cmake.yml/badge.svg)](https://github.com/Wunkolo/vkfetch/actions/workflows/cmake.yml)

`vkfetch` is a fetch-program that displays basic information about your vulkan-compatible graphic card(s)!

`vkfetch` will also display some vendor-specific information about your GPU, when available.

![Intel](images/intel.png)
![AMD](images/amd.png)
![Nvidia](images/nvidia.png)
![Apple](images/apple.png)
![Dozen](images/dozen.png)

# Building

`vkfetch` will require the vulkan SDK headers for your platform to build. Check out [lunarg](https://vulkan.lunarg.com/) to get started or use your linux distro's package manager!

This is a CMake project, so a typical [Cmake out-of-source build procedure](http://preshing.com/20170511/how-to-build-a-cmake-based-project/#running-cmake-from-the-command-line) will get you going on both Windows and Linux:

```
git clone git@github.com:Wunkolo/vkfetch.git
cd vkfetch
mkdir build
cd build
cmake ..
cmake --build .
./vkfetch
```
