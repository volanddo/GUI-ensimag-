# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /bigsoft/Xilinx/SDK/2019.1/tps/lnx64/cmake-3.3.2/bin/cmake

# The command to remove a file.
RM = /bigsoft/Xilinx/SDK/2019.1/tps/lnx64/cmake-3.3.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /user/7/volandd/Documents/1A-S2/le-projet-c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /user/7/volandd/Documents/1A-S2/le-projet-c/cmake

# Include any dependencies generated for this target.
include CMakeFiles/two048.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/two048.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/two048.dir/flags.make

CMakeFiles/two048.dir/tests/two048.c.o: CMakeFiles/two048.dir/flags.make
CMakeFiles/two048.dir/tests/two048.c.o: ../tests/two048.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/two048.dir/tests/two048.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/two048.dir/tests/two048.c.o   -c /user/7/volandd/Documents/1A-S2/le-projet-c/tests/two048.c

CMakeFiles/two048.dir/tests/two048.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/two048.dir/tests/two048.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /user/7/volandd/Documents/1A-S2/le-projet-c/tests/two048.c > CMakeFiles/two048.dir/tests/two048.c.i

CMakeFiles/two048.dir/tests/two048.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/two048.dir/tests/two048.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /user/7/volandd/Documents/1A-S2/le-projet-c/tests/two048.c -o CMakeFiles/two048.dir/tests/two048.c.s

CMakeFiles/two048.dir/tests/two048.c.o.requires:

.PHONY : CMakeFiles/two048.dir/tests/two048.c.o.requires

CMakeFiles/two048.dir/tests/two048.c.o.provides: CMakeFiles/two048.dir/tests/two048.c.o.requires
	$(MAKE) -f CMakeFiles/two048.dir/build.make CMakeFiles/two048.dir/tests/two048.c.o.provides.build
.PHONY : CMakeFiles/two048.dir/tests/two048.c.o.provides

CMakeFiles/two048.dir/tests/two048.c.o.provides.build: CMakeFiles/two048.dir/tests/two048.c.o


# Object files for target two048
two048_OBJECTS = \
"CMakeFiles/two048.dir/tests/two048.c.o"

# External object files for target two048
two048_EXTERNAL_OBJECTS =

two048: CMakeFiles/two048.dir/tests/two048.c.o
two048: CMakeFiles/two048.dir/build.make
two048: libei.a
two048: ../_x11/libeibase64.a
two048: CMakeFiles/two048.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable two048"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/two048.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/two048.dir/build: two048

.PHONY : CMakeFiles/two048.dir/build

CMakeFiles/two048.dir/requires: CMakeFiles/two048.dir/tests/two048.c.o.requires

.PHONY : CMakeFiles/two048.dir/requires

CMakeFiles/two048.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/two048.dir/cmake_clean.cmake
.PHONY : CMakeFiles/two048.dir/clean

CMakeFiles/two048.dir/depend:
	cd /user/7/volandd/Documents/1A-S2/le-projet-c/cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /user/7/volandd/Documents/1A-S2/le-projet-c /user/7/volandd/Documents/1A-S2/le-projet-c /user/7/volandd/Documents/1A-S2/le-projet-c/cmake /user/7/volandd/Documents/1A-S2/le-projet-c/cmake /user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles/two048.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/two048.dir/depend

