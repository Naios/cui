function(group_sources)
  set(arg_opt MODE)
  set(arg_single)
  set(arg_multi)
  cmake_parse_arguments(GROUP_SOURCES "${arg_opt}" "${arg_single}"
                        "${arg_multi}" ${ARGN})

  if(GROUP_SOURCES_MODE)
    set(WITH_SOURCE_TREE ${GROUP_SOURCES_MODE})
  else()
    set(WITH_SOURCE_TREE "hierarchical")
  endif()

  # Skip this if WITH_SOURCE_TREE is not set (empty string).
  if(NOT ${WITH_SOURCE_TREE} STREQUAL "")
    foreach(dir ${ARGN})
      # Include all header and c files
      file(
        GLOB_RECURSE elements
        RELATIVE ${dir}
        ${dir}/*)

      foreach(element ${elements})
        # Extract filename and directory
        get_filename_component(element_name ${element} NAME)
        get_filename_component(element_dir ${element} DIRECTORY)

        if(NOT ${element_dir} STREQUAL "")
          # If the file is in a subdirectory use it as source group.
          if(${WITH_SOURCE_TREE} STREQUAL "flat")
            # Build flat structure by using only the first subdirectory.
            string(FIND ${element_dir} "/" delemiter_pos)
            if(NOT ${delemiter_pos} EQUAL -1)
              string(SUBSTRING ${element_dir} 0 ${delemiter_pos} group_name)
              source_group("${group_name}" FILES ${dir}/${element})
            else()
              # Build hierarchical structure. File is in root directory.
              source_group("${element_dir}" FILES ${dir}/${element})
            endif()
          else()
            # Use the full hierarchical structure to build source_groups.
            string(REPLACE "/" "\\" group_name ${element_dir})
            source_group("${group_name}" FILES ${dir}/${element})
          endif()
        else()
          # If the file is in the root directory, place it in the root
          # source_group.
          source_group("\\" FILES ${dir}/${element})
        endif()
      endforeach()
    endforeach()
  endif()
endfunction()
