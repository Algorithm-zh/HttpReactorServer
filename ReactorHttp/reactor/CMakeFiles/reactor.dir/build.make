# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zxe/Documents/HttpReactorServer/ReactorHttp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zxe/Documents/HttpReactorServer/ReactorHttp

# Include any dependencies generated for this target.
include reactor/CMakeFiles/reactor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include reactor/CMakeFiles/reactor.dir/compiler_depend.make

# Include the progress variables for this target.
include reactor/CMakeFiles/reactor.dir/progress.make

# Include the compile flags for this target's objects.
include reactor/CMakeFiles/reactor.dir/flags.make

reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/flags.make
reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o: reactor/Dispatcher.cpp
reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o -MF CMakeFiles/reactor.dir/Dispatcher.cpp.o.d -o CMakeFiles/reactor.dir/Dispatcher.cpp.o -c /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/Dispatcher.cpp

reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reactor.dir/Dispatcher.cpp.i"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/Dispatcher.cpp > CMakeFiles/reactor.dir/Dispatcher.cpp.i

reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reactor.dir/Dispatcher.cpp.s"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/Dispatcher.cpp -o CMakeFiles/reactor.dir/Dispatcher.cpp.s

reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/flags.make
reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o: reactor/EpollDispatcher.cpp
reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o -MF CMakeFiles/reactor.dir/EpollDispatcher.cpp.o.d -o CMakeFiles/reactor.dir/EpollDispatcher.cpp.o -c /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EpollDispatcher.cpp

reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reactor.dir/EpollDispatcher.cpp.i"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EpollDispatcher.cpp > CMakeFiles/reactor.dir/EpollDispatcher.cpp.i

reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reactor.dir/EpollDispatcher.cpp.s"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EpollDispatcher.cpp -o CMakeFiles/reactor.dir/EpollDispatcher.cpp.s

reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o: reactor/CMakeFiles/reactor.dir/flags.make
reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o: reactor/EventLoop.cpp
reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o: reactor/CMakeFiles/reactor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o -MF CMakeFiles/reactor.dir/EventLoop.cpp.o.d -o CMakeFiles/reactor.dir/EventLoop.cpp.o -c /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EventLoop.cpp

reactor/CMakeFiles/reactor.dir/EventLoop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reactor.dir/EventLoop.cpp.i"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EventLoop.cpp > CMakeFiles/reactor.dir/EventLoop.cpp.i

reactor/CMakeFiles/reactor.dir/EventLoop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reactor.dir/EventLoop.cpp.s"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/EventLoop.cpp -o CMakeFiles/reactor.dir/EventLoop.cpp.s

reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/flags.make
reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o: reactor/PollDispatcher.cpp
reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o -MF CMakeFiles/reactor.dir/PollDispatcher.cpp.o.d -o CMakeFiles/reactor.dir/PollDispatcher.cpp.o -c /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/PollDispatcher.cpp

reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reactor.dir/PollDispatcher.cpp.i"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/PollDispatcher.cpp > CMakeFiles/reactor.dir/PollDispatcher.cpp.i

reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reactor.dir/PollDispatcher.cpp.s"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/PollDispatcher.cpp -o CMakeFiles/reactor.dir/PollDispatcher.cpp.s

reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/flags.make
reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o: reactor/SelectDispatcher.cpp
reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o: reactor/CMakeFiles/reactor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o -MF CMakeFiles/reactor.dir/SelectDispatcher.cpp.o.d -o CMakeFiles/reactor.dir/SelectDispatcher.cpp.o -c /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/SelectDispatcher.cpp

reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reactor.dir/SelectDispatcher.cpp.i"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/SelectDispatcher.cpp > CMakeFiles/reactor.dir/SelectDispatcher.cpp.i

reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reactor.dir/SelectDispatcher.cpp.s"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/SelectDispatcher.cpp -o CMakeFiles/reactor.dir/SelectDispatcher.cpp.s

# Object files for target reactor
reactor_OBJECTS = \
"CMakeFiles/reactor.dir/Dispatcher.cpp.o" \
"CMakeFiles/reactor.dir/EpollDispatcher.cpp.o" \
"CMakeFiles/reactor.dir/EventLoop.cpp.o" \
"CMakeFiles/reactor.dir/PollDispatcher.cpp.o" \
"CMakeFiles/reactor.dir/SelectDispatcher.cpp.o"

# External object files for target reactor
reactor_EXTERNAL_OBJECTS =

reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/Dispatcher.cpp.o
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/EpollDispatcher.cpp.o
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/EventLoop.cpp.o
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/PollDispatcher.cpp.o
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/SelectDispatcher.cpp.o
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/build.make
reactor/libreactor.a: reactor/CMakeFiles/reactor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zxe/Documents/HttpReactorServer/ReactorHttp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libreactor.a"
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && $(CMAKE_COMMAND) -P CMakeFiles/reactor.dir/cmake_clean_target.cmake
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/reactor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
reactor/CMakeFiles/reactor.dir/build: reactor/libreactor.a
.PHONY : reactor/CMakeFiles/reactor.dir/build

reactor/CMakeFiles/reactor.dir/clean:
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor && $(CMAKE_COMMAND) -P CMakeFiles/reactor.dir/cmake_clean.cmake
.PHONY : reactor/CMakeFiles/reactor.dir/clean

reactor/CMakeFiles/reactor.dir/depend:
	cd /home/zxe/Documents/HttpReactorServer/ReactorHttp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zxe/Documents/HttpReactorServer/ReactorHttp /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor /home/zxe/Documents/HttpReactorServer/ReactorHttp /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor /home/zxe/Documents/HttpReactorServer/ReactorHttp/reactor/CMakeFiles/reactor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : reactor/CMakeFiles/reactor.dir/depend

