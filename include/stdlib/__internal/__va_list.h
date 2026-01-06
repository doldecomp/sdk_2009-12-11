#ifndef __INTERNAL_VA_LIST_H__
#define __INTERNAL_VA_LIST_H__

#ifdef __cplusplus
	extern "C" {
#endif

// implementation detail, not a standard struct
// [SPQE7T]/ISpyD.elf:.debug_info::0x21dc81
struct __va_list_struct
{
	char	gpr;				// size 0x01, offset 0x00
	char	fpr;				// size 0x01, offset 0x01
	char	reserved[2];		// size 0x02, offset 0x02
	char	*input_arg_area;	// size 0x04, offset 0x04
	char	*reg_save_area;		// size 0x04, offset 0x08
}; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x21dc72 (just the array type)
typedef struct __va_list_struct va_list[1];

#if defined(__cplusplus)
namespace std { extern "C" { using ::va_list; } }
#endif

#ifdef __cplusplus
	}
#endif

#endif // __INTERNAL_VA_LIST_H__
