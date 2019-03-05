//
// Created by Qi Wu on 2019-03-04.
//

#include "Resolve.h"

#include <map>
#include <mutex>

namespace v3d { namespace dx {

using fcn_t = std::function<void()>;

static std::mutex           _reply_lock; //< the lock for access resolve list
static std::map<int, fcn_t> _reply_list; //< the global list for resolves
static int                  _reply_next = 0;

int resolves::add(const std::function<void()> &foo)
{
    _reply_lock.lock();
    auto id = _reply_next++;
    _reply_list[id] = foo;
    _reply_lock.unlock();
    return id;
}

const std::function<void()> resolves::get(int id)
{
    _reply_lock.lock();
    const auto &ret = _reply_list[id];
    _reply_lock.unlock();
    return ret;
}

const std::function<void()> resolves::pop(int id)
{
    _reply_lock.lock();
    auto ret = _reply_list[id];
    _reply_list.erase(id);
    _reply_lock.unlock();
    return std::move(ret);
}

bool resolves::has(int id)
{
    _reply_lock.lock();
    auto ret = (_reply_list.find(id) != _reply_list.end());
    _reply_lock.unlock();
    return std::move(ret);
}

}}
