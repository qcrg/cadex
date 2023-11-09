#include "curves.hxx"

#include <numeric>
#include <functional>
#include <thread>
#include <future>
#include <iostream>
#include <vector>
#include <memory>
#include <format>
#include <string>
#include <algorithm>
#include <ranges>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <execution>

using namespace curves;

using CurvePtr = std::shared_ptr<Curve>;
using Container = std::vector<CurvePtr>;

template<>
struct std::formatter<Point> : std::formatter<std::string>
{
  auto format(const Point &p, format_context &ctx) const
  {
    return formatter<string>::format(
        std::format("({}, {}, {})", p.x, p.y, p.z), ctx);
  }
};

template<>
struct std::formatter<Vector> : std::formatter<std::string>
{
  auto format(const Vector &v, std::format_context &ctx) const
  {
    return std::formatter<std::string>::format(
        std::format("{{{}, {}, {}}}", v.x, v.y, v.z), ctx);
  }
};

float myrand()
{
  return std::rand() % 60000;
}

float rand_radius()
{
  return myrand() + 1;
}

float rand_step()
{
  float sign = std::rand() % 2 ? -1 : 1;
  return myrand() * sign;
}

Container populate_container(size_t count)
{
  Container cont(count);
  std::ranges::generate(cont,
      []() -> CurvePtr {
        switch(std::rand() % 3)
        {
          case 0:
            return std::make_shared<Circle>(rand_radius());
          break;

          case 1:
            return std::make_shared<Ellipse>(rand_radius(), rand_radius());
          break;

          case 2:
            return std::make_shared<Helix>(rand_radius(), rand_step());
        }
        assert(false && "Populate container");
        return {};
      });
  return cont;
}

void print_point_and_derivative(const Container &cont)
{
  for (const auto &curve : cont)
    std::cout << std::format("{}, {}\n",
        curve->get_point(M_PI / 4),
        curve->get_derivative(M_PI / 4));
}

Container populate_circle_container(const Container &all_curves)
{
  Container cont;
  cont.reserve(all_curves.size() / 3 + 10);
  for (const auto &curve : all_curves)
    if (dynamic_cast<Circle*>(curve.get()))
      cont.push_back(curve);
  return cont;
}

void sort_circles(Container &circles)
{
  std::ranges::sort(circles,
      [](const CurvePtr &l_curve,
          const CurvePtr &r_curve) {
        auto l = reinterpret_cast<Circle*>(l_curve.get());
        auto r = reinterpret_cast<Circle*>(r_curve.get());
        return l->get_radius() < r->get_radius();
      });
}

float sum_circles_radii(const Container &circles)
{
  auto accum = [](float sum, const CurvePtr &circle) {
    return sum + reinterpret_cast<Circle*>(circle.get())->get_radius();
  };
  return std::accumulate(circles.begin(), circles.end(), 0.0f, accum);
}

int main()
{
  Container cont = populate_container(124126);
  print_point_and_derivative(cont);
  Container circles = populate_circle_container(cont);
  sort_circles(circles);
  float sum = sum_circles_radii(circles);
  (void)sum;
  return 0;
}
