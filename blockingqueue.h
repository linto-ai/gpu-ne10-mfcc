#ifndef BLOCKKINGQUEUE_H
#define BLOCKKINGQUEUE_H
#include <queue>
#include <condition_variable>
#include <mutex>
#include <iostream>

template <typename T> class BlockingQueue{
    public:
    BlockingQueue<T>(int maxsize)
    {
        this->maxsize = maxsize;
    }
    /**
     * @brief Push a value in the Queue. Notify consumer that a value is available.
     * If the queue is full, it will call pop() first. 
     * 
     * @param value
     */
    void push(const T value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        thequeue.push(value);
        condition.notify_all();
    }


    /**
     * @brief Try to retrieve the first value of the queue. If there is no available values
     * block the consumer until a new value is available.
     * 
     * @return T 
     */
    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [=]{ return ! this->thequeue.empty();});
        T ret = thequeue.front();
        thequeue.pop();
        return ret;
    }
    /**
     * @brief Return the number of values in the queue.
     * 
     * @return int 
     */
    int size()
    {
        return thequeue.size();
    }

    void flush()
    {
        while(!thequeue.empty()){
            delete thequeue.front();
            thequeue.pop();
        }
    }

    private:
    int maxsize;
    int transaction;
    std::queue<T> thequeue;
    std::condition_variable condition;
    std::mutex mutex;

};

#endif