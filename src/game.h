#ifndef GAME_H
#define GAME_H

// #include <random>
#include <future>
#include <thread>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "target.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  ~Game();
  void Run(Controller &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  std::shared_ptr<Food> food;
  std::future<void> ftr;  // Resides until ~Game
  std::thread thrd_coming_events;
  std::thread thrd_process_events;
  int score{0};

  void Update();
};

#endif