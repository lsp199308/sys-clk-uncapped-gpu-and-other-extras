 /*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam
    Copyright (C) 2019  m4xw

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "advanced_settings_tab.h"

#include "utils.h"

#include <sysclk/clocks.h>

AdvancedSettingsTab::AdvancedSettingsTab()
{
    // Get context
    SysClkContext context;
    Result rc = sysclkIpcGetCurrentContext(&context);

    if (R_FAILED(rc))
    {
        brls::Logger::error("无法获取上下文");
        errorResult("sysclkIpcGetCurrentContext", rc);
        brls::Application::crash("无法获取当前的sys-clk上下文，请检查它是否已正确安装和启用.");
        return;
    }

    // Create UI

    // Disclaimer
    this->addView(new brls::Label(brls::LabelStyle::REGULAR, "\uE140  请仅在您知道自己在做什么的情况下更改这些设置.", true));

    // Temporary overrides
    this->addView(new brls::Header("临时配置"));

    // CPU
    brls::SelectListItem *cpuFreqListItem = createFreqListItem(SysClkModule_CPU, context.overrideFreqs[SysClkModule_CPU] / 1000000);
    cpuFreqListItem->getValueSelectedEvent()->subscribe([](int result){
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_CPU) :
            sysclkIpcSetOverride(SysClkModule_CPU, sysclk_g_freq_table_cpu_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("无法更新CPU频率");
            errorResult(result == 0 ? "sysclkIpcRemoveOverride" : "sysclkIpcSetOverride",  rc);
            // TODO: Reset selected value
        }
    });

    // GPU
    brls::SelectListItem *gpuFreqListItem = createFreqListItem(SysClkModule_GPU, context.overrideFreqs[SysClkModule_GPU] / 1000000);
    gpuFreqListItem->getValueSelectedEvent()->subscribe([](int result){
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_GPU) :
            sysclkIpcSetOverride(SysClkModule_GPU, sysclk_g_freq_table_gpu_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("无法更新GPU频率");
            errorResult(result == 0 ? "sysclkIpcRemoveOverride" : "sysclkIpcSetOverride",  rc);
            // TODO: Reset selected value
        }
    });

    // MEM
    brls::SelectListItem *memFreqListItem = createFreqListItem(SysClkModule_MEM, context.overrideFreqs[SysClkModule_MEM] / 1000000);
    memFreqListItem->getValueSelectedEvent()->subscribe([](int result)
    {
        Result rc = result == 0 ?
            sysclkIpcRemoveOverride(SysClkModule_MEM) :
            sysclkIpcSetOverride(SysClkModule_MEM, sysclk_g_freq_table_mem_hz[result - 1]);

        if (R_FAILED(rc))
        {
            brls::Logger::error("无法更新内存频率");
            errorResult(result == 0 ? "sysclkIpcRemoveOverride" : "sysclkIpcSetOverride",  rc);
            // TODO: Reset selected value
        }
    });

    this->addView(cpuFreqListItem);
    this->addView(gpuFreqListItem);
    this->addView(memFreqListItem);

    // Config
    this->addView(new brls::Header("配置"));

    // Logging
    // TODO: add a logger view and put the button to enter it here

    // Config entries
    // TODO: add constraints to the swkbd if possible (min / max)

    sysclkIpcGetConfigValues(&this->configValues);

    for (int i = 0; i < 3; i++)
    {
        SysClkConfigValue config = (SysClkConfigValue) i;

        std::string label       = std::string(sysclkFormatConfigValue(config, true));
        std::string description = this->getDescriptionForConfig(config);
        uint64_t defaultValue   = configValues.values[config];

        brls::IntegerInputListItem* configItem = new brls::IntegerInputListItem(label, defaultValue, label, description);

        configItem->setReduceDescriptionSpacing(true);

        configItem->getClickEvent()->subscribe([this, configItem, config](View* view)
        {
            try
            {
                int value = std::stoi(configItem->getValue());

                // Validate the value
                if (value < 0)
                {
                    brls::Application::notify("\uE5CD 无法保存配置：无效值（为负值）");
                    configItem->setValue(std::to_string(this->configValues.values[config]));
                    return;
                }

                uint64_t uvalue = (uint64_t) value;

                if (!sysclkValidConfigValue(config, uvalue))
                {
                    brls::Application::notify("\uE5CD 无法保存配置：值无效");
                    configItem->setValue(std::to_string(this->configValues.values[config]));
                    return;
                }

                // Save the config
                this->configValues.values[config] = uvalue;
                sysclkIpcSetConfigValues(&this->configValues);

                brls::Application::notify("\uE14B 配置已保存");
            }
            catch(const std::exception& e)
            {
                brls::Logger::error("无法分析配置值 %s: %s", configItem->getValue().c_str(), e.what());
            }
        });

        this->addView(configItem);
    }
    
    //loop for the added 5 custom configs
    for (int i = 3; i < SysClkConfigValue_EnumMax; i++)
    {
        SysClkConfigValue config = (SysClkConfigValue) i;

        std::string label       = std::string(sysclkFormatConfigValue(config, true));
        
        uint64_t defaultValue   = configValues.values[config];
        
        if (i==4) {

            // Fake profile
            brls::SelectListItem *profileListItem = createProfileListItem(label, (uint32_t) defaultValue);
            profileListItem->getValueSelectedEvent()->subscribe([this,config,profileListItem](int result)
            {

                try
                {
                    
                    uint64_t uvalue = (uint64_t) sysclk_g_profile_table[result - 1];
                    
                    if (!sysclkValidConfigValue(config, uvalue))
                    {
                        brls::Application::notify("\uE5CD 无法保存配置：值无效");
                        profileListItem->setValue(sysclkFormatProfile((SysClkProfile)this->configValues.values[config],true) );
                        return;
                    }

                    // Save the config
                    this->configValues.values[config] = uvalue;
                    sysclkIpcSetConfigValues(&this->configValues);

                    brls::Application::notify("\uE14B 配置已保存");
                }
                catch(const std::exception& e)
                {
                    brls::Logger::error("无法分析配置值 %s: %s", profileListItem->getValue().c_str(), e.what());
                }
                
                
            });

            this->addView(profileListItem);

        } else {

            bool defValue = false;
            
            if(defaultValue==1) {
                defValue = true;
            }
        
            brls::ToggleListItem* configItem2 = new brls::ToggleListItem(label, defValue, "", "是", "否");
            
            configItem2->getClickEvent()->subscribe([this, configItem2, config](View* view)
            {
                try
                {
                    int value = 0;
                    std::string valuestring = configItem2->getValue();
                    
                    if(valuestring == "Yes") {
                        value = 1;
                    }
                    
                    uint64_t uvalue = (uint64_t) value;

                    if (!sysclkValidConfigValue(config, uvalue))
                    {
                        brls::Application::notify("\uE5CD 无法保存配置：值无效");
                        configItem2->setValue(std::to_string(this->configValues.values[config]));
                        return;
                    }

                    // Save the config
                    this->configValues.values[config] = uvalue;
                    sysclkIpcSetConfigValues(&this->configValues);

                    brls::Application::notify("\uE14B 配置已保存");
                }
                catch(const std::exception& e)
                {
                    brls::Logger::error("无法分析配置值 %s: %s", configItem2->getValue().c_str(), e.what());
                }
            });

            this->addView(configItem2);
            
        }
        
    }
    
    // Notes
    brls::Label *notes = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "选择的最低配置文件将确保您的设备至少保持在最低级别。它适用于那些在充电模式中具有更高超频的人，并且有时希望在没有充电器的情况下激活这些模式（例如，选择“官方充电器”将确保您的设备的配置文件至少保持在“官方充电器“，即使您的设备实际上是掌机模式）。如果实际配置文件高于最低配置文件，则选择的最低配置文件不会降级您的实际配置文件（例如，当底座时，选择“充电”没有任何作用（您已经处于较高的配置文件）).",
        true
    );
    this->addView(notes);
    
}

std::string AdvancedSettingsTab::getDescriptionForConfig(SysClkConfigValue config)
{
    switch (config)
    {
        case SysClkConfigValue_CsvWriteIntervalMs:
            return "更新/config/sys-clk/context.csv的频率 (毫秒)\n\uE016  使用0禁用";
        case SysClkConfigValue_TempLogIntervalMs:
            return "记录温度的频率（毫秒）\n\uE016  使用0禁用";
        case SysClkConfigValue_PollingIntervalMs:
            return "检查和应用配置文件的速度（毫秒）";
        default:
            return "";
    }
}
