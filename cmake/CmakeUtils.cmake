function(copy_data_contents source_folder)
    # Check if the source folder exists
    if(NOT EXISTS ${source_folder})
        message(FATAL_ERROR "Source folder does not exist: ${source_folder}")
    endif()

    # Determine the destination folder based on the build directory
    set(destination_folder ${CMAKE_BINARY_DIR})

    # Copy the contents of the source folder to the destination folder
    file(COPY ${source_folder} DESTINATION ${destination_folder})
endfunction()