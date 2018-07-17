#include "blockingqueue.h"

template <typename T>
BlockingQueue<T>::BlockingQueue(uint maxsize){
    this->maxsize = maxsize;
}

template <typename T>
void BlockingQueue<T>::push(T const value)
{
    /**if (thequeue.size() == maxsize)
    {

    } else { **/
        thequeue.push_back(value);
    //}
}

template <typename T>
T BlockingQueue<T>::pop()
{
    /**if (thequeue.size() == 0)
    {

    } else {**/
        return thequeue.pop_front();
    //}
}

template <typename T>
uint BlockingQueue<T>::size()
{
    return thequeue.size();
}