#include <iostream>
#include <string>
#include <stdexcept>
#include "target.h"

Target::~Target() 
{
  // thrd.join();  
}

Target::Target() 
    : engine(dev()) {}

Target::Target(std::size_t grid_width, std::size_t grid_height) 
    : engine(dev()),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height))  {}

bool Target::IsLocatedAt(int x, int y)
{
  return location.x == x && location.y == y;
}


Food::Food(std::size_t grid_width, std::size_t grid_height, bool placed_)
    : placed(placed_),
    Target(grid_width, grid_height)  {}


void Food::Place(TargetType&& type_, const Snake* snake)
{
  uint8_t r, g, b, a;
  
  std::unique_lock<std::mutex> lck(mtx);
  placed = false;
  type = std::move(type_);
  lck.unlock();

  switch (type)
  {
    case TargetType::VERYGOOD:
      {
        // color = brown;
        r = 0xA0;
        g = 0x52;
        b = 0x2D;
        a = 0xFF;
        break;
      }
    case TargetType::GOOD:
      {
        // color = red;
        r = 0xFF;
        g = 0x00;
        b = 0x00;
        a = 0xFF;
        break;
      }
    case TargetType::BAD:
      {
        // color = yellow;
        r = 0xFF;
        g = 0xCC;
        b = 0x00;
        a = 0xFF;
        break;
      }
    case TargetType::VERYBAD:
      {
        // color = green;
        r = 0x00; 
        g = 0xFF;
        b = 0x00;
        a = 0xFF;
        // // color = orange;
        // color.r = 0xFF; 
        // color.g = 0xCC;
        // color.b = 0x00;
        // color.a = 0xFF;
        break;
      }
    default:
      throw std::runtime_error("invalud input to Food::Place");
  }

  std::promise<void> prms;
  std::future ftr = prms.get_future();
  thrd = std::thread(&Food::PlaceAndColor, this, std::move(prms), snake, r, g, b, a);
  ftr.get();
  thrd.join();
}

void Food::PlaceAndColor(std::promise<void>&& prms, const Snake* snake, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  int x, y;
  bool write = false;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (snake==nullptr) {
      write = true;
    }
    else {
      if (!snake->SnakeCell(x, y)) {
        write = true;
      }
    }
    if (write) {
      std::lock_guard<std::mutex> lck(mtx);  // RAII
      location.x = x;
      location.y = y;
      placed = true;
      color.r = r;
      color.g = g;
      color.b = b;
      color.a = a;
      prms.set_value();
      return; // releases the lock
    }
  }
}