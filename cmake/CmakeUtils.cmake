function(copy_data_contents source_folder destination_folder)
    # Check if the source folder exists
    if(NOT EXISTS ${source_folder})
        message(FATAL_ERROR "Source folder does not exist: ${source_folder}")
    endif()

    # Copy the contents of the source folder to the destination folder
    file(COPY ${source_folder} DESTINATION ${destination_folder})
endfunction()

function(set_target_folder_prop target_list destination_folder)
    foreach(target IN LISTS ${target_list})
        set_property(TARGET ${target} PROPERTY FOLDER "${destination_folder}")
    endforeach()
endfunction()