#pragma once

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace MBase {

class NonCopyable
{
public:
    // Deletion of copy constructor and copy assignment makes the class
    // non-movable, too.
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    // Move construction and move assignment must be enabled explicitly, if desiered.
    //NonCopyable(NonCopyable&&) = default;
    //NonCopyable& operator=(NonCopyable&&) = default;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};
} // namespace MBase

#endif // __NONCOPYABLE_H__
