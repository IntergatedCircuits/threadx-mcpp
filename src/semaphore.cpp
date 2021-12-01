/**
 * @file      semaphore.cpp
 * @brief     ThreadX semaphore API abstraction
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
#include "threadx/semaphore.h"

using namespace threadx;
using namespace threadx::native;

bool semaphore::get(tick_timer::duration timeout)
{
    auto result = tx_semaphore_get(this, to_ticks(timeout));
    return (result == TX_SUCCESS);
}

bool semaphore::put(count_type update)
{
    // the API only allows giving a single count
    bool success = true;
    while (success && (update > 0))
    {
        success = TX_SUCCESS == tx_semaphore_put(this);
        update--;
    }
    return success;
}

semaphore::count_type semaphore::get_count() const
{
    return tx_semaphore_count;
}

semaphore::semaphore(count_type max, count_type desired, const char* name)
{
    auto result = tx_semaphore_create(this, const_cast<char*>(name), desired);
    assert(result == TX_SUCCESS);
}
