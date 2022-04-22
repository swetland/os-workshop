#pragma once

#ifdef __cplusplus
// static_assert(a, b) already exists
#else
#define static_assert(a, b) _Static_assert(a, b)
#endif

