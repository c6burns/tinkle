cmake_minimum_required(VERSION 3.0.2)
project(tinkle LANGUAGES CXX)

set(TNKL_TARGET_LINK_LIBS "")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	list(APPEND HB_TARGET_COMPILE_DEFS -DHB_CMAKE_BUILD_DEBUG)
endif()

if(NOT WIN32)
	set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pie")
else()
	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
	)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	
	if(MSVC)
		add_custom_target(CMake SOURCES CMakeLists.txt)
		list(APPEND HB_TARGET_COMPILE_DEFS -D_CRT_SECURE_NO_WARNINGS)
	endif()
endif()


#
# build tinkle
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/PluginAPI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/tnkl)
set(TNKL_LIBRARY_SRCS
	src/plugin.cpp
	include/tnkl/plugin.h
)
add_library(tnkl ${TNKL_LIBRARY_SRCS})
target_link_libraries(tnkl ${TNKL_TARGET_LINK_LIBS})
target_compile_definitions(tnkl PRIVATE ${TNKL_TARGET_COMPILE_DEFS})
list(APPEND TNKL_TARGET_LINK_LIBS tnkl)

add_custom_command(
	TARGET tnkl
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:tnkl>
		${CMAKE_CURRENT_SOURCE_DIR}/UnityProject/Assets/Plugins/x86_64/$<TARGET_FILE_NAME:tnkl>
)
