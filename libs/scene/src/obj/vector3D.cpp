// vector3D.cpp
#include "obj/vector3D.h"

Vector3D& Vector3D::operator=(const util::arr3& arr) {
  x = arr.x; y = arr.y; z = arr.z;
  return *this;
}

Vector3D& Vector3D::operator=(util::arr3&& arr) noexcept {
  x = arr.x; y = arr.y; z = arr.z;
  return *this;
}
