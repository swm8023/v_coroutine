project(colib)

enable_language(C ASM)

# select asm file
message(" --- select asm ---")
# name = cmd_platform_abi_format_tool
# support win/posix/

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(FC_ASM_PLATFORM "x86_64")
else()
	set(FC_ASM_PLATFORM "i386")
endif()

if(WIN32)
	set(FC_ASM_ABI "ms")
else()
	set(FC_ASM_ABI "sysv")
endif()

if(WIN32)
	set(FC_ASM_FORMAT "pe")
elseif(APPLE)
	set(FC_ASM_FORMAT "macho")
else()
	set(FC_ASM_FORMAT "elf")
endif()


if(WIN32)
	set(FC_ASM_TOOL "masm.asm")
else()
	set(FC_ASM_TOOL "gas.S")
endif()

set(ASM_SUFFIX "${FC_ASM_PLATFORM}_${FC_ASM_ABI}_${FC_ASM_FORMAT}_${FC_ASM_TOOL}")
message(${ASM_SUFFIX})

if (WIN32)
    enable_language(ASM_MASM)
    set(CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} -DBOOST_CONTEXT_EXPORT=")
endif()

set(ASM_DIR context/asm)
list(APPEND CO_ASM_FILE "${ASM_DIR}/make_${ASM_SUFFIX}")
list(APPEND CO_ASM_FILE "${ASM_DIR}/jump_${ASM_SUFFIX}")
list(APPEND CO_ASM_FILE "${ASM_DIR}/ontop_${ASM_SUFFIX}")
message("${CO_ASM_FILE}")
list(APPEND CO_SRC_LIST ${CO_ASM_FILE})


# src files
message(" --- add source ---")
macro(add_vg_source_dir name)
    file(GLOB SRC_FILE ${name}/*.h ${name}/*.cc)
	if (WIN32)
		list(FILTER SRC_FILE EXCLUDE  REGEX _posix.cc)
	else()
		list(FILTER SRC_FILE EXCLUDE  REGEX _win.cc)
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

install(TARGETS vco DESTINATION lib)
install(DIRECTORY ../co/ DESTINATION include/co FILES_MATCHING PATTERN "*.h")
