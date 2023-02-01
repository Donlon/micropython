MEMORY
{
    ITCM  (RX) : ORIGIN = 0x00000000, LENGTH = 32k
    DTCM  (RW) : ORIGIN = 0x20000000, LENGTH = 32k
    DDR   (RWX) : ORIGIN = 0x60000000, LENGTH = 512M
}

REGION_ALIAS("REGION_INTVEC", ITCM);
REGION_ALIAS("REGION_TEXT", DDR);
REGION_ALIAS("REGION_BSS",  DDR);
REGION_ALIAS("REGION_DATA", DDR);
REGION_ALIAS("REGION_HEAP", DDR);
REGION_ALIAS("REGION_STACK", DDR);

SECTIONS {
    /* section for the interrupt vector area                                 */
    PROVIDE (_intvecs_base_address =
        DEFINED(_intvecs_base_address) ? _intvecs_base_address : 0x0);

    .intvecs (_intvecs_base_address) : AT (_intvecs_base_address) {
        KEEP (*(.intvecs))
    } > REGION_INTVEC

    .text : {
        CREATE_OBJECT_SYMBOLS
        KEEP (*(.text))
        *(.text.*)
        . = ALIGN(0x4);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        KEEP (*(.dtors))
        . = ALIGN(0x4);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        KEEP (*(.init))
        KEEP (*(.fini*))
    } > REGION_TEXT AT> REGION_TEXT

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > REGION_TEXT AT> REGION_TEXT

    .data : {
        __data_load = LOADADDR (.data);
        __data_start = .;
        KEEP (*(.data))
        KEEP (*(.data*))
        . = ALIGN (4);
        __data_end = .;
    } > REGION_DATA AT> REGION_TEXT

    .bss : {
        __bss_start__ = .;
        *(.shbss)
        KEEP (*(.bss))
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
    } > REGION_BSS AT> REGION_BSS

    .heap (NOLOAD) : {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP (*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > REGION_HEAP AT> REGION_HEAP

    .stack (NOLOAD) : ALIGN(0x8) {
        _stack = .;
        KEEP(*(.stack))
    } > REGION_STACK AT> REGION_STACK

	__stack_top = ORIGIN(REGION_STACK) + LENGTH(REGION_STACK);
    PROVIDE(__stack = __stack_top);
}

ENTRY(Reset_Handler)
