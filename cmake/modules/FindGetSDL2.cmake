if(MSVC)
    execute_process(
        COMMAND vcpkg
        install 
        sdl2)
endif()

if(APPLE)
    execute_process(
        COMMAND brew
        install 
        sdl2)
endif()

if(UNIX)
    find_program(PCK_MAN_PATH NAMES apt pacman dnf yum)
    get_filename_component(PCK_MAN ${PCK_MAN_PATH} NAME)
    if(${PCK_MAN} STREQUAL apt)
        execute_process(
            COMMAND apt
            install 
            sdl2-dev)
    elseif({PCK_MAN} STREQUAL pacman)
        execute_process(
            COMMAND pacman
            -S 
            sdl2)
    elseif({PCK_MAN} STREQUAL dnf)
        execute_process(
        COMMAND dnf
            install 
            sdl2-dev
            sld2-static)
    elseif({PCK_MAN} STREQUAL yum)
    execute_process(
        COMMAND yum
            install 
            SDL2-devel)
    endif()
endif()

find_package(sdl2 REQUIRED)