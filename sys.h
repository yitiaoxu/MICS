#ifndef __SYS_H
#define __SYS_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// 一下定义均与'cmsis_gcc.h'、'cmsis_armclang.h'相同
#ifndef   __WEAK
	#define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __STATIC_FORCEINLINE
	#define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static __inline
#endif
#ifndef   __PACKED_STRUCT
	#define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif

#endif // __SYS_H
