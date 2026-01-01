#ifndef __INTERNAL_NULL_H__
#define __INTERNAL_NULL_H__

/* NOTE: A lot of SDK seems to have NULL defined as the void * variant, as
 * made apparent by asserts; thus it is now set as the default here for C.
 */

#if !defined(__cplusplus)
# define NULL	((void *)(0))
#else
# define NULL	0
#endif

#endif // __INTERNAL_NULL_H__
