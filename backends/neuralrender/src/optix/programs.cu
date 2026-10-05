#include "launchParams.h"


#include <optix_device.h>

extern "C" __constant__ LaunchParams optixLaunchParams;


extern "C" __global__ void __miss__radiance()
  { /*! for this simple example, this will remain empty */ }


extern "C" __global__ void __raygen__renderFrame() {

  unsigned int w = optixLaunchParams.width;
  unsigned int h = optixLaunchParams.height;

  const uint3 launch_idx = optixGetLaunchIndex();

  const float u = 2.0f * ((static_cast<float>(launch_idx.x) + 0.5f) / static_cast<float>(w)) - 1.0f;

  const float v = 1.0f - 2.0f * ((static_cast<float>(launch_idx.y) + 0.5f)/ static_cast<float>(h));

  const float aspectRatio =
      static_cast<float>(w) / static_cast<float>(h);

  const float3 rayOrigin = make_float3(0.0f, 0.0f, 0.0f);

  const float3 imagePlanePoint = make_float3(u * aspectRatio, v, -1.0f);

  float3 rayDirection = imagePlanePoint - rayOrigin;

  const float length = sqrtf(
          rayDirection.x * rayDirection.x +
          rayDirection.y * rayDirection.y +
          rayDirection.z * rayDirection.z
      );

    rayDirection.x /= length;
    rayDirection.y /= length;
    rayDirection.z /= length;
    optixTrace(
    static_cast<OptixTraversableHandle>(0), // scene to trace through
                                           // 0 because Phase 2 has no geometry / acceleration structure yet

    rayOrigin,                             // where the ray starts
                                           // for now: camera at (0,0,0)

    rayDirection,                          // normalized direction the ray travels
                                           // built from the current pixel's image-plane position

    0.001f,                                // tmin: ignore intersections closer than this
                                           // avoids self-intersection issues later

    1e20f,                                 // tmax: stop searching after this distance
                                           // effectively "very far away"

    0.0f,                                  // ray time
                                           // only matters for motion blur / time-varying geometry
                                           // not used in Phase 2

    OptixVisibilityMask(255),              // which geometry this ray is allowed to see
                                           // 255 = all 8 visibility bits enabled

    OPTIX_RAY_FLAG_NONE,                   // no special ray-tracing behavior
                                           // later flags can skip any-hit, terminate early, etc.

    0,                                     // SBT offset
                                           // only one ray type right now, so start at record 0

    1,                                     // SBT stride
                                           // move by 1 record per ray type
                                           // Phase 2 only has one ray type

    0                                      // miss SBT index
                                           // use miss record 0, which will point to __miss__radiance
);




}
