
//
//  noncopyable.h
//  game_server
//
//  Created by derek on 2023/4/10.
//

#ifndef __SYLAR_NONCOPYABLE_H__
#define __SYLAR_NONCOPYABLE_H__

namespace sylar{


class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;

    Noncopyable& operator=(const Noncopyable&) = delete;
};

}

#endif /* __SYLAR_NONCOPYABLE_H__ */




