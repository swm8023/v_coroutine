project(colib)

enable_language(C ASM)

# select asm file
message(" --- select asm ---")
if (WIN32)
    enable_language(ASM_MASM)
    set(CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} -DBOOST_CONTEXT_EXPORT=")
    if (MSVC)
        if (CMAKE_CL_64)
            set(ASM_SUFFIX "x86_64_ms_pe_masm.asm")
        else()
            set(ASM_SUFFIX "i386_ms_pe_masm.asm")
        endif()
    endif()
elseif(UNIX)

endif()

set(ASM_DIR context/asm)
list(APPEND CO_ASM_FILE "${ASM_DIR}/make_${ASM_SUFFIX}")
list(APPEND CO_ASM_FILE "${ASM_DIR}/jump_${ASM_SUFFIX}")
list(APPEND CO_ASM_FILE "${ASM_DIR}/ontop_${ASM_SUFFIX}")
message("${CONET_ASM_FILE}")
list(APPEND CO_SRC_LIST ${CO_ASM_FILE})


# src files
message(" --- add source ---")
macro(add_vg_source_dir name)
    message(${name})
    file(GLOB SRC_FILE ${name}/*.h ${name}/*.cc)
	if (WIN32)
		file(REMOVE SRC_FILE ${name}/*_posix.cc)
	else()
		file(REMOVE SRC_FILE ${name}/*_win.cc)
	endif()
    list(APPEND CO_SRC_LIST ${SRC_FILE})
    string(REPLACE "/" "\\" groupname ${name})
    source_group(${groupname} FILES ${SRC_FILE})
endmacro()

add_vg_source_dir(context)
add_vg_source_dir(io)
add_vg_source_dir(stack)
add_vg_source_dir(sync)

list(APPEND CO_SRC_LIST coroutine.h)
list(APPEND CO_SRC_LIST coroutine.cc)
list(APPEND CO_SRC_LIST common.h)

add_library(vco STATIC ${CO_SRC_LIST})
install(TARGETS http_parser DESTINATION lib)
install(DIRECTORY ../co/ DESTINATION include/co FILES_MATCHING PATTERN "*.h")
