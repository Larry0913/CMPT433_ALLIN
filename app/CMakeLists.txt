# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(allin ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(allin LINK_PRIVATE hal)
target_link_libraries(allin LINK_PRIVATE Threads::Threads)

# ALSA support
find_package(ALSA REQUIRED)
target_link_libraries(allin LINK_PRIVATE asound)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET allin POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:allin>"
     "~/cmpt433/public/myApps/allin" 
  COMMENT "Copying ARM executable to public NFS directory")
