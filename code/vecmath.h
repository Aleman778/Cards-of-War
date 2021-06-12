#include <math.h>

/***************************************************************************
 * 2D vector functions
 ***************************************************************************/

static const v2 vec2_zero = {{ 0.0f, 0.0f }};
static const v2 vec2_left = {{ -1.0f, 0.0f }};
static const v2 vec2_right = {{ 1.0f, 0.0f }};
static const v2 vec2_up = {{ 0.0f, -1.0f }};
static const v2 vec2_down = {{ 0.0f, 1.0f }};

bool float_equal(f32 x, f32 y)
{
    return fabsf(x - y) < 0.0000001f;
}

v2 vec2(f32 x, f32 y) {
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

v2 vec2_add(v2 a, v2 b) {
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

v2 vec2_adds(v2 a, f32 b) {
    v2 result;
    result.x = a.x + b;
    result.y = a.y + b;
    return result;
}

v2* vec2_addp(v2* a, v2 b) {
    a->x += b.x;
    a->y += b.y;
    return a;
}

v2 vec2_sub(v2 a, v2 b) {
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

v2 vec2_subs(v2 a, f32 b) {
    v2 result;
    result.x = a.x - b;
    result.y = a.y - b;
    return result;
}

v2 vec2_neg(v2 a) {
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

v2* vec2_subp(v2* a, v2 b) {
    a->x -= b.x;
    a->y -= b.y;
    return a;
}

v2 vec2_mul(v2 a, v2 b) {
    v2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

v2 vec2_muls(v2 a, f32 b) {
    v2 result;
    result.x = a.x * b;
    result.y = a.y * b;
    return result;
}

v2 vec2_div(v2 a, v2 b) {
    v2 result;
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    return result;
}

v2 vec2_divs(v2 a, f32 b) {
    v2 result;
    result.x = a.x / b;
    result.y = a.y / b;
    return result;
}

bool vec2_equals(v2 a, v2 b) {
    return (float_equal(a.x, b.x) && float_equal(a.y, b.y));
}

f32 vec2_dot_product(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y; // TODO(alexander): not sure if this should be called cross product?
}

f32 vec2_cross_product(v2 a, v2 b) {
    return a.x * b.y - a.y * b.x;
}

f32 vec2_length_sq(v2 v) {
    return v.x*v.x + v.y*v.y;
}

f32 vec2_length(v2 v) {
    return sqrtf(vec2_length_sq(v)); // TODO(alexander): replace sqrtf with own implementation
}

v2 vec2_normalize(v2 v) {
    return vec2_divs(v, vec2_length(v));
}

f32 vec2_angle(v2 a, v2 b) {
    // NOTE(alexander): is there a better way to calculate this?
    // NOTE(alexander): SPEED: maybe instead take in normalized vectors avoid double normalization
    a = vec2_normalize(a);
    b = vec2_normalize(b);
    return acosf(vec2_dot_product(a, b));
}

f32 vec2_determinant(v2 a, v2 b) {
    // NOTE(alexander): a is left column, b is right column
    return (a.x * b.y) - (b.x * a.y);
}

/***************************************************************************
 * 3D vector functions
 ***************************************************************************/

v3 vec3(f32 x, f32 y, f32 z) {
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

/***************************************************************************
 * 4D vector functions
 ***************************************************************************/

v4 vec4(f32 x, f32 y, f32 z, f32 w) {
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}