#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <vector>
#include <memory>
#include "utilities.hpp"

class RayTracer {
public:
    // RayTracer(int width, int height)
    //     : width(width), height(height), framebuffer(width * height) {}

    // Constructor updated to include aperture size and focus distance
    std::vector<std::shared_ptr<Sphere>> objects; // Store objects (like spheres, planes, etc.)
    std::vector<Light> lights;                   // Store light sources
    RayTracer(int width, int height, float aperture = 0.05f, float focusDist = 3.0f,
              const Vec3& cameraPos = Vec3(0.0f, 0.0f, -5.0f),
              const Vec3& focusPoint = Vec3(0.0f, 0.0f, 0.0f),
              const Vec3& upVector = Vec3(0.0f, 1.0f, 0.0f))
        : width(width), height(height), framebuffer(width * height), 
          apertureSize(aperture), focusDistance(focusDist), 
          cameraPosition(cameraPos), focusPoint(focusPoint), upVector(upVector) {
        updateCameraBasis();
    }
    Camera camera = Camera(Vec3(0, 0, 0), Vec3(0, 0, -1), Vec3(0, 1, 0), 45.0f, 0.0f, 1.0f);

    void updateCameraBasis();
    void setupScene();
    //FOR DOF
    void renderFrame(float timeDelta, float effectValue, bool useDOF = false, int samplesPerPixel = 1);

    //For 1 ray / px use this
    // void renderFrame(float timeDelta, float effectValue);
    //For multi ray / px use this
    // void renderFrame(float timeDelta, float effectValue, int samplesPerPixel);


    Vec3 trace(const Ray& ray, float timeDelta) const;
    Vec3 computeLighting(const Vec3& point, const Vec3& normal, const Vec3& viewDir, float timeDelta, const Sphere* hitSphere) const;
    Vec3 jitterLight() const;
    Ray jitteredRay(const Ray& ray, float effectValue) const;  //  function for motion blur

    //DOF helper
    Ray jitterApertureRay(const Vec3& origin, const Vec3& focusPoint) const;

    const std::vector<Vec3>& getFramebuffer() const { return framebuffer; }

public:
    bool intersectPlane(const Ray& ray, Vec3& hitPoint, Vec3& normal) const;  // function for plane intersection

public:
    int width, height;
    float apertureSize;   // Diameter of the lens aperture
    float focusDistance;  // Distance to the focus plane
    Vec3 cameraPosition;  // Position of the camera
    Vec3 focusPoint;      // Point the camera is focused on
    Vec3 upVector;        // Up vector for the camera
    Vec3 forward;         // Camera forward vector
    Vec3 right;           // Camera right vector
    Vec3 cameraUp;        // Adjusted up vector after basis calculation
    std::vector<Sphere> spheres;
    // std::vector<Light> lights;
    std::vector<Vec3> framebuffer;
    Vec3 planePoint;      // A point on the plane
    Vec3 planeNormal;     // Normal vector of the plane
    Vec3 planeColor;      // Color of the plane

};

#endif
