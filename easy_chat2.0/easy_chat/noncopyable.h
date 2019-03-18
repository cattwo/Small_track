//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_NONCOPYABLE_H
#define EASY_CHAT_NONCOPYABLE_H
/*禁止拷贝和赋值*/
class  noncopyable
{
public:
    noncopyable(const noncopyable) = delete;
    void operator=(const noncopyable) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
#endif //EASY_CHAT_NONCOPYABLE_H
