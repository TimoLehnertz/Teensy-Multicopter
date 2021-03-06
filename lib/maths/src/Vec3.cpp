/**
 * @file Vec3.cpp
 * @author Timo Lehnertz
 * @brief 
 * @version 0.1
 * @date 2022-01-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "maths.h"
#include <Arduino.h>

int strpos2(const char* haystack, const char needle, int start = 0) {
  for(int i = start; i < 100; i++) {
    if(haystack[i] == needle) return i;
  }
  return -1;
}

Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(double s) : x(s), y(s), z(s) {}

Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

Vec3::Vec3(double v[]) : x(v[0]), y(v[1]), z(v[2]) {}

Vec3::Vec3(char* str) {
    // if(strlen(str) < 7) return;
    // int delim1 = strpos2(str, ',');
    // str[delim1] = 0;
    // int delim2 = strpos2(str, ',', delim1 + 1);
    // str[delim2] = 0;
    // int delim3 = strpos2(str, ',', delim2 + 1);
    // str[delim3] = 0;
    // x = atof(str + 1);
    // y = atof(str + delim2 + 1);
    // z = atof(str + delim3 + 1);

    if(strlen(str) < 7) return;
    int delims[4];
    delims[0] = 0;
    bool succsess = true;
    for (size_t i = 0; i < 4; i++) {
        int start = i ? delims[i - 1] + 1 : 0;
        delims[i] = strpos2(str, ',', start);
        if(delims[i] < 0) {
            succsess = false;
            break;
        }
        str[delims[i]] = 0;
    }
    if(succsess) {
        x            = atof(str + delims[0] + 1);
        y            = atof(str + delims[1] + 1);
        z            = atof(str + delims[2] + 1);
        // print();
    } else {
        Serial.println("Vec3 String was formatted wrongly");
    }
}

double Vec3::getLength() const {
    return sqrt(x*x+y*y+z*z);
}

double Vec3::getLength2D() const {
    return sqrt(x*x+y*y);
}

void Vec3::setLength(double len) {
    double lenBefore = getLength();
    if(lenBefore > 0) {
        setFrom(*this / lenBefore * len);
    }
}

void Vec3::setLength2D(double len) {
    double lenBefore = getLength2D();
    if(lenBefore > 0) {
        x = x / lenBefore * len;
        y = y / lenBefore * len;
    }
}

double Vec3::dot(const Vec3& v) const {
    return *this * v;
}

Vec3 Vec3::crossProduct(const Vec3& v) const {
    Vec3 res;
    res.x = y * v.z - z * y;
    res.y = -(x * v.z - z * x);
    res.z = x * v.y - y * v.x;
    return res;
}

void Vec3::setFrom(const Vec3& v) {
    x = v.x;
    y = v.y;
    z = v.z;
}

void Vec3::setFrom(const Quaternion& q) {
    x = q.x;
    y = q.y;
    z = q.z;
}

void Vec3::setFrom(const Matrix3& m) {
    x = m.m[0];
    y = m.m[3];
    z = m.m[6];
}

double Vec3::getValue() const {
    return x + y + z;
}

double Vec3::getAxis(int axis) {
    switch(axis) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return 0;
    }
}

double Vec3::setAxis(int axis, double val) {
    switch(axis) {
        case 0: x = val;
        case 1: y = val;
        case 2: z = val;
    }
}

Vec3 Vec3::toRad() {
    x = (x * 71) / 4068;
    y = (y * 71) / 4068;
    z = (z * 71) / 4068;
    return *this;
}

Vec3 Vec3::toDeg() {
    // x = (x * 4068) / 71;
    // y = (y * 4068) / 71;
    // z = (z * 4068) / 71;
    x *= RAD_TO_DEG;
    y *= RAD_TO_DEG;
    z *= RAD_TO_DEG;
    return *this;
}

Vec3 Vec3::toG() {
    x = x / 9.807;
    y = y / 9.807;
    z = z / 9.807;
    return *this;
}

void Vec3::toUnitLength() {
    double len = getLength();
    if(len == 0) return;
    x /= len;
    y /= len;
    z /= len;
}

double Vec3::absSum() const {
    return abs(x) + abs(y) + abs(z);
}

bool Vec3::equals(const Vec3& v) const {
    return x == v.x && y == v.y && z == v.z;
}

bool Vec3::greaterThan(const Vec3& v) const {
    return x > v.x && y > v.y && z > v.z;
}

bool Vec3::greaterThanEquals(const Vec3& v) const {
    return x >= v.x && y >= v.y && z >= v.z;
}

Vec3 Vec3::min(const Vec3& a, const Vec3& b) {
    return Vec3(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

Vec3 Vec3::max(const Vec3& a, const Vec3& b) {
    return Vec3(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}

Vec3 Vec3::clone() const {
    return Vec3(x, y, z);
}

// Vector operators
Vec3 Vec3::operator + (const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator - (const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator * (const Vec3& v) const{
    return Vec3(x * v.x, y * v.y, z * v.z);
}

Vec3 Vec3::operator / (const Vec3& v) const {
    return Vec3(x / v.x, y / v.y, z / v.z);
}

Vec3 Vec3::operator ^ (const Vec3& v) const {
    return Vec3(pow(x, v.x), pow(y, v.y), pow(z, v.z));
}


Vec3 Vec3::operator += (const Vec3& v) {
    *this = *this + v;
    return *this;
}

Vec3 Vec3::operator -= (const Vec3& v) {
    *this = *this - v;
    return *this;
}

Vec3 Vec3::operator *= (const Vec3& v) {
    *this = *this * v;
    return *this;
}

Vec3 Vec3::operator /= (const Vec3& v) {
    *this = *this / v;
    return *this;
}

Vec3 Vec3::operator ^= (const Vec3& v) {
    *this = *this ^ v;
    return *this;
}

// Scalar operators
Vec3 Vec3::operator + (double s) const {
    return Vec3(x + s, y + s, z + s);
}

Vec3 Vec3::operator - (double s) const {
    return Vec3(x - s, y - s, z - s);
}

Vec3 Vec3::operator * (double s) const {
    return Vec3(x * s, y * s, z * s);
}

Vec3 Vec3::operator * (float s) const {
    return Vec3(x * s, y * s, z * s);
}

Vec3 Vec3::operator / (double s) const {
    return Vec3(x / s, y / s, z / s);
}

// Vec3 Vec3::operator / (long s) const {
//     return Vec3(x / s, y / s, z / s);
// }

Vec3 Vec3::operator ^ (double s) const {
    return Vec3(pow(x, s), pow(y, s), pow(z, s));
}

Vec3 Vec3::operator += (double s) {
    *this = *this + s;
    return *this;
}

Vec3 Vec3::operator -= (double s) {
    *this = *this - s;
    return *this;
}

Vec3 Vec3::operator *= (double s) {
    *this = *this * s;
    return *this;
}

Vec3 Vec3::operator /= (double s) {
    *this = *this / s;
    return *this;
}

Vec3 Vec3::operator ^= (double s) {
    *this = *this ^ s;
    return *this;
}


Vec3 Vec3::operator = (const Vec3& v) {
    setFrom(v);
    return *this;
}

Vec3 Vec3::operator = (const Matrix3& m) {
    setFrom(m);
    return *this;
}

// comparison operators
bool Vec3::operator == (const Vec3& v) const{
    return equals(v);
}

bool Vec3::operator != (const Vec3& v) const {
    return !equals(v);
}

bool Vec3::operator > (const Vec3& v) const{
    return greaterThan(v);
}

bool Vec3::operator >= (const Vec3& v) const {
    return !greaterThanEquals(v);
}

bool Vec3::operator < (const Vec3& v) const{
    return !greaterThanEquals(v);
}

bool Vec3::operator <= (const Vec3& v) const {
    return !greaterThan(v);
}

//conversion operator
Vec3::operator double() {
    return getValue();
}

Matrix3 Vec3::toMatrix3() const {
    return Matrix3(x, 0, 0, y, 0, 0, z, 0, 0);
}

Vec3::operator Matrix3() {
    return toMatrix3();
}