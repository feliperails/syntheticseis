include(GetPrerequisites)

SET(cmake_policy CMP0007)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR})

set(_exec_name ${TARGET_EXEC_NAME})
set(_exec_dir ${TARGET_EXEC_DIR})
set(_dst_dir ${PREREQUISITES_DEST_DIR})
set(_exec_path "${_exec_dir}/${_exec_name}")
set(_local_deps ${TARGET_LOCAL_DEPS})

set(_search_paths ${SEARCH_PATHS})
string(REPLACE "%3b" ";" _search_paths "${_search_paths}")

list(REMOVE_DUPLICATES _search_paths)

#message("search_paths : ${_search_paths}")

set(_lib_paths ${_search_paths})
#filter_valid_lib_paths("${_search_paths}" _lib_paths)


if(WIN32)
    set(_env_program_files "PROGRAMFILES(X86)")
    set(_program_files_temp $ENV{${_env_program_files}})
    file(TO_CMAKE_PATH "${_program_files_temp}" _program_files)
    list(APPEND _lib_paths
        "${_program_files}/Windows Kits/10/Redist/ucrt/DLLs/x64/"
        )
endif()

if( EXISTS ${_exec_path})
    get_prerequisites(${_exec_path} _prerequisites 1 1 "" "${_lib_paths}")
    #message("---> prerequisites: ${_prerequisites} for ${_exec_path}")
    foreach(dep_file ${_prerequisites})
        gp_resolve_item("${_exec_path}" "${dep_file}" "" "${_lib_paths}" dep_file_path)
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${dep_file_path}"
                        "${_dst_dir}"
                        )

        #message(" Copying dep: from ${dep_file} file : ${dep_file_path} to: ${_dst_dir}/${dep_file}")
    endforeach()

    #message("Copying local deps: ${_local_deps} TO: ${_dst_dir}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_local_deps}"
                    "${_dst_dir}"
                    )
endif()
