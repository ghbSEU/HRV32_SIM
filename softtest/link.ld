OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  flash : ORIGIN = 0x00, LENGTH = 0x10000000
}

_start = 0x00;
PHDRS
{
  flash PT_LOAD;
  ram_init PT_LOAD;
  ram PT_NULL;
}

SECTIONS
{
  .text           :
  {
    *(.text .text.*)
  } >flash AT>flash :flash

}



