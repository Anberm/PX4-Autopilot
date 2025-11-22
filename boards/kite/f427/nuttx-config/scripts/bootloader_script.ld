/****************************************************************************
 * scripts/bootloader_script.ld
 *
 * Bootloader linker script for STM32F427
 *
 * The STM32F427 has 2048Kb of FLASH beginning at address 0x0800:0000.
 * The first 0x4000 (16KB) of flash is reserved for the bootloader.
 * Application firmware runs from 0x0800:4000 onwards.
 *
 ****************************************************************************/

MEMORY
{
    flash (rx)   : ORIGIN = 0x08000000, LENGTH = 16K
    sram (rwx)   : ORIGIN = 0x20000000, LENGTH = 192K
    ccsram (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

OUTPUT_ARCH(arm)

ENTRY(__start)
EXTERN(_vectors)
EXTERN(abort)

SECTIONS
{
	.text : {
		_stext = ABSOLUTE(.);
		*(.vectors)
		*(.text .text.*)
		*(.fixup)
		*(.gnu.warning)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.t.*)
		*(.got)
		*(.gcc_except_table)
		*(.gnu.linkonce.r.*)
		_etext = ABSOLUTE(.);
	} > flash

	.init_section : {
		_sinit = ABSOLUTE(.);
		KEEP(*(.init_array .init_array.*))
		_einit = ABSOLUTE(.);
	} > flash

	.ARM.extab : {
		*(.ARM.extab*)
	} > flash

	__exidx_start = ABSOLUTE(.);
	.ARM.exidx : {
		*(.ARM.exidx*)
	} > flash
	__exidx_end = ABSOLUTE(.);

	_eronly = ABSOLUTE(.);

	.data : {
		_sdata = ABSOLUTE(.);
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		CONSTRUCTORS
		_edata = ABSOLUTE(.);
	} > sram AT > flash

	.bss : {
		_sbss = ABSOLUTE(.);
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = ABSOLUTE(.);
	} > sram

	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_info 0 : { *(.debug_info) }
	.debug_line 0 : { *(.debug_line) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	.debug_aranges 0 : { *(.debug_aranges) }
}
