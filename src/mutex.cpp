/**
 * @file      mutex.cpp
 * @brief     ThreadX mutex API abstraction
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
#include "threadx/mutex.h"

using namespace threadx;
using namespace threadx::native;

bool mutex::get(tick_timer::duration timeout)
{
    auto result = tx_mutex_get(this, to_ticks(timeout));
    return (result == TX_SUCCESS);
}

void mutex::unlock()
{
    auto result = tx_mutex_put(this);
    assert(result == TX_SUCCESS);
}

thread *mutex::get_locking_thread() const
{
    return reinterpret_cast<thread*>(tx_mutex_owner);
}

mutex::mutex()
{
    static const bool priority_inheritance = true;
    tx_mutex_create(this, const_cast<char*>(DEFAULT_NAME), priority_inheritance ? TX_INHERIT : TX_NO_INHERIT);
}
