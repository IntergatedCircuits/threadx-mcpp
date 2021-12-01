/**
 * @file      cpu.cpp
 * @brief     ThreadX CPU related API abstraction
 * @author    Benedek Kupper
 *
 * Copyright (c) 2021 Benedek Kupper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "threadx/cpu.h"

namespace threadx
{
    namespace native
    {
        #include "tx_initialize.h"
        #include "tx_thread.h"
    }
}
using namespace threadx;
using namespace threadx::native;

cpu::critical_section::critical_section()
{
    memset(this, 0, sizeof(*this));
}

void cpu::critical_section::lock()
{
    TX_DISABLE
}

void cpu::critical_section::unlock()
{
    TX_RESTORE
}

bool this_cpu::is_in_isr()
{
    auto system_state = TX_THREAD_GET_SYSTEM_STATE();
    return (system_state != 0) && (system_state < TX_INITIALIZE_IN_PROGRESS);
}
