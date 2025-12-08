#ifndef RVL_SDK_HBM_COMMON_H
#define RVL_SDK_HBM_COMMON_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

// #include <revolution/mem/allocator.h>

#include <context_rvl.h>

/*******************************************************************************
 * external variables
 */

// .data (not .sdata; hbm is -sdata 0)
extern "C" MEMAllocator *spAllocator;

/*******************************************************************************
 * classes and functions
 */

// library internals
extern "C++" void *HBMAllocMem(u32 size);
extern "C++" void HBMFreeMem(void *mem);

#endif // RVL_SDK_HBM_COMMON_H
