/**
 * @file      cpu.h
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
#ifndef __THREADX_CPU_H_
#define __THREADX_CPU_H_

#include "threadx/stdlib.h"

namespace threadx
{
    namespace native
    {
        #include "tx_api.h"
    }

    class cpu
    {
    public:
        /// @brief  A @ref BasicLockable class that prevents task and interrupt
        ///         context switches while locked.
        class critical_section
        {
        public:
            critical_section();

            /// @brief  Locks the CPU, preventing thread and interrupt switches.
            void lock();

            /// @brief  Unlocks the CPU, allowing other interrupts and threads
            ///         to preempt the current execution context.
            void unlock();

        private:
            TX_INTERRUPT_SAVE_AREA
        };
    };

    namespace this_cpu
    {
        /// @brief  Determines if the current execution context is inside
        ///         an interrupt service routine.
        /// @return true if the current execution context is ISR, false otherwise
        bool is_in_isr();
    }
}

#endif // __THREADX_CPU_H_
