/*!
 * \file      gpio.c
 *
 * \brief     GPIO driver implementation
 *
 * \remark: Relies on the specific board GPIO implementation as well as on
 *          IO expander driver implementation if one is available on the target
 *          board.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <nrfx_gpiote.h>
#include "board.h"
#include "gpio.h"

static Gpio_t *GpioIrq[IOE_0];

static void GpioIrqCallback(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if ((GpioIrq[pin] != NULL) && (GpioIrq[pin]->IrqHandler != NULL))
    {
        GpioIrq[pin]->IrqHandler(GpioIrq[pin]->Context);
    }
}

void GpioInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value)
{
    if (pin < IOE_0)
    {
        obj->pin = pin;
        // obj->pinIndex = pin;
        obj->pull = type;

        if (pin == NC)
        {
            return;
        }

        nrf_gpio_pin_dir_t Pin_Direction = NRF_GPIO_PIN_DIR_INPUT;
        nrf_gpio_pin_drive_t Pin_Output_Mode = NRF_GPIO_PIN_S0S1;
        nrf_gpio_pin_pull_t Pin_Pull_Type = NRF_GPIO_PIN_NOPULL;
        nrf_gpio_pin_input_t Pin_Connect_Input_Buffer = NRF_GPIO_PIN_INPUT_DISCONNECT;

        if (mode == PIN_INPUT)
        {
            Pin_Direction = NRF_GPIO_PIN_DIR_INPUT;
            Pin_Connect_Input_Buffer = NRF_GPIO_PIN_INPUT_CONNECT;
        }
        else if (mode == PIN_OUTPUT)
        {
            Pin_Direction = NRF_GPIO_PIN_DIR_OUTPUT;
            Pin_Connect_Input_Buffer = NRF_GPIO_PIN_INPUT_DISCONNECT;
        }
        else
        {
            ASSERT(FAIL);
        }

        if (config == PIN_S0S1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_S0S1;
        }
        else if (config == PIN_H0S1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_H0S1;
        }
        else if (config == PIN_S0H1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_S0H1;
        }
        else if (config == PIN_H0H1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_H0H1;
        }
        else if (config == PIN_D0S1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_D0S1;
        }
        else if (config == PIN_D0H1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_D0H1;
        }
        else if (config == PIN_S0D1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_S0D1;
        }
        else if (config == PIN_H0D1)
        {
            Pin_Output_Mode = NRF_GPIO_PIN_H0D1;
        }
        else
        {
            ASSERT(FAIL);
        }

        if (type == PIN_NO_PULL)
        {
            Pin_Pull_Type = NRF_GPIO_PIN_NOPULL;
        }
        else if (type == PIN_PULL_UP)
        {
            Pin_Pull_Type = NRF_GPIO_PIN_PULLUP;
        }
        else if (type == PIN_PULL_DOWN)
        {
            Pin_Pull_Type = NRF_GPIO_PIN_PULLDOWN;
        }
        else
        {
            ASSERT(FAIL);
        }

        nrf_gpio_cfg(
            obj->pin,
            Pin_Direction,
            Pin_Connect_Input_Buffer,
            Pin_Pull_Type,
            Pin_Output_Mode,
            NRF_GPIO_PIN_NOSENSE);

        // Sets initial output value
        if (mode == PIN_OUTPUT)
        {
            GpioWrite(obj, value);
        }
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeInit(obj, pin, mode, config, type, value);
#endif
    }
}

void GpioDeInit(Gpio_t *obj)
{
    if (obj->pin == NC)
    {
        return;
    }

    if (obj->pin < IOE_0)
    {
        nrf_gpio_cfg_default(obj->pin);
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeDeInit(obj);
#endif
    }
}

void GpioSetContext(Gpio_t *obj, void *context)
{
    obj->Context = context;
}

void GpioSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqAccuracys irqAccuracy,
    GpioIrqHandler *irqHandler)
{
    if (obj->pin == NC)
    {
        ASSERT(FAIL);
    }

    if (obj->pin < IOE_0)
    {
        nrfx_gpiote_in_config_t Pin_Gpiote_Config;

        if (irqHandler == NULL)
        {
            return;
        }

        obj->IrqHandler = irqHandler;

        if (nrfx_gpiote_is_init() == false)
        {
            APP_ERROR_CHECK(nrfx_gpiote_init());
        }

        if (irqMode == IRQ_RISING_EDGE)
        {
            Pin_Gpiote_Config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
        }
        else if (irqMode == IRQ_FALLING_EDGE)
        {
            Pin_Gpiote_Config.sense = NRF_GPIOTE_POLARITY_HITOLO;
        }
        else if (irqMode == IRQ_RISING_FALLING_EDGE)
        {
            Pin_Gpiote_Config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
        }
        else
        {
            ASSERT(FAIL);
        }

        if (irqAccuracy == IRQ_ACCURACY_HIGH)
        {
            Pin_Gpiote_Config.hi_accuracy = true;
        }
        else if (irqAccuracy == IRQ_ACCURACY_LOW)
        {
            Pin_Gpiote_Config.hi_accuracy = false;
        }
        else
        {
            ASSERT(FAIL);
        }

        Pin_Gpiote_Config.is_watcher = true;
        Pin_Gpiote_Config.pull = NRF_GPIO_PIN_NOPULL;
        Pin_Gpiote_Config.skip_gpio_setup = false;

        GpioIrq[(obj->pin)] = obj;

        APP_ERROR_CHECK(nrfx_gpiote_in_init(obj->pin, &Pin_Gpiote_Config, GpioIrqCallback));
        nrfx_gpiote_in_event_enable(obj->pin, true);
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeSetInterrupt(obj, irqMode, irqPriority, irqHandler);
#endif
    }
}

void GpioRemoveInterrupt(Gpio_t *obj)
{
    if (obj->pin < IOE_0)
    {
        // Clear callback before changing pin mode
        GpioIrq[(obj->pin)] = NULL;

        if (nrfx_gpiote_is_init() == true)
        {
            nrfx_gpiote_uninit();
        }
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeRemoveInterrupt(obj);
#endif
    }
}

void GpioWrite(Gpio_t *obj, uint32_t value)
{
    if (obj->pin < IOE_0)
    {
        if (obj == NULL)
        {
            ASSERT(FAIL);
        }
        // Check if pin is not connected
        if (obj->pin == NC)
        {
            return;
        }
        nrf_gpio_pin_write(obj->pin, value);
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeWrite(obj, value);
#endif
    }
}

void GpioToggle(Gpio_t *obj)
{
    if (obj->pin < IOE_0)
    {
        if (obj == NULL)
        {
            ASSERT(FAIL);
        }
        // Check if pin is not connected
        if (obj->pin == NC)
        {
            return;
        }
        nrf_gpio_pin_toggle(obj->pin);
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        GpioIoeToggle(obj);
#endif
    }
}

uint32_t GpioRead(Gpio_t *obj)
{
    if (obj->pin < IOE_0)
    {
        if (obj == NULL)
        {
            ASSERT(FAIL);
        }
        // Check if pin is not connected
        if (obj->pin == NC)
        {
            return 0;
        }
        return nrf_gpio_pin_read(obj->pin);
    }
    else
    {
#if defined(BOARD_IOE_EXT)
        // IOExt Pin
        return GpioIoeRead(obj);
#else
        return 0;
#endif
    }
}
