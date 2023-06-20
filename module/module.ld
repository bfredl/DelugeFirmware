OUTPUT_ARCH(arm)
ENTRY(mod_start)

MEMORY
{
  RAM012L   (rwx) : ORIGIN = 0x20020000, LENGTH = 0x002E0000
}

SECTIONS
{

.reset module_data :
{
  *startup.o (.text)
  *(.text)
  *(.text.mod_start)
} > RAM012L

.text :
{
  text_start = .;
  *(.text)
  address_end_text = .;
} > RAM012L

.rodata :
{
  *(.rodata)
  *(.rodata.*)
  . = ALIGN(0x8);
  _data = .;
  *(.data)
  *(.data.*)
  _edata = .;

  *(.got.plt)
  *(.got)
  . = ALIGN(0x8);
} > RAM012L

}