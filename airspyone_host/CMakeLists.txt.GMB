#top dir cmake project for libairspy + airspy-tools

cmake_minimum_required(VERSION 2.8)
project (airspy_all)

#provide missing strtoull() for VC11
if(MSVC11)
    add_definitions(-Dstrtoull=_strtoui64)
endif(MSVC11)

#GMB 15mar21 - linking against libusb.a with MinGW32 (no package)
cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0081 NEW)
include_directories(rtlsdr LIBUSB_INCLUDE_DIR)
link_directories(${LIBUSB_LIBRARY_DIRS})
link_libraries(${LIBUSB_LIBRARIES})
set(LIBUSB_FOUND 1)
#GMB END


add_subdirectory(libairspy)
add_subdirectory(airspy-tools)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)


add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Copy Files Win32 only
########################################################################

if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
 if (NOT DEFINED LIBUSB_LIBRARIES)
 configure_file(
     ${CMAKE_CURRENT_BINARY_DIR}/../libs_win32/libusb-1.0.dll
     ${CMAKE_CURRENT_BINARY_DIR}/airspy-tools/src/libusb-1.0.dll
 COPYONLY)
 endif()

 # if (NOT DEFINED THREADS_PTHREADS_WIN32_LIBRARY)
 # configure_file(
     # ${CMAKE_CURRENT_BINARY_DIR}/../libs_win32/pthreadGC2.dll
     # ${CMAKE_CURRENT_BINARY_DIR}/airspy-tools/src/pthreadGC2.dll
 # COPYONLY)
 # endif()
 
 if (NOT DEFINED THREADS_PTHREADS_WIN32_LIBRARY)
 configure_file(
     ${CMAKE_CURRENT_BINARY_DIR}/../deps/lib64/libpthreadGC2.a
     ${CMAKE_CURRENT_BINARY_DIR}/airspy-tools/src/pthreadGC2.dll
 COPYONLY)
 endif()
 
 
endif()

