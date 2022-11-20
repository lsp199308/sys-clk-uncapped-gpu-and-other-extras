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

typedef enum {
    SysClkConfigValue_PollingIntervalMs = 0,
    SysClkConfigValue_TempLogIntervalMs,
    SysClkConfigValue_CsvWriteIntervalMs,
    SysClkConfigValue_UncappedGPUEnabled,
    SysClkConfigValue_FakeProfileModeEnabled,
    SysClkConfigValue_OverrideCPUBoostEnabled,
    SysClkConfigValue_OverrideGPUBoostEnabled,
    SysClkConfigValue_OverrideMEMEnabled,
    SysClkConfigValue_EnumMax,
} SysClkConfigValue;

typedef struct {
    uint64_t values[SysClkConfigValue_EnumMax];
} SysClkConfigValueList;

static inline const char* sysclkFormatConfigValue(SysClkConfigValue val, bool pretty)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return pretty ? "轮询间隔 (ms)" : "poll_interval_ms";
        case SysClkConfigValue_TempLogIntervalMs:
            return pretty ? "温度间隔 (ms)" : "temp_log_interval_ms";
        case SysClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "CSV 写入间隔 (ms)" : "csv_write_interval_ms";
        case SysClkConfigValue_UncappedGPUEnabled:
            return pretty ? "无上限的GPU (不更改配置文件)" : "uncapped_gpu_enabled";
        case SysClkConfigValue_FakeProfileModeEnabled:
            return pretty ? "最小配置文件（欺骗配置文件）" : "fake_profile_mode_enabled";
        case SysClkConfigValue_OverrideCPUBoostEnabled:
            return pretty ? "升压期间将CPU设置为1785 MHz" : "override_cpu_boost_enabled";
        case SysClkConfigValue_OverrideGPUBoostEnabled:
            return pretty ? "升压期间将GPU设置为76 MHz" : "override_gpu_boost_enabled";
        case SysClkConfigValue_OverrideMEMEnabled:
            return pretty ? "将MEM设置至1600 MHz" : "override_mem_enabled";
        default:
            return NULL;
    }
}

static inline uint64_t sysclkDefaultConfigValue(SysClkConfigValue val)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return 300ULL;
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
        case SysClkConfigValue_UncappedGPUEnabled:
        case SysClkConfigValue_FakeProfileModeEnabled:
        case SysClkConfigValue_OverrideMEMEnabled:
            return 0ULL;
        case SysClkConfigValue_OverrideCPUBoostEnabled:
        case SysClkConfigValue_OverrideGPUBoostEnabled:
            return 1ULL;
        default:
            return 0ULL;
    }
}

static inline uint64_t sysclkValidConfigValue(SysClkConfigValue val, uint64_t input)
{
    switch(val)
    {
        case SysClkConfigValue_PollingIntervalMs:
            return input > 0;
        case SysClkConfigValue_UncappedGPUEnabled:
        case SysClkConfigValue_OverrideCPUBoostEnabled:
        case SysClkConfigValue_OverrideGPUBoostEnabled:
        case SysClkConfigValue_OverrideMEMEnabled:
            return (input == 0 || input == 1);
        case SysClkConfigValue_FakeProfileModeEnabled:
            return (input >=  0 && input < 5);
        case SysClkConfigValue_TempLogIntervalMs:
        case SysClkConfigValue_CsvWriteIntervalMs:
            return true;
        default:
            return false;
    }
}
