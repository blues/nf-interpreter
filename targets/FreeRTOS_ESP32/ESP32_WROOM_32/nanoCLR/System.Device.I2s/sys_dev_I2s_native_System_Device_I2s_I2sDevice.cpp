//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include "sys_dev_I2s_native.h"
#include "Esp32_DeviceMapping.h"

typedef Library_sys_dev_I2s_native_System_Device_I2s_I2sConnectionSettings I2sConnectionSettings;

static char Esp_I2S_Initialised_Flag[I2S_NUM_MAX] = {0, 0};

void Esp32_I2s_UnitializeAll()
{
    for (int c = 0; c < I2S_NUM_MAX; c++)
    {
        if (Esp_I2C_Initialised_Flag[c])
        {
            // Delete bus driver
            i2s_driver_delete((i2c_port_t)c);
            Esp_I2S_Initialised_Flag[c] = 0;
        }
    }
}

void SetI2sConfig(i2c_port_t bus, CLR_RT_HeapBlock *config)
{
    i2s_pin_config_t pin_config;
    pin_config.mck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 0);
    pin_config.bck_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 1);
    pin_config.ws_io_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 2);
    pin_config.data_out_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 3);
    pin_config.data_in_num = (gpio_num_t)Esp32_GetMappedDevicePins(DEV_TYPE_I2S, bus, 4);

    i2s_config_t conf;
    conf.mode = config[I2sConnectionSettings::FIELD___i2sMode].NumericByRef().s4;
    conf.sample_rate = config[I2sConnectionSettings::FIELD___sampleRate].NumericByRef().s4;
    conf.bits_per_sample = config[I2sConnectionSettings::FIELD___i2sBitsPerSample].NumericByRef().s4;
    conf.channel_format = config[I2sConnectionSettings::FIELD___i2sChannelFormat].NumericByRef().s4;
    conf.communication_format = config[I2sConnectionSettings::FIELD___i2sConnectionFormat].NumericByRef().s4;
    // As recommended from the sample
    conf.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    // Default size used in samples
    conf.dma_buf_count = 64;
    conf.dma_buf_len = 1024;

    // If this is first device on Bus then init driver
    if (Esp_I2C_Initialised_Flag[bus] == 0)
    {
        // Install driver without events
        esp_err_t res = i2s_driver_install(bus, &conf, 0, NULL);
        if (res != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Ensure driver gets unitialized during soft reboot
        HAL_AddSoftRebootHandler(Esp32_I2s_UnitializeAll);

        Esp_I2C_Initialised_Flag[bus]++;
    }

    i2s_set_pin(bus, &pin_config);
    i2s_set_clk(bus, conf.sample_rate, conf.bits_per_sample, conf.channel_format);
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Read___VOID__SystemSpanByte( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Read___VOID__SZARRAY_U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Write___VOID__SZARRAY_U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::Write___VOID__SystemSpanByte( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::NativeInit___VOID( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pConfig;
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2c_port_t bus = (i2c_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        if (bus != I2C_NUM_0 && bus != I2C_NUM_1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Set the Bus parameters
        SetConfig(bus, pConfig);        
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice::NativeDispose___VOID( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pConfig;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a pointer to the managed spi connectionSettings object instance
        pConfig = pThis[FIELD___connectionSettings].Dereference();

        // get bus index
        // subtract 1 to get ESP32 bus number
        i2s_port_t bus = (isc_port_t)(pConfig[I2sConnectionSettings::FIELD___busId].NumericByRef().s4 - 1);

        Esp_I2S_Initialised_Flag[bus]--;

        if (Esp_I2S_Initialised_Flag[bus] <= 0)
        {
            i2s_driver_uninstall(bus);

            Esp_I2S_Initialised_Flag[bus] = 0;
        }
    }
    NANOCLR_NOCLEANUP();
}