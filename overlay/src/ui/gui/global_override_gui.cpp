/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "global_override_gui.h"

#include "fatal_gui.h"
#include "../format.h"

GlobalOverrideGui::GlobalOverrideGui()
{
    for(std::uint16_t m = 0; m < SysClkModule_EnumMax; m++)
    {
        this->listItems[m] = nullptr;
        this->listHz[m] = 0;
    }
    
    for(std::uint16_t m = 0; m < SysClkConfigValue_EnumMax; m++)
    {
        this->ToggleListItems[m] = nullptr;
        
        this->customListItems[m] = nullptr;
        this->customListProfiles[m] = 0;
    }
}

void GlobalOverrideGui::openFreqChoiceGui(SysClkModule module, std::uint32_t* hzList)
{
    tsl::changeTo<FreqChoiceGui>(this->context->overrideFreqs[module], hzList, [this, module](std::uint32_t hz) {
        Result rc = sysclkIpcSetOverride(module, hz);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetOverride", rc);
            return false;
        }

        this->lastContextUpdate = armGetSystemTick();
        this->context->overrideFreqs[module] = hz;

        return true;
    });
}

void GlobalOverrideGui::openProfileChoiceGui(int configNumber, std::uint32_t* profileList)
{
    sysclkIpcGetConfigValues(&this->configValues);

    SysClkConfigValue config = (SysClkConfigValue) configNumber;
    
    tsl::changeTo<ProfileChoiceGui>((uint64_t) configValues.values[config], profileList, [this,config](std::uint32_t profile) {
        
        uint64_t uvalue = (uint64_t) profile;
        
        // Save the config
        this->configValues.values[config] = uvalue;
        
        Result rc =  sysclkIpcSetConfigValues(&this->configValues);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
            return false;
            
        }
        
        this->lastContextUpdate = armGetSystemTick();
        return true;
        
        
    });
}

void GlobalOverrideGui::addModuleListItem(SysClkModule module, std::uint32_t* hzList)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(sysclkFormatModule(module, true));
    listItem->setValue(formatListFreqMhz(0));

    listItem->setClickListener([this, module, hzList](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openFreqChoiceGui(module, hzList);
            return true;
        }

        return false;
    });

    this->listElement->addItem(listItem);
    this->listItems[module] = listItem;
}

void GlobalOverrideGui::addCustomListItem(int configNumber,std::string shortLabel, std::uint32_t* profileList)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(shortLabel);
    listItem->setValue(formatListProfile(0));

    listItem->setClickListener([this, profileList,configNumber](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            this->openProfileChoiceGui(configNumber,profileList);
            return true;
        }

        return false;
    });

    this->listElement->addItem(listItem);
    this->customListItems[configNumber] = listItem;
}

void GlobalOverrideGui::addCustomToggleListItem(int configNumber,std::string shortLabel)
{
       
    sysclkIpcGetConfigValues(&this->configValues);

    SysClkConfigValue config = (SysClkConfigValue) configNumber;
    
    uint64_t defaultValue   = configValues.values[config];
    
    bool defValue = false;
    
    if(defaultValue==1) {
        defValue = true;
    }

    this->enabledToggle = new tsl::elm::ToggleListItem(shortLabel, defValue,"是","否");

    enabledToggle->setStateChangedListener([this,config](bool state) {
            
            int value = 0;
            
            if(state) {
                value = 1;
            }
            
            uint64_t uvalue = (uint64_t) value;
            
            // Save the config
            this->configValues.values[config] = uvalue;
        
            Result rc =  sysclkIpcSetConfigValues(&this->configValues);
            if(R_FAILED(rc))
            {
                FatalGui::openWithResultCode("sysclkIpcSetConfigValues", rc);
                return false;
            }
            
            this->lastContextUpdate = armGetSystemTick();
            return true;
    });
    
    this->listElement->addItem(this->enabledToggle);
    this->ToggleListItems[configNumber] = this->enabledToggle;
}

void GlobalOverrideGui::listUI()
{
    this->addModuleListItem(SysClkModule_CPU, &sysclk_g_freq_table_cpu_hz[0]);
    this->addModuleListItem(SysClkModule_GPU, &sysclk_g_freq_table_gpu_hz[0]);
    this->addModuleListItem(SysClkModule_MEM, &sysclk_g_freq_table_mem_hz[0]);
    //added 5 custom configs
    this->addCustomToggleListItem(3,"无上限的GPU");
    this->addCustomListItem(4,"最小模式",&sysclk_g_profile_table[0]);
    this->addCustomToggleListItem(5,"CPU提升至1785Mhz（加载游戏）");
    this->addCustomToggleListItem(6,"GPU降低至76Mhz（加载游戏）");
    this->addCustomToggleListItem(7,"超频内存至1600Mhz");
}

void GlobalOverrideGui::refresh()
{
    BaseMenuGui::refresh();

    if(this->context)
    {
        for(std::uint16_t m = 0; m < SysClkModule_EnumMax; m++)
        {
            if(this->listItems[m] != nullptr && this->listHz[m] != this->context->overrideFreqs[m])
            {
                this->listItems[m]->setValue(formatListFreqHz(this->context->overrideFreqs[m]));
                this->listHz[m] = this->context->overrideFreqs[m];
            }
        }
        

        
        for(std::uint16_t m = 3; m < SysClkConfigValue_EnumMax; m++)
        {
            
            if(m == 4) {

                sysclkIpcGetConfigValues(&this->configValues);
            
                SysClkConfigValue config = (SysClkConfigValue) m;
            
                uint32_t defaultValue   = configValues.values[config];
                
                
                if(this->customListItems[m] != nullptr && this->customListProfiles[m] != defaultValue)
                {
                    this->customListItems[m]->setValue(formatListProfile(defaultValue));
                    this->customListProfiles[m] = defaultValue;
                }
                
            } else {
 
                if(this->ToggleListItems[m] != nullptr)
                {
                    sysclkIpcGetConfigValues(&this->configValues);
                
                    SysClkConfigValue config = (SysClkConfigValue) m;
                
                    uint64_t defaultValue   = configValues.values[config];
                    
                    bool defValue = false;
                    
                    if(defaultValue==1) {
                        defValue = true;
                    }
                    
                    this->ToggleListItems[m]->setState(defValue);
                
                }
            
            }
            

        }
        
    }
}
