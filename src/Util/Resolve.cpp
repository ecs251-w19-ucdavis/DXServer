//
// Created by Qi Wu on 2019-03-04.
//

#include "Resolve.h"

#include <vector>
#include <mutex>

namespace v3d { namespace dx {

using fcn_t = std::function<void()>;

static std::mutex         _reply_lock;
static std::vector<fcn_t> _reply_list;

int resolves::add(const std::function<void()> &foo)
{
    _reply_lock.lock();
    _reply_list.emplace_back(foo);
    auto ret = int(_reply_list.size() - 1);
    _reply_lock.unlock();
    return ret;
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
    _reply_list.erase(_reply_list.begin() + id);
    _reply_lock.unlock();
    return std::move(ret);
}

}}
