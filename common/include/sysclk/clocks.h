/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    SysClkProfile_Handheld = 0,
    SysClkProfile_HandheldCharging,
    SysClkProfile_HandheldChargingUSB,
    SysClkProfile_HandheldChargingOfficial,
    SysClkProfile_Docked,
    SysClkProfile_EnumMax
} SysClkProfile;

typedef enum
{
    SysClkModule_CPU = 0,
    SysClkModule_GPU,
    SysClkModule_MEM,
    SysClkModule_EnumMax
} SysClkModule;

typedef enum
{
    SysClkThermalSensor_SOC = 0,
    SysClkThermalSensor_PCB,
    SysClkThermalSensor_Skin,
    SysClkThermalSensor_EnumMax
} SysClkThermalSensor;

typedef struct
{
    uint8_t enabled;
    uint64_t applicationId;
    SysClkProfile profile;
    uint32_t freqs[SysClkModule_EnumMax];
    uint32_t overrideFreqs[SysClkModule_EnumMax];
    uint32_t temps[SysClkThermalSensor_EnumMax];
} SysClkContext;

typedef struct
{
    union {
        uint32_t mhz[SysClkProfile_EnumMax * SysClkModule_EnumMax];
        uint32_t mhzMap[SysClkProfile_EnumMax][SysClkModule_EnumMax];
    };
} SysClkTitleProfileList;

#define SYSCLK_GPU_HANDHELD_MAX_HZ 460800000
#define SYSCLK_GPU_UNOFFICIAL_CHARGER_MAX_HZ 921000000

extern uint32_t sysclk_g_profile_table[];
extern uint32_t sysclk_g_freq_table_mem_hz[];
extern uint32_t sysclk_g_freq_table_cpu_hz[];
extern uint32_t sysclk_g_freq_table_gpu_hz[];

#define SYSCLK_ENUM_VALID(n, v) ((v) < n##_EnumMax)

static inline const char* sysclkFormatModule(SysClkModule module, bool pretty)
{
    switch(module)
    {
        case SysClkModule_CPU:
            return pretty ? "CPU" : "cpu";
        case SysClkModule_GPU:
            return pretty ? "GPU" : "gpu";
        case SysClkModule_MEM:
            return pretty ? "内存" : "mem";
        default:
            return NULL;
    }
}

static inline const char* sysclkFormatThermalSensor(SysClkThermalSensor thermSensor, bool pretty)
{
    switch(thermSensor)
    {
        case SysClkThermalSensor_SOC:
            return pretty ? "核心" : "soc";
        case SysClkThermalSensor_PCB:
            return pretty ? "主板" : "pcb";
        case SysClkThermalSensor_Skin:
            return pretty ? "外壳" : "skin";
        default:
            return NULL;
    }
}

static inline const char* sysclkFormatProfile(SysClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case SysClkProfile_Docked:
            return pretty ? "底座" : "docked";
        case SysClkProfile_Handheld:
            return pretty ? "掌机" : "handheld";
        case SysClkProfile_HandheldCharging:
            return pretty ? "充电" : "handheld_charging";
        case SysClkProfile_HandheldChargingUSB:
            return pretty ? "USB充电" : "handheld_charging_usb";
        case SysClkProfile_HandheldChargingOfficial:
            return pretty ? "官方充电" : "handheld_charging_official";
        default:
            return NULL;
    }
}
