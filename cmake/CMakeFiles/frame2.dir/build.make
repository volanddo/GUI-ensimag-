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
include CMakeFiles/frame2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/frame2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/frame2.dir/flags.make

CMakeFiles/frame2.dir/tests/frame_test2.c.o: CMakeFiles/frame2.dir/flags.make
CMakeFiles/frame2.dir/tests/frame_test2.c.o: ../tests/frame_test2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/frame2.dir/tests/frame_test2.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/frame2.dir/tests/frame_test2.c.o   -c /user/7/volandd/Documents/1A-S2/le-projet-c/tests/frame_test2.c

CMakeFiles/frame2.dir/tests/frame_test2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/frame2.dir/tests/frame_test2.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /user/7/volandd/Documents/1A-S2/le-projet-c/tests/frame_test2.c > CMakeFiles/frame2.dir/tests/frame_test2.c.i

CMakeFiles/frame2.dir/tests/frame_test2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/frame2.dir/tests/frame_test2.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /user/7/volandd/Documents/1A-S2/le-projet-c/tests/frame_test2.c -o CMakeFiles/frame2.dir/tests/frame_test2.c.s

CMakeFiles/frame2.dir/tests/frame_test2.c.o.requires:

.PHONY : CMakeFiles/frame2.dir/tests/frame_test2.c.o.requires

CMakeFiles/frame2.dir/tests/frame_test2.c.o.provides: CMakeFiles/frame2.dir/tests/frame_test2.c.o.requires
	$(MAKE) -f CMakeFiles/frame2.dir/build.make CMakeFiles/frame2.dir/tests/frame_test2.c.o.provides.build
.PHONY : CMakeFiles/frame2.dir/tests/frame_test2.c.o.provides

CMakeFiles/frame2.dir/tests/frame_test2.c.o.provides.build: CMakeFiles/frame2.dir/tests/frame_test2.c.o


# Object files for target frame2
frame2_OBJECTS = \
"CMakeFiles/frame2.dir/tests/frame_test2.c.o"

# External object files for target frame2
frame2_EXTERNAL_OBJECTS =

frame2: CMakeFiles/frame2.dir/tests/frame_test2.c.o
frame2: CMakeFiles/frame2.dir/build.make
frame2: libei.a
frame2: ../_x11/libeibase64.a
frame2: CMakeFiles/frame2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable frame2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/frame2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/frame2.dir/build: frame2

.PHONY : CMakeFiles/frame2.dir/build

CMakeFiles/frame2.dir/requires: CMakeFiles/frame2.dir/tests/frame_test2.c.o.requires

.PHONY : CMakeFiles/frame2.dir/requires

CMakeFiles/frame2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/frame2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/frame2.dir/clean

CMakeFiles/frame2.dir/depend:
	cd /user/7/volandd/Documents/1A-S2/le-projet-c/cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /user/7/volandd/Documents/1A-S2/le-projet-c /user/7/volandd/Documents/1A-S2/le-projet-c /user/7/volandd/Documents/1A-S2/le-projet-c/cmake /user/7/volandd/Documents/1A-S2/le-projet-c/cmake /user/7/volandd/Documents/1A-S2/le-projet-c/cmake/CMakeFiles/frame2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/frame2.dir/depend

