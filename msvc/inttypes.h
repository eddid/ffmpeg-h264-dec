#ifndef INTTYPES_H
#define INTTYPES_H

#include <math.h>
#include <intrin.h>

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

#if defined(_MSC_VER)
#define isnan _isnan
#define strtoll _strtoi64
#define snprintf _snprintf
#endif

#define PRIu32  "u"
#define PRId32  "d"
#define PRId64 "ld"
#define PRIx64 "lx"
#define PRIX64 "lX"

typedef unsigned char              u8;
typedef signed char                s8;
typedef unsigned short             u16;
typedef signed short               s16;
typedef unsigned int               u32;
typedef signed int                 s32;

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

#ifndef _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#define _TM_DEFINED
#endif

#endif