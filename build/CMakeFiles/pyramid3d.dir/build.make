# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vladimirmarkov/Gkomp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vladimirmarkov/Gkomp/build

# Include any dependencies generated for this target.
include CMakeFiles/pyramid3d.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/pyramid3d.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pyramid3d.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pyramid3d.dir/flags.make

CMakeFiles/pyramid3d.dir/codegen:
.PHONY : CMakeFiles/pyramid3d.dir/codegen

CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o: CMakeFiles/pyramid3d.dir/flags.make
CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o: /Users/vladimirmarkov/Gkomp/pyramid3d.cpp
CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o: CMakeFiles/pyramid3d.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/vladimirmarkov/Gkomp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o -MF CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o.d -o CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o -c /Users/vladimirmarkov/Gkomp/pyramid3d.cpp

CMakeFiles/pyramid3d.dir/pyramid3d.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/pyramid3d.dir/pyramid3d.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vladimirmarkov/Gkomp/pyramid3d.cpp > CMakeFiles/pyramid3d.dir/pyramid3d.cpp.i

CMakeFiles/pyramid3d.dir/pyramid3d.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/pyramid3d.dir/pyramid3d.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vladimirmarkov/Gkomp/pyramid3d.cpp -o CMakeFiles/pyramid3d.dir/pyramid3d.cpp.s

# Object files for target pyramid3d
pyramid3d_OBJECTS = \
"CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o"

# External object files for target pyramid3d
pyramid3d_EXTERNAL_OBJECTS =

pyramid3d: CMakeFiles/pyramid3d.dir/pyramid3d.cpp.o
pyramid3d: CMakeFiles/pyramid3d.dir/build.make
pyramid3d: /Library/Developer/CommandLineTools/SDKs/MacOSX14.0.sdk/System/Library/Frameworks/OpenGL.framework
pyramid3d: /opt/homebrew/lib/libGLEW.2.2.0.dylib
pyramid3d: /opt/homebrew/lib/libsfml-graphics.2.6.2.dylib
pyramid3d: /opt/homebrew/lib/libsfml-window.2.6.2.dylib
pyramid3d: /opt/homebrew/lib/libsfml-system.2.6.2.dylib
pyramid3d: CMakeFiles/pyramid3d.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/vladimirmarkov/Gkomp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable pyramid3d"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pyramid3d.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pyramid3d.dir/build: pyramid3d
.PHONY : CMakeFiles/pyramid3d.dir/build

CMakeFiles/pyramid3d.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pyramid3d.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pyramid3d.dir/clean

CMakeFiles/pyramid3d.dir/depend:
	cd /Users/vladimirmarkov/Gkomp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vladimirmarkov/Gkomp /Users/vladimirmarkov/Gkomp /Users/vladimirmarkov/Gkomp/build /Users/vladimirmarkov/Gkomp/build /Users/vladimirmarkov/Gkomp/build/CMakeFiles/pyramid3d.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/pyramid3d.dir/depend
