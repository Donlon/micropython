#include <stdio.h>
#include <string.h>

#include <py/builtin.h>
#include <py/compile.h>
#include <py/runtime.h>
#include <py/repl.h>
#include <py/gc.h>
#include <py/mperrno.h>
#include <py/mphal.h>
#include <shared/runtime/gchelper.h>
#include <shared/runtime/pyexec.h>
#include <py/stackctrl.h>

#include "mphalport.h"

static char *stack_top;

#define MICROPY_GC_HEAP_SIZE (1 * 1024 * 1024)

__attribute__((section(".heap")))
static char heap[MICROPY_GC_HEAP_SIZE];

extern uint32_t _stack; // NOLINT
extern uint32_t __stack_top; // NOLINT

int main() {
    int stack_dummy;
    stack_top = (char *) &stack_dummy;

    int status = mp_hal_init();
    if (status) {
        return 1;
    }
    mp_stack_set_top(&__stack_top);
    mp_stack_set_limit(&__stack_top - &_stack - 256);
    gc_init(heap, heap + sizeof(heap));

    for (;;) {
        mp_init();
#if MICROPY_ENABLE_COMPILER
#if MICROPY_REPL_EVENT_DRIVEN
        pyexec_event_repl_init();
        for (;;) {
            int c = mp_hal_stdin_rx_chr();
            if (c < 0) {
                continue;
            }
            if (pyexec_event_repl_process_char(c)) {
                break;
            }
        }
#else
        pyexec_friendly_repl();
#endif
        gc_sweep_all();
        mp_deinit();
#else
        pyexec_frozen_module("frozentest.py");
#endif
    }
    return 0;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG

void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}

#endif
