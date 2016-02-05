// defined in linker script
extern unsigned long _sidata_st; // start address for the initialization values of the .data section
extern unsigned long _sdata_st; // start address for the .data section
extern unsigned long _edata_st; // end address for the .data section
extern unsigned long _sbss_st; // start address for the .bss section
extern unsigned long _ebss_st; // end address for the .bss section

extern unsigned long __init_array_start_st; // start address for constructor array
extern unsigned long __init_array_end_st; // end address for constructor array

extern unsigned long _sidata; // start address for the initialization values of the .data section
extern unsigned long _sdata; // start address for the .data section
extern unsigned long _edata; // end address for the .data section
extern unsigned long _sbss; // start address for the .bss section
extern unsigned long _ebss; // end address for the .bss section

extern unsigned long __init_array_start; // start address for constructor array
extern unsigned long __init_array_end; // end address for constructor array

extern char _end; // end of RAM

void startup()
{
	unsigned long *src, *dest, *func;

	// Copy initial values for data section.
	src = &_sidata_st;
	for(dest = &_sdata_st; dest < &_edata_st; )
	{
		*(dest++) = *(src++);
	}
	// Zero fill the bss segment.
	for(dest = &_sbss_st; dest < &_ebss_st; )
	{
		*(dest++) = 0;
	}

	// Copy initial values for data section.
	src = &_sidata;
	for(dest = &_sdata; dest < &_edata; )
	{
		*(dest++) = *(src++);
	}
	// Zero fill the bss segment.
	for(dest = &_sbss; dest < &_ebss; )
	{
		*(dest++) = 0;
	}

	// set heap start address
	extern char *init_heap_end;
	init_heap_end = &_end;

	// Call static constructors.
	for(func = &__init_array_start_st; func < &__init_array_end_st; func++ ){
		((void(*)())(*func))();
	}

	// Call static constructors.
	for(func = &__init_array_start; func < &__init_array_end; func++ ){
		((void(*)())(*func))();
	}
}
