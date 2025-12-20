#ifndef RVL_SDK_H
#define RVL_SDK_H

/* NOTE: Headers are determined not to be #included in <revolution.h> if there
 * exists a game with DWARF line info that lists <revolution.h> but not that
 * specific header.
 */

#include <revolution/cx.h>
/* #include <revolution/darch.h> */ // Doesn't seem to be included here
/* #include <revolution/enc.h> */ // [SPQE7T]: Not included in <revolution.h>.
#include <revolution/kpad.h>
#include <revolution/kpr.h>
/* #include <revolution/rso.h> */ // Not included in <revolution.h>.
#include <revolution/tpl.h>
#include <revolution/wenc.h>
#include <revolution/wpad.h>

#endif // RVL_SDK_H
