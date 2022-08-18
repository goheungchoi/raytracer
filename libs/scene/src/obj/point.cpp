#include "point.h"

Point::Point() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Point::Point(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::Point(const Point &p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

Point Point::operator+(const Point &p) {
    return Point(this->x + p.x, this->y + p.y, this->z + p.z);
}

Point Point::operator-(const Point &p) {
    return Point(this->x - p.x, this->y - p.y, this->z - p.z);
}

Point Point::operator*(const Point &p) {
    return Point(this->x * p.x, this->y * p.y, this->z * p.z);
}

Point Point::operator*(const double &n) {
    return Point(this->x * n, this->y * n, this->z * n);
}

Point Point::operator/(const Point &p) {
    return Point(this->x / p.x, this->y / p.y, this->z / p.z);
}

Point Point::operator/(const double &n) {
    return Point(this->x / n, this->y / n, this->z / n);
}

double Point::getDistance(const Point &p) {
    double tempx = (x - p.x) * (x - p.x);
    double tempy = (y - p.y) * (y - p.y);
    double tempz = (z - p.z) * (z - p.z);
    return sqrt(tempx + tempy + tempz);
}

void Point::print() const {
    std::cout << "point(" << x << ", " << y << ", " << z << ")\n";
}