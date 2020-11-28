/**
 * UpIot
 */

#define NRF_LOG_MODULE_NAME up

#include "uplog.h"
#include "sdk_common.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

void up_log_init( void )
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void up_log_0(uint32_t severity, char const * const p_str)
{
    nrf_log_frontend_std_0(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str);
}

void up_log_1(uint32_t severity, char const * const p_str, uint32_t val0)
{
    nrf_log_frontend_std_1(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0);
}

void up_log_2(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1)
{
    nrf_log_frontend_std_2(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0, val1);
}

void up_log_3(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2)
{
    nrf_log_frontend_std_3(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0, val1, val2);
}

void up_log_4(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3)
{
    nrf_log_frontend_std_4(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0, val1, val2, val3);
}

void up_log_5(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4)
{
    nrf_log_frontend_std_5(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0, val1, val2, val3, val4);
}

void up_log_6(uint32_t severity, char const * const p_str, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4, uint32_t val5)
{
    nrf_log_frontend_std_6(severity | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_str, val0, val1, val2, val3, val4, val5);
}

void up_log_hexdump(uint32_t severity_mid, const void * const p_data, uint16_t length)
{
    nrf_log_frontend_hexdump(severity_mid | NRF_LOG_MODULE_ID << NRF_LOG_MODULE_ID_POS, p_data, length);
}