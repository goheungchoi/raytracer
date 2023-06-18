#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "util/util.h"

#include "ray.h"

struct I {
  double r{1.0};
  double g{1.0};
  double b{1.0};

  // Default constructor
  constexpr I() = default;
  // Initializer list constructor
  constexpr I(std::initializer_list<double> init) {
    if(init.size() == 3) {
      auto it = init.begin();
      r = *it;
      g = *(it+1);
      b = *(it+2);
    }
  }
  // Parameterized constructors
  constexpr I(util::arr3 arr) : r(arr.x), g(arr.y), b(arr.z) {}
  // Copy assignment operators
  I& operator=(const I& other) = default;
  I& operator=(const util::arr3& arr) {
    r = arr.x; g = arr.y; b = arr.z;
    return *this;
  }

  inline void scale() {
    r = r > 1.0 ? 1.0 : r < 0.0 ? 0.0 : r;
    g = g > 1.0 ? 1.0 : g < 0.0 ? 0.0 : g;
    b = b > 1.0 ? 1.0 : b < 0.0 ? 0.0 : b;
  }

  void print() const {
    std::cout << "I: (" << r << ", " << g << ", " << b << ")" << std::endl;
  }

  // Overloaded operant
  constexpr I& operator+=(const I& rhs) {                           
    r += rhs.r; g += rhs.g; b += rhs.b;
    return *this; // return the result by reference
  }
  constexpr I& operator-=(const I& rhs) {
    r -= rhs.r; g -= rhs.g; b -= rhs.b;
    return *this;
  }
  constexpr I& operator*=(const I& rhs) {
    r *= rhs.r; g *= rhs.g; b *= rhs.b;
    return *this;
  }
  constexpr I& operator*=(const double n) {
    r *= n; g *= n; b *= n;
    return *this;
  }
  constexpr I& operator/=(const I& rhs) {
    if (rhs.r == 0 || rhs.g == 0 || rhs.b == 0)
      throw std::invalid_argument("Division by zero");
    r /= rhs.r; g /= rhs.g; b /= rhs.b;
    return *this;
  }
  constexpr I& operator/=(const double n) {
    if (n == 0)
      throw std::invalid_argument("Division by zero");
    r /= n; g /= n; b /= n;
    return *this;
  }
};

// Overloaded operators
constexpr I operator+(const I& l, const I& r){
  I temp{l.r+r.r, l.g+r.g, l.b+r.b};
  return temp;
}
constexpr I operator-(const I& l, const I& r){
  I temp{l.r-r.r, l.g-r.g, l.b-r.b};
  return temp;
}
constexpr I operator*(const I& l, const I& r){
  I temp{l.r*r.r, l.g*r.g, l.b*r.b};
  return temp;
}
constexpr I operator*(const double n, const I& p){
  I temp{n*p.r, n*p.g, n*p.b};
  return temp;
}
constexpr I operator*(const I& l, const double n){
    return n*l;
}
constexpr I operator/(const I& l, const I &r){
  if (r.r == 0 || r.g == 0 || r.b == 0)
    throw std::invalid_argument("Division by zero");
  I temp{l.r/r.r, l.g/r.g, l.b/r.b};
  return temp;
}
constexpr I operator/(const double n, const I &p){
  if (p.r == 0 || p.g == 0 || p.b == 0)
    throw std::invalid_argument("Division by zero");
  I temp{n/p.r, n/p.g, n/p.b};
  return temp;
}
constexpr I operator/(const I &p, const double n) {
  if (n == 0)
    throw std::invalid_argument("Division by zero");
  I temp{p.r/n, p.g/n, p.b/n};
  return temp;
}
constexpr bool operator==(const I& l, const I& r){
  bool x = util::almost_equal(l.r, r.r, 100);
  bool y = util::almost_equal(l.g, r.g, 100);
  bool z = util::almost_equal(l.b, r.b, 100);
  return x && y && z; 
}
constexpr bool operator!=(const I& l, const I& r){
    return !(l == r);
}

enum class LightType {
  DIRECTIONAL,
  POINT,
  HIGHLIGHT,
  UNDEFINED
};

class Light {
public:
  // Member variables
  LightType type = LightType::UNDEFINED;
  I intensity{0.0, 0.0, 0.0};
  // Default constructor
  constexpr Light() = default;
  // Parameterized constructors
  constexpr Light(const I& intensity) : intensity(intensity) {};
  constexpr Light(I&& intensity) noexcept : intensity(std::move(intensity)) {};
  constexpr Light(std::initializer_list<double> intensity_) : intensity(intensity_) {};
  // Default destructor
  virtual ~Light(){};
  // Member functions
  virtual double attenuationFunc(const Point &p) const;
};

class DirectionalLight : virtual public Light {
public:
  Vector3D dir;
  DirectionalLight() : Light(), dir(Vector3D()) 
  { type = LightType::DIRECTIONAL; };
  DirectionalLight(I intensity, Vector3D dir) : Light(intensity), dir(dir.unit()) 
  { type = LightType::DIRECTIONAL; };
};

class PointLight : virtual public Light {
public:
  // Member variables
  Point o;
  double radius = 0.0;
  double c1 = 0.0, c2 = 0.0, c3 = 0.0;
  // Default constructor
  PointLight() : Light() { type = LightType::POINT; };
  // Parameterized constructors
  PointLight(I intensity, Point o, double radius) 
  : Light(intensity), radius(radius), o(o) 
  { type = LightType::POINT; };
  // Override functions
  double attenuationFunc(const Point &p) const;
  // Inline member functions
  inline void setAttenuation(double c1, double c2, double c3) {
    // if c1, c2, c3 = 0, don't assign
    if (util::almost_equal(c1, 0.0, 100) 
      && util::almost_equal(c2, 0.0, 100) 
      && util::almost_equal(c3, 0.0, 100))      
      return;
    
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;
  };
  /**
   * @brief Returns the degree that indicates how far the point is 
   * from the point light source. 
   * The further the point, the smaller the degree.
   * 
   * @param p 
   * @return constexpr double 
   */
  inline constexpr double getTheta(const Point& p) const {
    double dist = o.getDistance(p);
    return atan(radius / dist);
  }
};

class Highlight : virtual public Light {
public:
  // Member variables
  Point o;
  double radius;
  Vector3D dir;   // direction of highlight
  double cutoff;  // the degree between dir and edge of highlight
  // Default constructor
  Highlight() : Light(), radius(0.0) 
  { type = LightType::HIGHLIGHT; };
  // Parameterized constructors
  Highlight(I intensity, Point o, double radius, Vector3D dir, double cutoff);
  // Override functions
  double attenuationFunc(const Point &p) const;
  // Inline member functions
  inline constexpr bool isInHighlight(const Point &p) const {
    Vector3D v_o(o, p);
    double rad = acos(dir.dot(v_o.unit()));
    double degree = rad*180.0/util::PI;
    return degree <= cutoff;
  };
  inline constexpr double getTheta(const Point& p) const {
    double dist = o.getDistance(p);
    return atan(radius / dist);
  };
};

#endif  // _LIGHT_H_
