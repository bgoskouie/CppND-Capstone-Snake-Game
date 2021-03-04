#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

std::mutex Controller::mtx;


void Controller::ChangeDirection(Snake* snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake->direction != opposite || snake->size == 1) snake->direction = input;
  return;
}

void Controller::ProcessEventQueue(bool *running, Snake* snake)
{
  SDL_Event e;
  size_t queue_size = 0;

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    queue_size = _msgQueue.GetSize();

    if (queue_size > 0) {
      std::unique_lock<std::mutex> ulock(mtx);
      std::cout << "A Message seems available!, queue_size = " << queue_size << std::endl;
      ulock.unlock();

      e = _msgQueue.receive();
      // std::unique_lock<std::mutex> ulock(mtx);
      ulock.lock();
      std::cout << "received: queue_size = " << queue_size << ", event type = " << e.type << std::endl;

      if (e.type == SDL_QUIT) {
        *running = false;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_UP:
            ChangeDirection(snake, Snake::Direction::kUp,
                            Snake::Direction::kDown);
            break;

          case SDLK_DOWN:
            ChangeDirection(snake, Snake::Direction::kDown,
                            Snake::Direction::kUp);
            break;

          case SDLK_LEFT:
            ChangeDirection(snake, Snake::Direction::kLeft,
                            Snake::Direction::kRight);
            break;

          case SDLK_RIGHT:
            ChangeDirection(snake, Snake::Direction::kRight,
                            Snake::Direction::kLeft);
            break;
        }
      }
    }
  }
}

void Controller::HandleEvents() {   //bool &running, Snake &snake) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {  // we don't care about other ones, especially mouse motion
      std::unique_lock<std::mutex> uLock(mtx);
      std::cout << "HandleEvents: A msg came - BEFORE sending! _msgQueue size = " << _msgQueue.GetSize() << std::endl;
      uLock.unlock();
      _msgQueue.send(std::move(e));
      uLock.lock(); // RAII
      std::cout << "HandleEvents: A msg came - AFTER sending! _msgQueue size = " << _msgQueue.GetSize() << std::endl;
    }
  }
}

// void Controller::HandleInput(bool &running, Snake &snake) const {
//   SDL_Event e;
//   while (SDL_PollEvent(&e)) {
    // if (e.type == SDL_QUIT) {
    //   running = false;
    // } else if (e.type == SDL_KEYDOWN) {
    //   switch (e.key.keysym.sym) {
    //     case SDLK_UP:
    //       ChangeDirection(snake, Snake::Direction::kUp,
    //                       Snake::Direction::kDown);
    //       break;

    //     case SDLK_DOWN:
    //       ChangeDirection(snake, Snake::Direction::kDown,
    //                       Snake::Direction::kUp);
    //       break;

    //     case SDLK_LEFT:
    //       ChangeDirection(snake, Snake::Direction::kLeft,
    //                       Snake::Direction::kRight);
    //       break;

    //     case SDLK_RIGHT:
    //       ChangeDirection(snake, Snake::Direction::kRight,
    //                       Snake::Direction::kLeft);
    //       break;
    //   }
    // }
//   }
// }


/* Implementation of class "MessageQueue" */

template <typename T>
size_t MessageQueue<T>::GetSize()
{
  std::unique_lock<std::mutex> uLock(_mutex);
  return _queue.size();
}

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // simulate some work
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);

    // add vector to queue
    _queue.push_back(std::move(msg));
    _cond.notify_one(); // notify client after pushing new Vehicle into vector
}


