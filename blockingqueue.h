#ifndef BLOCKKINGQUEUE_H
#define BLOCKKINGQUEUE_H
#include <queue>
#include <condition_variable>
#include <mutex>

template <typename T>
class BlockingQueue{
    public:
    BlockingQueue(uint maxsize);
    void push(const T value);
    T pop();
    uint size();

    private:
    uint maxsize;
    std::queue<T> thequeue;
    std::condition_variable q_condition;

};

#endif