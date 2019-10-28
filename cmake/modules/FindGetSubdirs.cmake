set(subdir_list "")

FILE(GLOB children ${ROOT_DIR} ${ROOT_DIR}/*)
SET(dirlist "")
FOREACH(child ${children})
  IF(IS_DIRECTORY ${child})
    LIST(APPEND dirlist ${child})
  ENDIF()
ENDFOREACH()

foreach(subdir ${dirlist})
    get_filename_component(subdir_name ${subdir} NAME)
    if(${subdir_name} MATCHES "^[0-9]+-.+$")
        list(APPEND subdir_list ${subdir})
    endif()
endforeach()

