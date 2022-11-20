/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019-2020  natinusala
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

#include "cheat_sheet_tab.h"

#include <borealis.hpp>

CheatSheetTab::CheatSheetTab()
{
    // CPU
    this->addView(new brls::Header("CPU 频率"));
    brls::Table *cpuTable = new brls::Table();

    cpuTable->addRow(brls::TableRowType::BODY, "最大", "1785 MHz");
    cpuTable->addRow(brls::TableRowType::BODY, "官方底座和掌机最大", "1020 MHz");

    this->addView(cpuTable);

    // GPU
    this->addView(new brls::Header("GPU 频率"));
    brls::Table *gpuTable = new brls::Table();

    gpuTable->addRow(brls::TableRowType::BODY, "最大", "921 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "底座最大", "768 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "掌机最大", "460 MHz");
    gpuTable->addRow(brls::TableRowType::BODY, "官方掌机", "384 MHz");

    this->addView(gpuTable);

    // MEM
    this->addView(new brls::Header("内存 频率"));
    brls::Table *memTable = new brls::Table();

    memTable->addRow(brls::TableRowType::BODY, "官方底座最大", "1600 MHz");
    memTable->addRow(brls::TableRowType::BODY, "官方掌机最大", "1331 MHz");

    this->addView(memTable);
}

void CheatSheetTab::customSpacing(brls::View* current, brls::View* next, int* spacing)
{
    if (dynamic_cast<brls::Table*>(current))
        *spacing = 0;
    else
        List::customSpacing(current, next, spacing);
}
