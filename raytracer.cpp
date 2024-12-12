#include "raytracer.hpp"
#include <cstdlib>
#include <limits>


// void RayTracer::setupScene() {
//     spheres.clear();
//     spheres.emplace_back(Vec3(0, 0, 3), 1.0f, Vec3(1, 0, 0));  // Red sphere
//     spheres.emplace_back(Vec3(-1.5, 0.5, 4), 0.5f, Vec3(0, 1, 0));  // Green sphere
//     spheres.emplace_back(Vec3(1.5, -0.5, 4), 0.5f, Vec3(0, 0, 1));  // Blue sphere
//     spheres.emplace_back(Vec3(0.5, 1, 2.5), 0.3f, Vec3(1, 1, 0));   // Yellow sphere

//     lights.clear();
//     lights.emplace_back(Vec3(-2, 5, 5), 0.8f); // Add a light source

//     // Add ground plane
//     planePoint = Vec3(0, -1, 0);  // Plane at y = -1
//     planeNormal = Vec3(0, 1, 0);  // Upward normal
//     planeColor = Vec3(0.8f, 0.8f, 0.8f);  // Light gray
// }

void RayTracer::setupScene() {
    spheres.clear();
    lights.clear();

    // Adding Spheres (Three spheres only)
    // Sphere in focus (centered at z=-2.0f)
    spheres.emplace_back(Sphere(Vec3(0.0f, 0.0f, -2.0f), 0.5f, Vec3(1.0f, 0.0f, 0.0f))); // Red sphere

    // Sphere in front (blurry, z=-1.0f, closer to camera)
    spheres.emplace_back(Sphere(Vec3(0.6f, 0.0f, -1.0f), 0.5f, Vec3(0.0f, 1.0f, 0.0f))); // Green sphere (slightly more blur)

    // Sphere behind (blurry, z=-3.0f, slightly smaller, positioned slightly left and slightly above)
    spheres.emplace_back(Sphere(Vec3(-0.35f, -0.05f, -3.0f), 0.4f, Vec3(0.0f, 0.0f, 1.0f))); // Blue sphere (strong blur)

    // Adding Ground (distant ground plane for context)
    spheres.emplace_back(Sphere(Vec3(0.0f, -100.5f, -2.0f), 100.0f, Vec3(0.5f, 0.5f, 0.5f))); // Ground plane

    // Adding Light (basic light source)
    lights.emplace_back(Light(Vec3(0.0f, 3.0f, -1.0f), 1.0f)); // Light source

    // Configuring Camera
    camera.setCamera(
        Vec3(0.0f, 0.0f, 0.0f),  // Camera position (centered to view all spheres)
        Vec3(0.0f, 0.0f, -3.0f), // Look-at target (focus on the red sphere)
        Vec3(0.0f, 1.0f, 0.0f),  // Up direction
        45.0f,                   // Field of View (FoV)
        0.0f,                    // Aperture (increase to enhance blur)
        0.0f                     // Focus distance 
    );
}


bool RayTracer::intersectPlane(const Ray& ray, Vec3& hitPoint, Vec3& normal) const {
    float denom = planeNormal.dot(ray.direction);
    if (fabs(denom) > 1e-6) {  // Avoid division by zero
        float t = (planePoint - ray.origin).dot(planeNormal) / denom;
        if (t > 0) {  // Intersection occurs in front of the ray
            hitPoint = ray.origin + ray.direction * t;
            normal = planeNormal;
            return true;
        }
    }
    return false;
}

void RayTracer::renderFrame(float timeDelta, float effectValue, bool useDOF, int samplesPerPixel) {
    float aspectRatio = static_cast<float>(width) / height;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec3 colorSum(0, 0, 0);

            for (int sample = 0; sample < samplesPerPixel; ++sample) {
                // Jittered sampling for anti-aliasing
                float u = (x + (rand() / (float)RAND_MAX - 0.5f)) / width;
                float v = (y + (rand() / (float)RAND_MAX - 0.5f)) / height;

                // Transform u, v to viewport space
                Vec3 origin(cameraPosition);
                Vec3 direction = (forward + right * ((u - 0.5f) * 2 * aspectRatio) 
                                            + cameraUp * ((v - 0.5f) * 2)).normalize();
                Ray primaryRay(origin, direction);

                // Apply DOF if enabled
                if (useDOF) {
                    Vec3 focalPoint = primaryRay.origin + primaryRay.direction * focusDistance;
                    primaryRay = jitterApertureRay(origin, focalPoint);
                }

                colorSum = colorSum + trace(primaryRay, timeDelta);
            }

            // Average colors
            framebuffer[y * width + x] = colorSum / samplesPerPixel;
        }
    }
}



Ray RayTracer::jitterApertureRay(const Vec3& origin, const Vec3& focusPoint) const {
    // Random point on a disk for the aperture
    float r = apertureSize * sqrt((float)rand() / RAND_MAX);  // Random radius on disk
    float theta = 2 * M_PI * ((float)rand() / RAND_MAX);  // Random angle

    // Random offset based on aperture
    Vec3 apertureOffset(
        r * cos(theta), 
        r * sin(theta), 
        0 // Assume aperture lies in the camera's xy-plane
    );

    // Adjust origin with aperture offset
    Vec3 newOrigin = origin + apertureOffset;

    // Compute the new direction towards the focal point
    Vec3 newDirection = (focusPoint - newOrigin).normalize();
    return Ray(newOrigin, newDirection);
}


void RayTracer::updateCameraBasis() {
    // Calculate the forward vector: normalized direction from camera to focus point
    forward = (focusPoint - cameraPosition).normalize();

    // Calculate the right vector: perpendicular to forward and the up vector
    right = forward.cross(upVector).normalize();

    // Calculate the up vector: perpendicular to both right and forward (adjusted)
    cameraUp = right.cross(forward).normalize();
}



// For 1 ray / px use this
// void RayTracer::renderFrame(float timeDelta, float effectValue) {
//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             float u = (x + 0.5f) / width;
//             float v = (y + 0.5f) / height;
//             Vec3 origin(0, 0, 0);  // Camera position
//             Vec3 direction(u * 2 - 1, v * 2 - 1, 1);  // Basic perspective
//             Ray ray(origin, direction.normalize());
            
//             // Apply jittering for motion blur
//             Ray jittered_ray = jitteredRay(ray, effectValue);
//             framebuffer[y * width + x] = trace(jittered_ray, timeDelta);
//         }
//     }
// }

// For multi ray / px use this

// void RayTracer::renderFrame(float timeDelta, float effectValue, int samplesPerPixel)  {
    
//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             Vec3 colorSum(0, 0, 0);  // Sum of colors for averaging
            
//             for (int sample = 0; sample < samplesPerPixel; ++sample) {
//                 // Randomly jitter the pixel coordinates for supersampling
//                 float u = (x + (rand() / (float)RAND_MAX - 0.5f)) / width;
//                 float v = (y + (rand() / (float)RAND_MAX - 0.5f)) / height;
//                 Vec3 origin(0, 0, 0);  // Camera position
//                 Vec3 direction(u * 2 - 1, v * 2 - 1, 1);  // Basic perspective
//                 Ray ray(origin, direction.normalize());
                
//                 // Apply jittering for motion blur
//                 Ray jittered_ray = jitteredRay(ray, effectValue);
                
//                 // Accumulate the color from the traced ray
//                 colorSum = colorSum + trace(jittered_ray, timeDelta);
//             }
            
//             // Average the colors and store in framebuffer
//             framebuffer[y * width + x] = colorSum / samplesPerPixel;
//         }
//     }
// }


Vec3 RayTracer::trace(const Ray& ray, float timeDelta) const {
    const Sphere* hitSphere = nullptr;
    float closest = std::numeric_limits<float>::max();
    Vec3 hitPoint, normal, planeHitPoint, planeNormal;

    // Check intersection with spheres
    for (const auto& sphere : spheres) {
        float t = sphere.intersect(ray);
        if (t > 0 && t < closest) {
            closest = t;
            hitSphere = &sphere;
            hitPoint = ray.origin + ray.direction * t;
            normal = (hitPoint - sphere.center).normalize();
        }
    }

    // Check intersection with plane
    if (intersectPlane(ray, planeHitPoint, planeNormal)) {
        float planeDist = (planeHitPoint - ray.origin).length();
        if (planeDist < closest) {
            closest = planeDist;
            hitSphere = nullptr;  // No sphere hit
            hitPoint = planeHitPoint;
            normal = planeNormal;
        }
    }

    // Determine color based on hit
    if (closest < std::numeric_limits<float>::max()) {
        Vec3 viewDir = -ray.direction;
        if (hitSphere) {
            return computeLighting(hitPoint, normal, viewDir, timeDelta, hitSphere) * hitSphere->color;
        } else {
            return computeLighting(hitPoint, normal, viewDir, timeDelta, nullptr) * planeColor;
        }
    }

    return Vec3(0.53f, 0.81f, 0.92f);  // Light sky blue background color
}


Vec3 RayTracer::computeLighting(const Vec3& point, const Vec3& normal, const Vec3& viewDir, float timeDelta, const Sphere* hitSphere) const {
    Vec3 lighting(0.1f, 0.1f, 0.1f);  // Ambient light for dim shadow areas
    for (const auto& light : lights) {
        // Jitter light position for soft shadows
        Vec3 lightPos = light.position + jitterLight();

        Vec3 lightDir = (lightPos - point).normalize();
        float intensity = light.intensity * std::max(0.0f, normal.dot(lightDir));

        // Check for shadows
        Ray shadowRay(point + normal * 1e-4f, lightDir); // Offset the origin to prevent self-intersection
        bool shadowed = false;
        for (const auto& sphere : spheres) {
            if (&sphere != hitSphere && sphere.intersect(shadowRay) > 0) {
                shadowed = true;
                break;
            }
        }

        // If shadowed, reduce intensity for a dim shadow effect
        if (shadowed) {
            lighting = lighting + Vec3(0.3f, 0.3f, 0.3f) * intensity;  // Adjust shadow intensity
        } else {
            lighting = lighting + Vec3(1, 1, 1) * intensity;  // Full intensity
        }
    }
    return lighting;
}



// Jitter function for motion blur
Ray RayTracer::jitteredRay(const Ray& ray, float effectValue) const {
    // Increased jitter values to simulate more motion blur
    Vec3 jitterOffset(
        ((float)rand() / RAND_MAX - 0.5f) * effectValue * 0.01f,  // Increased factor (0.05f) for more motion
        ((float)rand() / RAND_MAX - 0.5f) * effectValue * 0.01f,
        ((float)rand() / RAND_MAX - 0.5f) * effectValue * 0.01f
    );
    return Ray(ray.origin + jitterOffset, ray.direction);
}

// Jitter function for soft shadow
Vec3 RayTracer::jitterLight() const {
    float jitterAmount = 0.2f;  // Adjust for softness
    return Vec3(
        ((float)rand() / RAND_MAX - 0.5f) * jitterAmount,
        ((float)rand() / RAND_MAX - 0.5f) * jitterAmount,
        ((float)rand() / RAND_MAX - 0.5f) * jitterAmount
    );
}
