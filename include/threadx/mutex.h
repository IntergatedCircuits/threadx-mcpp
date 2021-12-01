/**
 * @file      mutex.h
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
#ifndef __THREADX_MUTEX_H_
#define __THREADX_MUTEX_H_

#include "threadx/tick_timer.h"
#include "threadx/stdlib.h"

namespace threadx
{
    class thread;

    /// @brief  A class implementing std::mutex and std::timed_mutex API.
    class mutex : private native::TX_MUTEX_STRUCT
    {
    public:
        /// @brief  Locks the mutex, blocks until the mutex is lockable.
        inline void lock()
        {
            (void)get(infinity);
        }

        /// @brief  Attempts to lock the mutex.
        /// @return true if the mutex got locked, false if it's already locked
        inline bool try_lock()
        {
            return get(tick_timer::duration(0));
        }

        /// @brief  Unlocks the mutex.
        void unlock();

        /// @brief  Tries to lock the mutex within the given time duration.
        /// @param  rel_time: duration to wait for the mutex to become unlocked
        /// @return true if successful, false if the mutex is locked
        template<class Rep, class Period>
        inline bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time)
        {
            return get(rel_time);
        }

        /// @brief  Tries to lock the mutex until the given deadline.
        /// @param  abs_time: deadline to wait until the mutex becomes unlocked
        /// @return true if successful, false if the mutex is locked
        template<class Clock, class Duration>
        inline bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time)
        {
            return get(abs_time - Clock::now());
        }

        /// @brief  Function to observe the mutex's current locking thread.
        /// @return The mutex's current holder thread, or nullptr if the mutex is unlocked
        thread *get_locking_thread() const;

        /// @brief  Constructs a mutex statically.
        mutex();

        // non-copyable
        mutex(const mutex&) = delete;
        mutex& operator=(const mutex&) = delete;

    private:
        static constexpr const char* DEFAULT_NAME = "mutex";

        bool get(tick_timer::duration timeout);
    };


    /// @brief  A type alias since @ref mutex already implements std::timed_mutex API.
    using timed_mutex = mutex;


    /// @brief  A type alias since @ref mutex is recursive
    using recursive_mutex = mutex;


    /// @brief  A type alias since @ref recursive_mutex already implements std::recursive_timed_mutex API.
    using recursive_timed_mutex = recursive_mutex;
}

#endif // __THREADX_MUTEX_H_
