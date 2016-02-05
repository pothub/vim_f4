/* memory definition script */
INCLUDE ../memory.ld

/* set the start address after power-on (reset) */
start = ORIGIN(VECTOR);

SECTIONS
{
/* interrupt vector table */
	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > VECTOR

/* c++ class constructor */
	.init_array :
	{
		. = ALIGN(4);
		__init_array_start = ABSOLUTE(.);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		. = ALIGN(4);
		__init_array_end = ABSOLUTE(.);
	} > VECTOR

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH
	
/* program code */
	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		. = ALIGN(4);
	} > FLASH

/* read-only data */
	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
		_FLASH_end = ABSOLUTE(.);
	} > FLASH

/* read-write initialized data */
/* put data after the RAM region for static program */
	.data _RAMst_end :
	{
		. = ALIGN(4);
		_sdata = ABSOLUTE(.);
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		_edata = ABSOLUTE(.);
	} > RAM AT>FLASH

	_sidata = LOADADDR(.data);
	_FLASH_end = LOADADDR(.data) + SIZEOF(.data);

/* read-write zero initialized data */
	.bss :
	{
		. = ALIGN(4);
		_sbss = ABSOLUTE(.);
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = ABSOLUTE(.);
	} > RAM
	
	_end = _ebss; /* start of heap region */

/* stack region reservation */
	.usrstack :
	{
		. = ALIGN(4);
		_susrstack = ABSOLUTE(.);
		. = . + _Minimum_Stack_Size ;
		. = ALIGN(4);
		_eusrstack = ABSOLUTE(.);
		_RAM_end = ABSOLUTE(.);
	} > RAM

	.empty_FLASH 0 :
	{
		. = ORIGIN(FLASH) + LENGTH(FLASH) - _FLASH_end;
	}
	
	.empty_RAM 0 :
	{
		. = ORIGIN(RAM) + LENGTH(RAM) - _RAM_end;
	}

	DISCARD 0 :
	{
		libc.a(*)
		libm.a(*)
		libgcc.a(*)
		*(.comment)
		*(.ARM.attributes)
		*(.debug_frame)
	}
}
