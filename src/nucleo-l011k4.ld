/*
 * Nucleo-L011K4
 */

MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 16K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 2K
}

INCLUDE ./cortex-m-generic.ld
