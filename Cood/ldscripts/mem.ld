/*
 * Memory Spaces Definitions.
 *
 * Need modifying for a specific board. 
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 *
 * The values below can be addressed in further linker scripts
 * using functions like 'ORIGIN(RAM)' or 'LENGTH(RAM)'.
 */

MEMORY
{
  RAM_EEP (xrw) : ORIGIN = 0x20000000, LENGTH = 8K
  RAM (xrw) : ORIGIN = 0x20002000, LENGTH = 176K

  RAM_T (xrw) : ORIGIN = 0x2002EC00, LENGTH = 1K /* for test */
  RAM_BP (xrw) : ORIGIN = 0x2002F000, LENGTH = 4K /* for dbg message */
  CCMRAM (xrw) : ORIGIN = 0x10000000, LENGTH = 64K
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
  FLASH_VER(rx) : ORIGIN = 0x0809FF00, LENGTH = 240
  FLASH_CHECK (rx) : ORIGIN = 0x0809FFF0, LENGTH = 16
  FLASHB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB0 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB2 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB3 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  MEMORY_ARRAY (xrw)  : ORIGIN = 0x20002000, LENGTH = 32
}

/*
 * For external ram use something like:

   RAM (xrw) : ORIGIN = 0x64000000, LENGTH = 2048K

 */
