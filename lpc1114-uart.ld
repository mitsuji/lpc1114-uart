MEMORY
{
	ROM (rx) : ORIGIN = 0x00000000, LENGTH = (32k)
	RAM (w)  : ORIGIN = 0x10000000, LENGTH = (4k)
}

SECTIONS
{
	.text :
        {
		KEEP(*(.VECTOR))
		*(.text .text.*)
		*(.rodata .rotdata.*)
	} > ROM
} 
