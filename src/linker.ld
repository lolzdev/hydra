OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
  ram  (wxa) : ORIGIN = 0x0000000080200000, LENGTH = 128M
}

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}

SECTIONS
{
  .text : {
    PROVIDE(__text_start = .);
    *(.text.init) *(.text .text.*)
    PROVIDE(__text_end = .);
  } >ram AT>ram :text

  PROVIDE(__global_pointer = .);

  .rodata : {
    PROVIDE(__rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(__rodata_end = .);
  } >ram AT>ram :text

  .data : {
    . = ALIGN(4096);
    PROVIDE(__data_start = .);
    *(.sdata .sdata.*) *(.data .data.*)
    PROVIDE(__data_end = .);
  } >ram AT>ram :data

  .bss : {
    PROVIDE(__bss_start = .);
    *(.sbss .sbss.*) *(.bss .bss.*)
    PROVIDE(__bss_end = .);
  } >ram AT>ram :bss

  PROVIDE(__memory_start = ORIGIN(ram));
  PROVIDE(__stack = __bss_end + 0x80000);
  PROVIDE(__memory_end = ORIGIN(ram) + LENGTH(ram));
  PROVIDE(__heap_start = __stack);
  PROVIDE(__heap_size = __memory_end - __heap_start);
}
