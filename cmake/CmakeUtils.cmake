function(copy_runtime_data target source_folder data_set)
    if(NOT EXISTS "${source_folder}")
        message(FATAL_ERROR "Source folder does not exist: ${source_folder}")
    endif()

    file(GLOB_RECURSE data_files CONFIGURE_DEPENDS LIST_DIRECTORIES false "${source_folder}/*")
    set(stamp_file "${CMAKE_CURRENT_BINARY_DIR}/${target}_${data_set}_data.stamp")
    set(copy_target "${target}_${data_set}_data")

    add_custom_command(
        OUTPUT "${stamp_file}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/Data"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${source_folder}" "${CMAKE_BINARY_DIR}/Data"
        COMMAND "${CMAKE_COMMAND}" -E touch "${stamp_file}"
        DEPENDS ${data_files}
        COMMENT "Copying ${data_set} runtime data"
        VERBATIM
    )
    add_custom_target("${copy_target}" DEPENDS "${stamp_file}")
    add_dependencies("${target}" "${copy_target}")
endfunction()

function(set_target_folder_prop target_list destination_folder)
    foreach(target IN LISTS ${target_list})
        if(TARGET "${target}")
            set_property(TARGET "${target}" PROPERTY FOLDER "${destination_folder}")
        endif()
    endforeach()
endfunction()
