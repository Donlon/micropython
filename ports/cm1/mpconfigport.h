#include <stdint.h>

#define MICROPY_HW_BOARD_NAME "SoftCore-CM1"
#define MICROPY_HW_MCU_NAME "ARM DesignStart Cortex-M1"

#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (16)
#define MICROPY_ALLOC_PATH_MAX              (256)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER             (1)

#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_EMACS_KEYS             (1)
#define MICROPY_REPL_EMACS_WORDS_MOVE       (1)
#define MICROPY_REPL_EMACS_EXTRA_WORDS_MOVE (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT      (1)

#define MICROPY_STACK_CHECK                 (1)

#define MICROPY_VFS                         (1)
#define MICROPY_VFS_LFS2                    (1)

#define MICROPY_PY_USELECT                  (0)
#define MICROPY_PY_UASYNCIO                 (1)

// type definitions for the specific machine
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef intptr_t mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MP_STATE_PORT MP_STATE_VM
