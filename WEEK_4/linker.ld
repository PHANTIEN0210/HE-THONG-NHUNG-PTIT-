MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.rodata)
    } > FLASH

    .data : { *(.data) } > RAM
    
    .bss :
    {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        __bss_end__ = .;
    } > RAM
}
