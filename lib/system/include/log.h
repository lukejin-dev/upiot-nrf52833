
/**
 * upiot
 */

#ifndef _UPIOT_LOG_H_
#define _UPIOT_LOG_H_

#include <stdint.h>

typedef enum
{
    UP_LOG_SEVERITY_NONE,
    UP_LOG_SEVERITY_ERROR,
    UP_LOG_SEVERITY_WARNING,
    UP_LOG_SEVERITY_INFO,
    UP_LOG_SEVERITY_DEBUG,
    UP_LOG_SEVERITY_INFO_RAW, /* Artificial level to pass information about skipping string postprocessing.*/
} up_log_severity_t;

#define UP_CONCAT_2(p1, p2)      UP_CONCAT_2_(p1, p2)
/** Auxiliary macro used by @ref CONCAT_2 */
#define UP_CONCAT_2_(p1, p2)     p1##p2

/**@brief Implementation details for NUM_VAR_ARGS */
#define UP_NUM_VA_ARGS_LESS_1_IMPL(                       \
    _ignored,                                          \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,  \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,  \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,  \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,  \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60,  \
    _61, _62, N, ...) N

/**@brief Macro to get the number of arguments in a call variadic macro call.
 * First argument is not counted.
 *
 * param[in]    ...     List of arguments
 *
 * @retval  Number of variadic arguments in the argument list
 */
#define UP_NUM_VA_ARGS_LESS_1(...) UP_NUM_VA_ARGS_LESS_1_IMPL(__VA_ARGS__, 63, 62, 61,  \
    60, 59, 58, 57, 56, 55, 54, 53, 52, 51,                         \
    50, 49, 48, 47, 46, 45, 44, 43, 42, 41,                         \
    40, 39, 38, 37, 36, 35, 34, 33, 32, 31,                         \
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21,                         \
    20, 19, 18, 17, 16, 15, 14, 13, 12, 11,                         \
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ~)

void up_log_0(uint32_t severity, char const * const p_str);
void up_log_1(uint32_t severity, char const * const p_str, uint32_t val0);
void up_log_2(uint32_t severity, char const * const p_str, uint32_t val0,
    uint32_t val1);
void up_log_3(uint32_t severity, char const * const p_str, uint32_t val0,
    uint32_t val1, uint32_t val2);
void up_log_4(uint32_t severity, char const * const p_str, uint32_t val0,
    uint32_t val1, uint32_t val2, uint32_t val3);
void up_log_5(uint32_t severity, char const * const p_str, uint32_t val0,
    uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4);
void up_log_6(uint32_t severity, char const * const p_str, uint32_t val0,
    uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4, uint32_t val5);
void up_log_hexdump(uint32_t severity_mid, const void * const p_data,
    uint16_t length);

#define UP_LOG_INTERNAL_X(N, ...)          UP_CONCAT_2(UP_LOG_INTERNAL_, N) (__VA_ARGS__)
#define UP_LOG_INTERNAL(type, ...) UP_LOG_INTERNAL_X(UP_NUM_VA_ARGS_LESS_1( \
                                                           __VA_ARGS__), type, __VA_ARGS__)

#define UP_LOG_INTERNAL_0(type, str) \
    up_log_0(type, str)
#define UP_LOG_INTERNAL_1(type, str, arg0) \
    /*lint -save -e571*/up_log_1(type, str, (uint32_t)(arg0))/*lint -restore*/
#define UP_LOG_INTERNAL_2(type, str, arg0, arg1) \
    /*lint -save -e571*/up_log_2(type, str, (uint32_t)(arg0), \
            (uint32_t)(arg1))/*lint -restore*/
#define UP_LOG_INTERNAL_3(type, str, arg0, arg1, arg2) \
    /*lint -save -e571*/up_log_3(type, str, (uint32_t)(arg0), \
            (uint32_t)(arg1), (uint32_t)(arg2))/*lint -restore*/
#define UP_LOG_INTERNAL_4(type, str, arg0, arg1, arg2, arg3) \
    /*lint -save -e571*/up_log_4(type, str, (uint32_t)(arg0), \
            (uint32_t)(arg1), str, (uint32_t)(arg2), (uint32_t)(arg3))/*lint -restore*/
#define UP_LOG_INTERNAL_5(type, str, arg0, arg1, arg2, arg3, arg4) \
    /*lint -save -e571*/up_log_5(type, str, (uint32_t)(arg0), \
            (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3), (uint32_t)(arg4))/*lint -restore*/
#define UP_LOG_INTERNAL_6(type, str, arg0, arg1, arg2, arg3, arg4, arg5) \
    /*lint -save -e571*/up_log_6(type, str, (uint32_t)(arg0), \
            (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3), (uint32_t)(arg4), (uint32_t)(arg5))/*lint -restore*/

#define UP_LOG_LEVEL UP_LOG_SEVERITY_DEBUG

#define UP_LOG_COMMON(level, ...)                       \
    if (UP_LOG_LEVEL >= level)                          \
    {                                                   \
        UP_LOG_INTERNAL(level, __VA_ARGS__);            \
    }                                                   \

#define UP_INFO(...)     UP_LOG_COMMON(UP_LOG_SEVERITY_INFO, __VA_ARGS__)
#define UP_DEBUG(...)    UP_LOG_COMMON(UP_LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define UP_WARN(...)     UP_LOG_COMMON(UP_LOG_SEVERITY_WARNING, __VA_ARGS__)
#define UP_ERROR(...)    UP_LOG_COMMON(UP_LOG_SEVERITY_ERROR, __VA_ARGS__)

void up_log_hexdump(uint32_t severity_mid, const void * const p_data, uint16_t length);

#define UP_LOG_HEXDUMP_INTERNAL(level, p_data, length)      \
    if (UP_LOG_LEVEL >= level)                              \
    {                                                       \
        up_log_hexdump((level), (p_data), (length));  \
    }                                                       \

#define UP_HEXDUMP_INFO(p_data, length) \
    UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_INFO, (p_data), (length))
#define UP_HEXDUMP_DEBUG(p_data, length) \
    UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_DEBUG, (p_data), (length))
#define UP_HEXDUMP_WARN(p_data, length) \
    UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_WARNING, (p_data), (length))
#define UP_HEXDUMP_ERROR(p_data, length) \
    UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_ERROR, (p_data), (length))

void up_log_init( void );

#endif // _UPIOT_LOG_H_