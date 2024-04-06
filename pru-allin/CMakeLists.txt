# Always run our custom command to copy PRU code to NFS
# Source: https://stackoverflow.com/a/31518137
add_custom_target(
    my_custom_target_that_always_runs ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/__this_file_never_exists__.h
)

# Copy the PRU code folder to NFS
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/__this_file_never_exists__.h  # fake! ensure we run!
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_SOURCE_DIR}/pru-allin"
        "~/cmpt433/public/pru/pru-allin" 
    COMMENT "=======> Copying PRU code to public NFS directory... You must build /install on BBG <======="
)