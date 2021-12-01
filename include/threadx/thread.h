/**
 * @file      thread.h
 * @brief     ThreadX thread API abstraction
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
#ifndef __THREADX_THREAD_H_
#define __THREADX_THREAD_H_

#include "threadx/tick_timer.h"

namespace threadx
{
    namespace native
    {
        // these macros may use native type casts, so need some redirection
        constexpr UINT TOP_PRIORITY = TX_MAX_PRIORITIES;
        constexpr ULONG MIN_STACK_SIZE = TX_TIMER_THREAD_STACK_SIZE;

        constexpr UINT THREAD_EXIT_ID = TX_THREAD_EXIT;
    }

    // https://docs.microsoft.com/en-us/azure/rtos/threadx/chapter3



    /// @brief A class representing a thread of execution.
    class thread : private native::TX_THREAD_STRUCT
    {
    public:
        using function = void (*)(native::ULONG id);

        using id = std::uintptr_t;

        /// @brief  Signals to the thread's observer that it's being terminated,
        ///         and destroys the thread, stopping its execution and freeing
        ///         its dynamically allocated memory.
        ~thread();

        /// @brief  Suspends the execution of the thread, until @ref resume is called.
        void suspend();

        /// @brief  Resumes the execution of the suspended thread.
        void resume();

        /// @brief  Provides a unique identifier of the thread.
        /// @return The thread's unique identifier (0 is reserved as invalid)
        id get_id() const;

        /// @brief  Reads the thread's friendly name (the string's size is limited to @ref NAME_MAX_SIZE).
        /// @return Pointer to the thread's name
        const char *get_name();

        /// @brief  Possible operating states of a thread.
        enum class state
        {
            running = 0,
            ready,
            completed,
            terminated,
            suspended,
        };

        /// @brief  Reads the current state of the thread.
        /// @return The current state of the thread
        state get_state() const;

        /// @brief  Returns the currently executing thread.
        /// @return Pointer to the currently executing thread
        static thread* get_current();

        /// @brief  Thin type wrapper for thread priority.
        class priority
        {
        public:
            using value_type = native::UINT;

            /// @brief  Default priority is 1, to preempt the built-in IDLE thread of the RTOS.
            constexpr priority()
                    : value_(1)
            { }
            constexpr priority(value_type value)
                    : value_(value)
            { }
            operator value_type&()
            {
                return value_;
            }
            constexpr operator value_type() const
            {
                return value_;
            }

            static constexpr priority max()
            {
                return native::TOP_PRIORITY;
            }
            static constexpr priority min()
            {
                return 0;
            }

        private:
            value_type value_;
        };

        /// @brief  Returns the thread's current priority level.
        /// @return The priority of the thread
        /// @remark Thread and ISR context callable
        priority get_priority() const;

        /// @return Pointer to the currently executing thread
        /// @remark Thread context callable
        void set_priority(priority prio);

        #ifndef TX_DISABLE_NOTIFY_CALLBACKS

        private:
            using entry_exit_callback = void (*)(thread*, native::UINT id);

            void set_entry_exit_callback(entry_exit_callback func, void* param);
            entry_exit_callback get_entry_exit_callback() const;
            void* get_entry_exit_param() const;
            static void join_exit_callback(thread *t, native::UINT id);

        public:
            /// @brief  Waits for the thread to finish execution.
            /// @note   May only be called when the thread is joinable, and not from the owned thread's context
            void join();

            /// @brief  Checks if the thread is joinable (potentially executing).
            /// @return true if the thread is valid and hasn't been joined, false otherwise
            /// @remark Thread and ISR context callable
            bool joinable() const;

        #endif // !TX_DISABLE_NOTIFY_CALLBACKS

    protected:
        static constexpr const char* DEFAULT_NAME = "anonym";
        static constexpr size_t DEFAULT_STACK_SIZE = native::MIN_STACK_SIZE;

        thread(void *pstack, std::uint32_t stack_size,
                function func, native::ULONG param,
                priority prio, const char *name);

    private:
        // non-copyable
        thread(const thread&) = delete;
        thread& operator=(const thread&) = delete;
        // non-movable
        thread(const thread&&) = delete;
        thread& operator=(const thread&&) = delete;
    };


    /// @brief  A thread with statically allocated stack.
    template <const std::size_t STACK_SIZE_BYTES>
    class static_thread : public thread
    {
    public:
        static constexpr std::size_t STACK_SIZE = STACK_SIZE_BYTES;

        /// @brief  Constructs a static thread. The thread becomes ready to execute
        ///         within this call, meaning that it might have started running
        ///         by the time this call returns.
        /// @param  func:      the function to execute in the thread context
        /// @param  param:     opaque parameter to pass to the thread function
        /// @param  prio:      thread priority level
        /// @param  name:      short label for identifying the thread
        static_thread(function func, void *param,
                priority prio = priority(), const char *name = DEFAULT_NAME)
            : thread(stack_, sizeof(stack_) / sizeof(stack_[0]),
                    func, param, prio, name)
        {
        }

        template<typename T>
        static_thread(typename std::enable_if<(sizeof(T) <= sizeof(std::uintptr_t)),
                void (*)(T)>::type func, T arg,
                priority prio = priority(), const char *name = DEFAULT_NAME)
            : static_thread(reinterpret_cast<function>(func),
                    reinterpret_cast<void*>(static_cast<std::uintptr_t>(arg)),
                    prio, name)
        {
        }

        template<typename T>
        static_thread(void (*func)(T*), T* arg,
                priority prio = priority(), const char *name = DEFAULT_NAME)
            : static_thread(reinterpret_cast<function>(func),
                    reinterpret_cast<void*>(arg),
                    prio, name)
        {
        }

        template<typename T>
        static_thread(void (*func)(T*), T& arg,
                priority prio = priority(), const char *name = DEFAULT_NAME)
            : static_thread(reinterpret_cast<function>(func),
                    reinterpret_cast<void*>(arg),
                    prio, name)
        {
        }

        template<class T>
        static_thread(T& obj, void (T::*member_func)(),
                priority prio = priority(), const char *name = DEFAULT_NAME)
            : static_thread(reinterpret_cast<function>(member_func),
                    reinterpret_cast<void*>(&obj),
                    prio, name)
        {
        }

        /// @brief  Static threads aren't allocated by the OS, so new and delete
        ///         may use the default operators.
        void operator delete(void *p)
        {
            ::delete p;
        }

    private:
        unsigned char stack_[STACK_SIZE_BYTES];
    };


    /// @brief  Namespace offering control on the current thread of execution.
    namespace this_thread
    {
        /// @brief  Yields execution of the current thread so the OS can schedule
        ///         other thread(s) for the remainder of the time slice.
        void yield();

        /// @brief  Provides a unique identifier of the current thread.
        /// @return The current thread's unique identifier
        thread::id get_id();

        void sleep_for(tick_timer::duration rel_time);

        /// @brief  Blocks the current thread's execution for a given duration.
        /// @param  rel_time: duration to block the current thread
        template<class Rep, class Period>
        void sleep_for(const std::chrono::duration<Rep, Period>& rel_time)
        {
            // workaround to prevent this function calling itself
            const auto ticks_sleep_for = static_cast<void (*)(tick_timer::duration)>(&sleep_for);
            ticks_sleep_for(std::chrono::duration_cast<tick_timer::duration>(rel_time));
        }

        /// @brief  Blocks the current thread's execution until the given deadline.
        /// @param  abs_time: deadline to block the current thread
        template<class Clock, class Duration>
        void sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time)
        {
            sleep_for(abs_time - Clock::now());
        }

        #if 0 && (configUSE_TASK_NOTIFICATIONS == 1)

            bool notify_wait_for(const tick_timer::duration& rel_time,
                    thread::notify_flag clr_at_entry = 0, thread::notify_flag clr_at_exit = 0,
                    thread::notify_flag *received = nullptr);

            notify_value notify_value_wait_for(const tick_timer::duration& rel_time,
                    thread::notify_flag clr_at_entry = 0, thread::notify_flag clr_at_exit = 0,
                    thread::notify_flag *received = nullptr);

        #endif // (configUSE_TASK_NOTIFICATIONS == 1)
    }
}

#endif // __THREADX_THREAD_H_
