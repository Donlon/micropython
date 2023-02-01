#include "py/mphal.h"
#include "xuartlite.h"
#include "xuartlite_i.h"

static XUartLite hal_uart_inst;

XUartLite_Config uart_def_config = {
        .DeviceId    = 0,        /**< Unique ID  of device */
        .RegBaseAddr = 0x40600000,    /**< Register base address */
        .BaudRate    = 115200,        /**< Fixed baud rate */
        .UseParity   = 0,        /**< Parity generator enabled when TRUE */
        .ParityOdd   = 0,        /**< Parity generated is odd when TRUE, even when FALSE */
        .DataBits    = 8,        /**< Fixed data bits */
};

static int hal_uart_init() {
    int status = XUartLite_CfgInitialize(&hal_uart_inst, &uart_def_config,
                                         uart_def_config.RegBaseAddr);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    status = XUartLite_SelfTest(&hal_uart_inst);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }
    return status;
}

int mp_hal_init() {
    int status = hal_uart_init();
    return status;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    XUartLite_Send(&hal_uart_inst, (u8 *) str, len);
    while (hal_uart_inst.SendBuffer.RemainingBytes > 0) {
        XUartLite_SendBuffer(&hal_uart_inst);
    }
}

int mp_hal_stdin_rx_chr(void) {
    u8 ch;
    unsigned int c = XUartLite_Recv(&hal_uart_inst, &ch, 1);
    return c == 1 ? ch : -1;
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return 0;
}

uint64_t mp_hal_time_ns() {
    return 0;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return 0;
}