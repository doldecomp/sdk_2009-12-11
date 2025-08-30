#ifndef __PPC_EABI_LINKER_H
#define __PPC_EABI_LINKER_H

/* This instance of <__ppc_eabi_linker.h> only provides the _f linker symbols.
 * For the rest of the header, please see your local <__ppc_eabi_linker.h>.
 */

extern char _f_init[];
extern char _f_text[];
extern char _fextab[];
extern char _fextabindex[];
extern char _f_ctors[];
extern char _f_dtors[];
extern char _f_rodata[];
extern char _f_data[];
extern char _f_bss[];
extern char _f_sdata[];
extern char _f_sbss[];
extern char _f_sdata2[];
extern char _f_sbss2[];

#endif // __PPC_EABI_LINKER_H
