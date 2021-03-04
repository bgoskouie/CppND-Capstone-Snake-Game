#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <thread>
#include "snake.h"

template <class T>
class MessageQueue
{
public:
    T receive();

    void send(T &&msg);
    size_t GetSize();

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};

class Controller {
 public:
  void HandleEvents();
  void ProcessEventQueue(bool *running, Snake *snake); // Thread needs pointers. Reference doesn't work!

 private:
  void ChangeDirection(Snake* snake, Snake::Direction input,
                       Snake::Direction opposite) const;
  MessageQueue<SDL_Event> _msgQueue;   // capture all events and save them in this Queue for future processing (in the order they were received - i.e. fifo)
  static std::mutex mtx;
};

#endif