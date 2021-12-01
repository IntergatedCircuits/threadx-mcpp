/**
 * @file      thread.cpp
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
#include "threadx/thread.h"
#include "threadx/semaphore.h"

using namespace threadx;
using namespace threadx::native;

thread::~thread()
{
    if (tx_thread_state != TX_COMPLETED)
    {
        auto result = tx_thread_terminate(this);
        assert(result == TX_SUCCESS);
    }
    auto result = tx_thread_delete(this);
    assert(result == TX_SUCCESS);
}

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    void thread::set_entry_exit_callback(entry_exit_callback func, void* param)
    {
        if (TX_SUCCESS == tx_thread_entry_exit_notify(this, reinterpret_cast<void(*)(TX_THREAD *, unsigned)>(func)))
        {
            // TODO: make sure that tx_user.h contains this:
            // #define TX_THREAD_USER_EXTENSION     void* entry_exit_param_;
            entry_exit_param_ = param;
        }
    }

// fun name collision between macro used right above, and member variable used below
#undef tx_thread_entry_exit_notify

    thread::entry_exit_callback thread::get_entry_exit_callback() const
    {
        return reinterpret_cast<entry_exit_callback>(this->tx_thread_entry_exit_notify);
    }

    void* thread::get_entry_exit_param() const
    {
        return this->entry_exit_param_;
    }

    bool thread::joinable() const
    {
        auto state = get_state();
        return (state != state::completed) && (state != state::terminated) && (nullptr == get_entry_exit_param());
    }

    void thread::join_exit_callback(thread *t, UINT id)
    {
        if (id == TX_THREAD_EXIT)
        {
            auto *exit_cond = reinterpret_cast<semaphore*>(t->get_entry_exit_param());

            exit_cond->release();
        }
    }

    void thread::join()
    {
        assert(joinable()); // else invalid_argument
        assert(this->get_id() != this_thread::get_id()); // else resource_deadlock_would_occur

        binary_semaphore exit_cond;
        set_entry_exit_callback(&thread::join_exit_callback, reinterpret_cast<void*>(&exit_cond));

        // wait for signal from thread exit
        exit_cond.acquire();

        // signal received, thread is deleted, return
    }

#endif // !TX_DISABLE_NOTIFY_CALLBACKS

void thread::suspend()
{
    tx_thread_suspend(this);
}

void thread::resume()
{
    tx_thread_resume(this);
}

thread::priority thread::get_priority() const
{
    return tx_thread_user_priority;
}

void thread::set_priority(priority prio)
{
    priority::value_type old_prio;
    tx_thread_priority_change(this, prio, &old_prio);
}

thread::id thread::get_id() const
{
    return id(this);
}

const char *thread::get_name()
{
    return const_cast<const char*>(tx_thread_name);
}

thread::state thread::get_state() const
{
    state s;
    switch (tx_thread_state)
    {
        case TX_READY:
            s = (get_current() == this) ? state::running : state::ready;
            break;
        case TX_COMPLETED:
            s = state::completed;
            break;
        case TX_TERMINATED:
            s = state::terminated;
            break;
        default:
            s = state::suspended;
            break;
    }
    return s;
}

thread* thread::get_current()
{
    return reinterpret_cast<thread*>(tx_thread_identify());
}

thread::thread(void *pstack ,std::uint32_t stack_size,
        function func, native::ULONG param, priority prio, const char *name)
{
    auto result = tx_thread_create(
            this,                       // TX_THREAD *thread_ptr
            const_cast<char*>(name),    // CHAR *name_ptr
            func,                       // VOID (*entry_function)(ULONG id)
            param,                      // ULONG entry_input
            pstack,                     // VOID *stack_start
            stack_size,                 // ULONG stack_size
            prio,                       // UINT priority
            prio,                       // UINT preempt_threshold
            TX_NO_TIME_SLICE,           // ULONG time_slice
            TX_AUTO_START);             // UINT auto_start
    assert(result == TX_SUCCESS);
}

void this_thread::yield()
{
    tx_thread_relinquish();
}

thread::id this_thread::get_id()
{
    return thread::get_current()->get_id();
}

void this_thread::sleep_for(tick_timer::duration rel_time)
{
    auto result = tx_thread_sleep(to_ticks(rel_time));
    assert(result == TX_SUCCESS);
}
