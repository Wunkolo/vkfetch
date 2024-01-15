cmake_minimum_required( VERSION 3.21.0 )
project( vkfetch CXX )

### Standard
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

set( CMAKE_COLOR_MAKEFILE ON )

# Generate 'compile_commands.json' for clang_complete
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

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
	source/VulkanUtil.cpp
)

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
