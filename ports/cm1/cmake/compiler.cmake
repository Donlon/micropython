set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSTEM_NAME Generic)

set(CFLAGS "-mthumb -mcpu=cortex-m1 -Wall -Wextra -g")
set(CFLAGS "${CFLAGS} -fdata-sections -ffunction-sections -flto=auto")
set(CXXFLAGS "${CFLAGS} -fno-exceptions")

add_compile_options("$<$<CONFIG:Debug>:-DDEBUG>")
add_compile_options("$<$<CONFIG:Debug>:-Og>")
add_compile_options("$<$<CONFIG:Release>:-O2>")
add_compile_options("$<$<CONFIG:MinSizeRelease>:-Os>")

set(LDFLAGS "--specs=nosys.specs")
set(LDFLAGS "${LDFLAGS} -Wl,--gc-sections -Wl,--print-memory-usage")

set(CMAKE_ASM_FLAGS ${CFLAGS})
set(CMAKE_C_FLAGS ${CFLAGS})
set(CMAKE_CXX_FLAGS ${CXXFLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${LDFLAGS})