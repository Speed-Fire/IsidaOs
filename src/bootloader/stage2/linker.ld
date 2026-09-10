OUTPUT_FORMAT(binary)
ENTRY(entry)

SECTIONS
{
	. = 0;

	.text :
	{
		KEEP(*(.entry))	
		*(.text*)
		*(.rodata*)
	}

	.data :
	{
		*(.data*)
	}

	.bss (NOLOAD) :
	{
		*(.bss*)
		*(COMMON)
	}
}