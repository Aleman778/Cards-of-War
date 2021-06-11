#include <stdint.h>

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

#if BUILD_DEBUG
void
__assert(const char* expression, const char* file, int line) {
    fprintf(stderr, "%s:%d: Assertion failed: %s\n", file, line, expression);
    *(int *)0 = 0; // NOTE(alexander): purposefully trap the program
}

#define assert(expression) (void)((expression) || (__assert(#expression, __FILE__, __LINE__), 0))
#else
#define assert(expression)
#endif

typedef int8_t    s8;
typedef uint8_t   u8;
typedef int16_t   s16;
typedef uint16_t  u16;
typedef int32_t   s32;
typedef uint32_t  u32;
typedef int64_t   s64;
typedef uint64_t  u64;
typedef uintptr_t usize;
typedef intptr_t  ssize;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef uint8_t   bool;
#define true 1
#define false 0

typedef union {
    struct {
        f32 x, y;
    };
    struct {
        f32 u, v;
    };
    struct {
        f32 width, height;
    };
    f32 data[2];
} v2;

typedef union {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    struct {
        f32 u;
        f32 v;
        f32 w;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
    };
    f32 data[3];
} v3;

typedef union {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 data[4];
} v4;

typedef struct {
    v2 min;
    v2 max;
} AABB;

typedef struct {
    v2 p;
    v2 r;
} Line;


#define kilobytes(value) (1024LL * (value))
#define megabytes(value) (1024LL * kilobytes(value))
#define gigabytes(value) (1024LL * megabytes(value))
#define terabytes(value) (1024LL * gigabytes(value))

#define MAX_UINT32 0xFFFFFFFF

#define epsilon32 0.0001f;
#define pi32 3.14159265359f;
#define half_pi32 0.5f * pi32;
#define two_pi32 2.0f * pi32;
#define deg_to_rad two_pi32 / 360.0f;
#define rad_to_deg 1.0f / deg_to_rad;
