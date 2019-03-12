//
// Created by Qi Wu on 2019-03-04.
//

#include "Resolve.h"

#include <map>
#include <mutex>

namespace v3d { namespace dx {

using fcn_t  = std::function<void()>;
using lock_t = std::unique_lock<std::mutex>;

static std::mutex           _reply_lock; //< the lock for access resolve list
static std::map<int, fcn_t> _reply_list; //< the global list for resolves
static int                  _reply_next = 0;

int resolves::add(const std::function<void()> &foo)
{
    lock_t lock(_reply_lock);
    auto id = _reply_next++;
    _reply_list[id] = foo;
    return id;
}

const std::function<void()> resolves::pop(int id)
{
    lock_t lock(_reply_lock);
    auto ret = _reply_list[id];
    _reply_list.erase(id);
    return std::move(ret);
}

}}
