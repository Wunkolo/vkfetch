cmake_minimum_required( VERSION 3.30.0 )
project( vkfetch CXX )

set( CMAKE_COLOR_MAKEFILE ON )

# Generate 'compile_commands.json' for clangd
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Check for LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)

if( MSVC )
elseif( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
	# Force colored diagnostic messages in Ninja's output
	if( CMAKE_GENERATOR STREQUAL "Ninja" )
		add_compile_options( -fdiagnostics-color=always )
	endif()
endif()

#vulkan
find_package( Vulkan REQUIRED )

#fmt
find_package(fmt QUIET)
if( fmt_FOUND )
else()
	add_subdirectory( externals/fmt EXCLUDE_FROM_ALL )
endif()

add_executable(
	vkfetch
	source/main.cpp
	source/Format.cpp
	source/VendorArt.cpp
	source/VulkanUtil.cpp
)

### Standard
set_target_properties( vkfetch PROPERTIES CXX_STANDARD 23 )
set_target_properties( vkfetch PROPERTIES CXX_STANDARD_REQUIRED ON )
set_target_properties( vkfetch PROPERTIES CXX_EXTENSIONS OFF )

target_include_directories(
	vkfetch
	PRIVATE
	include
)
target_link_libraries(
	vkfetch
	PRIVATE
	Vulkan::Vulkan
	fmt::fmt
)

if( LTO_SUPPORTED )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET vkfetch PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${LTO_ERROR}>")
endif()