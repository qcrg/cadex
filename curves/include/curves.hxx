#pragma once

#include "basic_types.hxx"

namespace curves
{

  struct Curve
  {
    virtual ~Curve();
    virtual Point get_point(float t) const = 0;
    virtual Vector get_derivative(float t) const = 0;
  };

  class Circle : public Curve
  {
  public:
    Circle(float radius);
    virtual ~Circle();
    virtual Point get_point(float t) const override;
    virtual Vector get_derivative(float t) const override;
    float get_radius() const;
  private:
    float radius;
  };

  class Ellipse : public Curve
  {
  public:
    Ellipse(float radius_x, float radius_y);
    virtual ~Ellipse();
    virtual Point get_point(float t) const override;
    virtual Vector get_derivative(float t) const override;
  private:
    float radius_x;
    float radius_y;
    float min_radius;
    float max_radius;
    float ext;

    float calc_radius(float t) const;
  };

  class Helix : public Curve
  {
  public:
    Helix(float radius, float step);
    virtual ~Helix();
    virtual Point get_point(float t) const override;
    virtual Vector get_derivative(float t) const override;
  private:
    float radius;
    float step;
    const float z_der;
  };

} //namespace curves
