/**
 * @file      semaphore.h
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
#ifndef __THREADX_SEMAPHORE_H_
#define __THREADX_SEMAPHORE_H_

#include "threadx/tick_timer.h"

namespace threadx
{
    /// @brief  An abstract base class for semaphores. Implements std::counting_semaphore API.
    class semaphore : private native::TX_SEMAPHORE_STRUCT
    {
    public:
        using count_type = native::ULONG;

        /// @brief  Waits indefinitely until the semaphore is available, then takes it.
        inline void acquire()
        {
            (void)get(infinity);
        }

        /// @brief  Tries to take the semaphore if it is available.
        /// @return true if successful, false if the semaphore is unavailable
        inline bool try_acquire()
        {
            return get(tick_timer::duration(0));
        }

        /// @brief  Tries to take the semaphore within the given time duration.
        /// @param  rel_time: duration to wait for the semaphore to become available
        /// @return true if successful, false if the semaphore is unavailable
        template<class Rep, class Period>
        inline bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time)
        {
            return get(std::chrono::duration_cast<tick_timer::duration>(rel_time));
        }

        /// @brief  Tries to take the semaphore until the given deadline.
        /// @param  abs_time: deadline to wait until the semaphore becomes available
        /// @return true if successful, false if the semaphore is unavailable
        template<class Clock, class Duration>
        inline bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time)
        {
            return try_acquire_for(abs_time - Clock::now());
        }

        /// @brief  Makes the semaphore available a given number of times.
        /// @param  update: the number of available signals to send
        inline void release(count_type update = 1)
        {
            (void)put(update);
        }

        /// @brief  Function to observe the semaphore's current acquirable count.
        /// @return The semaphore's acquirable count
        count_type get_count() const;

        // non-copyable
        semaphore(const semaphore&) = delete;
        semaphore& operator=(const semaphore&) = delete;

    protected:
        semaphore(count_type max, count_type desired, const char* name);

    private:
        bool get(tick_timer::duration timeout);
        bool put(count_type update);
    };

    /// @brief  Counting semaphore class.
    template<semaphore::count_type MAX_VALUE = 1>
    class counting_semaphore : public semaphore
    {
    public:
        /// @brief  Maximum value of the internal counter.
        static constexpr count_type max()
        {
            return MAX_VALUE;
        }

        /// @brief  Constructs a counting semaphore.
        counting_semaphore(count_type desired = 0)
            : semaphore(MAX_VALUE, desired, "counting_semaphore")
        {
        }
    };

    /// @brief  Binary semaphore class.
    class binary_semaphore : public semaphore
    {
    public:
        /// @brief  Maximum value of the internal counter.
        static constexpr count_type max()
        {
            return 1;
        }

        /// @brief  Constructs a binary semaphore.
        binary_semaphore(count_type desired = 0)
            : semaphore(1, desired, "binary_semaphore")
        {
        }
    };
}

#endif // __THREADX_SEMAPHORE_H_
