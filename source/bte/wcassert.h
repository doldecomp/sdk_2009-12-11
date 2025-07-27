#ifndef BTE_WC_ASSERT_H
#define BTE_WC_ASSERT_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/wcassert.h
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * macros
 */

#if !defined(NDEBUG)
# define WCAssert_FileLine(file_, line_)	\
	wc_assert("ASSERT at %s line %d\n", file_, line_)
#else
# define WCAssert_FileLine(file_, line_)
#endif

#define WCAssert_File(file_)	WCAssert_FileLine(  file_ , __LINE__)
#define WCAssert_Line(line_)	WCAssert_FileLine(__FILE__,   line_ )
#define WCAssert()				WCAssert_FileLine(__FILE__, __LINE__)

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

// WARNING: This function does not have a definition
void wc_assert(char const *message, char const *file, unsigned long int line);

#ifdef __cplusplus
	}
#endif

#endif // BTE_WC_ASSERT_H
