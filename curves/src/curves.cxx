#include "curves.hxx"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <utility>

namespace curves
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
  template<typename T, size_t... I>
  inline constexpr T pow_impl(const T &val, std::index_sequence<I...>)
  {
    return ((I, val) * ...);
  }
#pragma GCC diagnostic pop

  template<size_t Count, typename T>
  inline constexpr T pow(const T &val)
  {
    return pow_impl<T>(val, std::make_index_sequence<Count>{});
  }

  Point point(float t, float radius)
  {
    return Point{
      std::cos(t) * radius,
      std::sin(t) * radius,
      0
    };
  }

  Vector derivative(float t, float radius)
  {
    return Vector{
      -std::sin(t) * radius,
      std::cos(t) * radius,
      0
    };
  }

  Curve::~Curve()
  {
  }

  Circle::Circle(float radius_)
    : radius{radius_}
  {
    assert((radius > 0) && "Circle(...)");
  }

  Circle::~Circle()
  {}

  Point Circle::get_point(float t) const
  {
    return point(t, radius);
  }

  Vector Circle::get_derivative(float t) const
  {
    return derivative(t, radius);
  }

  float Circle::get_radius() const
  {
    return radius;
  }

  Ellipse::Ellipse(float radius_x_, float radius_y_)
    : radius_x{radius_x_}
    , radius_y{radius_y_}
    , min_radius{std::min(radius_x, radius_y)}
    , max_radius{std::max(radius_x, radius_y)}
    , ext{std::sqrt(1 - pow<2>(min_radius) / pow<2>(max_radius))}
  {
    assert((radius_x > 0) &&
        (radius_y > 0) &&
        "Ellipse(...)");
  }

  Ellipse::~Ellipse()
  {}

  Point Ellipse::get_point(float t) const
  {
    return point(t, calc_radius(t));
  }

  Vector Ellipse::get_derivative(float t) const
  {
    return derivative(t, calc_radius(t));
  }

  float Ellipse::calc_radius(float t) const
  {
    t -= radius_x > radius_y ? 0 : M_PI / 2;
    float t_cos = std::cos(t);
    float radius = min_radius / std::sqrt(1 - pow<2>(ext) * pow<2>(t_cos));
    return radius;
  }

  Helix::Helix(float radius_, float step_)
    : radius{radius_}
    , step{step_}
    , z_der{step / static_cast<float>(2 * M_PI)}
  {
    assert((radius > 0) &&
        "Helix(...)");
  }

  Helix::~Helix()
  {}

  Point Helix::get_point(float t) const
  {
    Point res = point(t, radius);
    res.z = t * step / (2 * M_PI);
    return res;
  }

  Vector Helix::get_derivative(float t) const
  {
    Vector res = derivative(t, radius);
    res.z = z_der;
    return res;
  }
} //namespace curves
