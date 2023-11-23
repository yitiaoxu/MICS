#ifndef __ASSIST_CMSIS_H
#define __ASSIST_CMSIS_H

#ifdef __ARM_ARCH
#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */
#elif (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
// Nothing to do
#else
#warning Unknown plateform
#endif // __ARM_ARCH


// 以下定义均与'cmsis_gcc.h'、'cmsis_armclang.h'相同
#ifndef   __WEAK
    #define __WEAK                                  __attribute__((weak))
#endif
#ifndef   __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE                    __attribute__((always_inline)) static __inline
#endif
#ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT                         struct __attribute__((packed, aligned(1)))
#endif

// 以下定义在CMSIS内暂时没有
#ifndef __FORCEINLINE
    #define __FORCEINLINE                           __attribute__((always_inline)) __inline
#endif

#endif // __ASSIST_CMSIS_H
