#ifndef TARGET_H
#define TARGET_H

#include <mutex>
#include <random>
#include <thread>
#include <future>
#include "SDL.h"
#include "snake.h"

enum class TargetType { GOOD, VERYGOOD, BAD, VERYBAD };

class Target
{
public:
  Target(std::size_t grid_width, std::size_t grid_height);
  Target();
  virtual ~Target();

  SDL_Point GetLocation() const {return location;}
  SDL_Color GetColor() const {return color;}
  TargetType GetType() const {return type;}
  bool IsLocatedAt(int x, int y);
  void SetType(TargetType newType) {type = newType;}
  virtual void Place(TargetType&& type_, const Snake* snake) {};

protected:
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  SDL_Point location;  
  TargetType type;
  SDL_Color color;   // has .r, .g, .b, .a fields. All are uint8_t
  std::thread thrd;

  // virtual void PlaceAndColor() {};  //std::promise<void>&& prms, const Snake* snake, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};


class Food : public Target
{
public:
  Food(std::size_t grid_width, std::size_t grid_height, bool placed_=false); 
  void Place(TargetType&& type_, const Snake* snake) override;

private:
  bool placed;
  std::mutex mtx;
  void PlaceAndColor(std::promise<void>&& prms, const Snake* snake, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};


#endif