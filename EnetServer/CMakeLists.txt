# CMakeList.txt : CMake project for EnetGame, include source and define
# project specific logic here.
#
# Add source to this project's executable.
add_executable (EnetServer "Packet.cpp" "Packet.hpp" "server.cpp")
target_include_directories(EnetServer PRIVATE 
                                            "../enet/include" 
                                                "../common")
# Conditionally specify enet library based on the platform
if(WIN32)
    target_link_libraries(EnetServer PRIVATE ${BASE_DIR}/enet/enet.lib)
elseif(UNIX)
    find_library(ENET_LIBRARY enet)

    if(ENET_LIBRARY)
        target_link_libraries(EnetServer PRIVATE ${ENET_LIBRARY})
    else()
        message(FATAL_ERROR "libenet.so not found. Specify the correct path or install enet.")
    endif()
endif()
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EnetServer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
