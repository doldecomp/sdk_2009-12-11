#ifndef DECOMP_H
#define DECOMP_H

/*******************************************************************************
 * Unknown objects
 */

#define unk1_t	char
#define unk2_t	short
#define unk4_t	int
#define unk8_t	long long

#define unk_t	unk4_t

#define STRUCT_MEMBER(type_, expr_, offset_)	\
	(*(__typeof__(type_) *)((unsigned long)(expr_) + (offset_)))

#endif // DECOMP_H
