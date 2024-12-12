#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector addition
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Vector subtraction
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Unary negation
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    // Scalar multiplication
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Element-wise multiplication
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    // Dot product
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    
    // Cross product
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Normalize
    Vec3 normalize() const {
        float mag = std::sqrt(x * x + y * y + z * z);
        return (mag > 0) ? (*this * (1.0f / mag)) : Vec3(0, 0, 0);
    }

    // Scalar division
    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
};


struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction) {}
};

class Sphere {
public:
    Vec3 center, color, velocity;
    float radius;

    Sphere(Vec3 c, float r, Vec3 col, Vec3 v = Vec3(0, 0, 0))
        : center(c), radius(r), color(col), velocity(v) {}
    float intersect(const Ray& ray) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return -1.0f;

        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        return (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1.0f);
    }
};

struct Light {
    Vec3 position;
    float intensity;

    Light(const Vec3& position, float intensity)
        : position(position), intensity(intensity) {}
};


// Camera class
class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float fov;
    float aperture;
    float focusDistance;

    // Default constructor
    Camera()
        : position(Vec3(0, 0, 0)),
          target(Vec3(0, 0, -1)),
          up(Vec3(0, 1, 0)),
          fov(45.0f),
          aperture(0.1f),
          focusDistance(1.0f) {}

    // Parameterized constructor
    Camera(const Vec3& pos, const Vec3& tar, const Vec3& u, float fieldOfView, float ap, float focusDist)
        : position(pos), target(tar), up(u), fov(fieldOfView), aperture(ap), focusDistance(focusDist) {}

    void setCamera(const Vec3& pos, const Vec3& tar, const Vec3& u, float fieldOfView, float ap, float focusDist) {
        position = pos;
        target = tar;
        up = u;
        fov = fieldOfView;
        aperture = ap;
        focusDistance = focusDist;
    }

    Ray getRay(float u, float v) const {
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.cross(up).normalize();
        Vec3 trueUp = right.cross(forward);

        float aspectRatio = 16.0f / 9.0f;
        float theta = fov * M_PI / 180.0f;
        float halfHeight = tan(theta / 2);
        float halfWidth = aspectRatio * halfHeight;

        Vec3 lowerLeftCorner = position + forward * focusDistance - right * halfWidth - trueUp * halfHeight;
        Vec3 horizontal = right * (2 * halfWidth);
        Vec3 vertical = trueUp * (2 * halfHeight);

        Vec3 rayDirection = (lowerLeftCorner + horizontal * u + vertical * v - position).normalize();
        return Ray(position, rayDirection);
    }
};

#endif