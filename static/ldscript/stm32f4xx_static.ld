/* memory definition script */
INCLUDE ../memory.ld
/* 強制的に取り込むシンボル名リスト */
INCLUDE includes.txt


SECTIONS
{
/* c++ class constructor */
	.init_array :
	{
		. = ALIGN(4);
		__init_array_start_st = ABSOLUTE(.);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		. = ALIGN(4);
		__init_array_end_st = ABSOLUTE(.);
	} > FLASHst

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASHst
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASHst
	
/* program code */
	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		. = ALIGN(4);
		_FLASHst_end = ABSOLUTE(.);
	} > FLASHst

/* read-only data */
	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} > FLASHst

/* read-write initialized data */
	.data :
	{
		. = ALIGN(4);
		_sdata_st = ABSOLUTE(.);
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		_edata_st = ABSOLUTE(.);
	} > RAM AT>FLASHst

	_sidata_st = LOADADDR(.data);
	_FLASHst_end = LOADADDR(.data) + SIZEOF(.data);

/* read-write zero initialized data */
	.bss :
	{
		. = ALIGN(4);
		_sbss_st = ABSOLUTE(.);
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss_st = ABSOLUTE(.);
		_RAMst_end = ABSOLUTE(.);
	} > RAM
	

	.empty_FLASHst 0 :
	{
		. = ORIGIN(FLASHst) + LENGTH(FLASHst) - _FLASHst_end;
	}
	
	DISCARD 0 :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
		*(.comment)
		*(.ARM.attributes)
		*(.debug_frame)
	}
}
