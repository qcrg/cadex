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

float sum_second_container_radii(const Container &circles)
{
  auto accum = [](float sum, const CurvePtr &circle) {
    return sum + reinterpret_cast<Circle*>(circle.get())->get_radius();
  };

  if (circles.size() == 0)
    return 0;
  std::vector<std::future<float>> futures;
  int t_count = std::min(static_cast<unsigned int>(circles.size()),
      std::thread::hardware_concurrency());
  t_count = t_count == 0 ? 8 : t_count;
  futures.reserve(t_count);

  int count_per_thread = circles.size() / t_count;
  auto first = circles.begin();
  for (int i = 1; i < t_count; i++)
  {
    auto process_sum = [](auto first, auto last) {
      float tmp_sum = 0;
      for (; first != last; ++first)
        tmp_sum += reinterpret_cast<Circle*>(first->get())->get_radius();
      return tmp_sum;
    };
    auto last = std::next(first, count_per_thread);
    futures.push_back(std::async(process_sum, first, last));
    first = last;
  }
  float last_sum = std::accumulate(first, circles.end(), 0.0f, accum);
  std::clog << circles.end() - first << std::endl;
  // sum = std::accumulate(first, circles.end(), 0.0f, accum);
  float sum = 0;
  sum += last_sum;
  for (auto &fut : futures)
    sum += fut.get();

  // sum += last_sum;
  std::clog << circles.end() - first << std::endl;

  return sum;
}

//FIXME
float sum_single_manual(const Container &circles)
{
  int t_count = std::thread::hardware_concurrency();
  int count_per_thread = circles.size() / t_count;
  float sum = 0;
  for (int i = 0; i < (int)circles.size(); i++)
  {
    if (!(i % count_per_thread) && i != 0)
    {
      std::cout << std::format("Manual: [{}] {}\n",
        i / count_per_thread,
        sum
      );
    }
    sum += reinterpret_cast<Circle*>(circles[i].get())->get_radius();
  }
  // for (const auto &circle : circles)
  //   sum += reinterpret_cast<Circle*>(circle.get())->get_radius();
  return sum;
}

//FIXME
float sum_single_thread(const Container &circles)
{
  auto accum = [](float sum, const CurvePtr &circle) {
    return sum + reinterpret_cast<Circle*>(circle.get())->get_radius();
  };
  return std::accumulate(circles.begin(), circles.end(), 0.0f, accum);
}

//FIXME
float sum_simulate_multithread(const Container &circles)
{
  const auto accum = [](const float &a, const CurvePtr &circle) {
    return a + reinterpret_cast<Circle*>(circle.get())->get_radius();
  };
  int t_count = std::thread::hardware_concurrency();
  const int count_per_thread = circles.size() / t_count;
  auto first = circles.begin();
  float sum = 0;
  for (int i = 1; i <= t_count; i++)
  {
    auto last = std::next(first, count_per_thread);
    sum += std::accumulate(first, last, 0.0f, accum);
    first = last;
    std::clog << std::format("Sim multithread: [{}] {}\n", i, sum);
  }
  sum += std::accumulate(first, circles.end(), 0.0f, accum);
  return sum;
}

int main()
{
  Container cont = populate_container(124126);
  // print_point_and_derivative(cont);
  Container circles = populate_circle_container(cont);
  sort_circles(circles);
  std::cout << std::format(
      "Multithread sum:           {:10.0f}\n"
      "Simulated multithread sum: {:10.0f}\n"
      "Single thread sum:         {:10.0f}\n"
      "Single manual sum:         {:10.0f}\n",
      sum_second_container_radii(circles),
      sum_simulate_multithread(circles),
      sum_single_thread(circles),
      sum_single_manual(circles));
  return 0;
}
