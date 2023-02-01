cmake_minimum_required(VERSION 3.18.0)

include(cmake/compiler.cmake)

project(micropython)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 99)

set(MICROPY_PORT_DIR    ${CMAKE_CURRENT_SOURCE_DIR})
set(MICROPY_DIR         ${MICROPY_PORT_DIR}/../..)
set(MICROPY_TARGET      micropython)
set(XILINX_LIB_PATH     ${MICROPY_DIR}/lib/xilinx-embeddedsw)

include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/py/usermod.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)
include(cmake/xilinx-bsp.cmake)

create_xilinx_bsp(xil_bsp "" "uartlite;spi")

set(MICROPY_SOURCE_PORT
        main.c
        mphalport.c
        baremetal/startup.c
        baremetal/irq_handlers.c
        baremetal/syscalls.c
)
list(TRANSFORM MICROPY_SOURCE_PORT PREPEND ${MICROPY_PORT_DIR}/)

set(MICROPY_SOURCE_SHARED
    libc/printf.c
    readline/readline.c
    runtime/interrupt_char.c
    runtime/mpirq.c
    runtime/pyexec.c
    runtime/gchelper_m0.s
    runtime/gchelper_native.c
    runtime/stdout_helpers.c
    runtime/sys_stdio_mphal.c
    timeutils/timeutils.c
)
list(TRANSFORM MICROPY_SOURCE_SHARED PREPEND ${MICROPY_DIR}/shared/)

set(MICROPY_SOURCE_LIB
    littlefs/lfs2.c
    littlefs/lfs2_util.c
)
list(TRANSFORM MICROPY_SOURCE_LIB PREPEND ${MICROPY_DIR}/lib/)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
)

add_executable(${MICROPY_TARGET})

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
)

target_link_libraries(${MICROPY_TARGET} PRIVATE xil_bsp)
target_link_libraries(${MICROPY_TARGET} PRIVATE usermod)

target_include_directories(${MICROPY_TARGET} PRIVATE
    ${MICROPY_INC_CORE}
    ${MICROPY_INC_USERMOD}
#    ${MICROPY_BOARD_DIR}
    "${PROJECT_SOURCE_DIR}"
    "${xil_bsp_include_dir}"
    "${CMAKE_BINARY_DIR}"
)

target_compile_options(${MICROPY_TARGET} PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)

target_link_options(${MICROPY_TARGET} PRIVATE
    -Wall
    -Wextra
    -T ${CMAKE_CURRENT_SOURCE_DIR}/${MICROPY_TARGET}.ld
    -Wl,-Map=${MICROPY_TARGET}.map
)

set_target_properties(${MICROPY_TARGET} PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MICROPY_TARGET}.ld)

include(${MICROPY_DIR}/py/mkrules.cmake)
