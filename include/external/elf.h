#ifndef _ELF_H
#define _ELF_H

/* This instance of <elf.h> only provides the relocation macros and PowerPC
 * relocation types. For the rest of the header, please see your local <elf.h>.
 */

#define ELF32_R_SYM(val)			((val) >> 8)
#define ELF32_R_TYPE(val)			((val) & 0xff)
#define ELF32_R_INFO(sym, type)		(((sym) << 8) + ((type) & 0xff))

#define R_PPC_NONE					0
#define R_PPC_ADDR32				1
#define R_PPC_ADDR24				2
#define R_PPC_ADDR16				3
#define R_PPC_ADDR16_LO				4
#define R_PPC_ADDR16_HI				5
#define R_PPC_ADDR16_HA				6
#define R_PPC_ADDR14				7
#define R_PPC_ADDR14_BRTAKEN		8
#define R_PPC_ADDR14_BRNTAKEN		9
#define R_PPC_REL24					10
#define R_PPC_REL14					11
#define R_PPC_REL14_BRTAKEN			12
#define R_PPC_REL14_BRNTAKEN		13

#define R_PPC_EMB_SDA21				109

#endif // _ELF_H
