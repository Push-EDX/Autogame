# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/blipi/Autogame

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/blipi/Autogame/build

# Include any dependencies generated for this target.
include dep/tinyxml2/CMakeFiles/tinyxml2static.dir/depend.make

# Include the progress variables for this target.
include dep/tinyxml2/CMakeFiles/tinyxml2static.dir/progress.make

# Include the compile flags for this target's objects.
include dep/tinyxml2/CMakeFiles/tinyxml2static.dir/flags.make

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/flags.make
dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o: ../dep/tinyxml2/tinyxml2.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/blipi/Autogame/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o"
	cd /home/blipi/Autogame/build/dep/tinyxml2 && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o -c /home/blipi/Autogame/dep/tinyxml2/tinyxml2.cpp

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.i"
	cd /home/blipi/Autogame/build/dep/tinyxml2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/blipi/Autogame/dep/tinyxml2/tinyxml2.cpp > CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.i

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.s"
	cd /home/blipi/Autogame/build/dep/tinyxml2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/blipi/Autogame/dep/tinyxml2/tinyxml2.cpp -o CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.s

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.requires:
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.requires

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.provides: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.requires
	$(MAKE) -f dep/tinyxml2/CMakeFiles/tinyxml2static.dir/build.make dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.provides.build
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.provides

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.provides.build: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o

# Object files for target tinyxml2static
tinyxml2static_OBJECTS = \
"CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o"

# External object files for target tinyxml2static
tinyxml2static_EXTERNAL_OBJECTS =

dep/tinyxml2/libtinyxml2.a: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o
dep/tinyxml2/libtinyxml2.a: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/build.make
dep/tinyxml2/libtinyxml2.a: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libtinyxml2.a"
	cd /home/blipi/Autogame/build/dep/tinyxml2 && $(CMAKE_COMMAND) -P CMakeFiles/tinyxml2static.dir/cmake_clean_target.cmake
	cd /home/blipi/Autogame/build/dep/tinyxml2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyxml2static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
dep/tinyxml2/CMakeFiles/tinyxml2static.dir/build: dep/tinyxml2/libtinyxml2.a
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/build

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/requires: dep/tinyxml2/CMakeFiles/tinyxml2static.dir/tinyxml2.cpp.o.requires
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/requires

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/clean:
	cd /home/blipi/Autogame/build/dep/tinyxml2 && $(CMAKE_COMMAND) -P CMakeFiles/tinyxml2static.dir/cmake_clean.cmake
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/clean

dep/tinyxml2/CMakeFiles/tinyxml2static.dir/depend:
	cd /home/blipi/Autogame/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/blipi/Autogame /home/blipi/Autogame/dep/tinyxml2 /home/blipi/Autogame/build /home/blipi/Autogame/build/dep/tinyxml2 /home/blipi/Autogame/build/dep/tinyxml2/CMakeFiles/tinyxml2static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dep/tinyxml2/CMakeFiles/tinyxml2static.dir/depend

