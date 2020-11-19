
/**
 * upiot
 */

#ifndef _UPIOT_LOG_H_
#define _UPIOT_LOG_H_

// TODO: implement in future to decouple the dependency of nRF SDK
#include "nrf_log.h"

typedef enum
{
    UP_LOG_SEVERITY_NONE,
    UP_LOG_SEVERITY_ERROR,
    UP_LOG_SEVERITY_WARNING,
    UP_LOG_SEVERITY_INFO,
    UP_LOG_SEVERITY_DEBUG,
    UP_LOG_SEVERITY_INFO_RAW, /* Artificial level to pass information about skipping string postprocessing.*/
} up_log_severity_t;

void up_log_0(uint32_t severity, char const * const p_str);
void up_log_1(uint32_t severity, char const * const p_str, uint32_t val0);
void up_log_2(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1);
void up_log_3(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2);
void up_log_4(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3);
void up_log_5(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4);
void up_log_6(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4, uint32_t val5);
void up_log_hexdump(uint32_t severity_mid, const void * const p_data, uint16_t length);

#define UP_LOG_INTERNAL_X(N, ...)          CONCAT_2(UP_LOG_INTERNAL_, N) (__VA_ARGS__)
#define UP_LOG_INTERNAL(type, ...) UP_LOG_INTERNAL_X(NUM_VA_ARGS_LESS_1( \
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

#define UP_LOG_HEXDUMP_INFO(p_data, length) UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_INFO, (p_data), (length))
#define UP_LOG_HEXDUMP_DEBUG(p_data, length) UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_DEBUG, (p_data), (length))
#define UP_LOG_HEXDUMP_WARN(p_data, length) UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_WARNING, (p_data), (length))
#define UP_LOG_HEXDUMP_ERROR(p_data, length) UP_LOG_HEXDUMP_INTERNAL(UP_LOG_SEVERITY_ERROR, (p_data), (length))

void up_log_init( void );

#endif // _UPIOT_LOG_H_