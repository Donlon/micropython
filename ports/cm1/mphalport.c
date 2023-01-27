#include "py/mphal.h"

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
}

int mp_hal_stdin_rx_chr(void) {
    while (1);
    // return 0;
}

void mp_hal_set_interrupt_char(int c) {

}