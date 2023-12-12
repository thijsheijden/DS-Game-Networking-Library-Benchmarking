# CMakeList.txt : CMake project for EnetClient, include source and define

if(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()
add_executable (EnetClient "Client.h" "Client.cpp" "main.cpp" "Game.h" "Game.cpp" )
target_include_directories(EnetClient PRIVATE 
                                            "../enet/include" 
                                                "../common")
# Conditionally specify enet library based on the platform
if(WIN32)
    target_link_libraries(EnetClient PRIVATE ${BASE_DIR}/enet/enet.lib Ws2_32 winmm)
elseif(UNIX)
 
    find_library(ENET_LIBRARY enet)

    if(ENET_LIBRARY)
        target_link_libraries(EnetClient PRIVATE ${ENET_LIBRARY} pthread dl)
    else()
        message(FATAL_ERROR "libenet.so not found. Specify the correct path or install enet.")
    endif()
endif()
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EnetClient PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
