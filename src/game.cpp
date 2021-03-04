#include "game.h"
#include <iostream>
#include "SDL.h"
#include <future>
#include <thread>
#include <mutex>

Game::~Game() {
  thrd_coming_events.join();
  thrd_process_events.join();
}

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      food(std::make_shared<Food>(grid_width, grid_height)) {
      
  ftr = std::async(std::launch::async, &Food::Place, food, std::move(TargetType::GOOD), nullptr);
  
  // food.Place(TargetType::GOOD, nullptr);
}

void Game::Run(Controller &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  thrd_coming_events = std::thread(&Controller::HandleEvents, &controller);
  thrd_process_events = std::thread(&Controller::ProcessEventQueue, &controller, &running, &snake);

  while (running) {
    frame_start = SDL_GetTicks();
    // controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food->IsLocatedAt(new_x, new_y)) {
    score++;
    food->Place(TargetType::GOOD, &snake);
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }