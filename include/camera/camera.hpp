#pragma once
#include "raylib.h"
#include "raymath.h"
struct VoxCam{
    Camera3D camera;
    float yaw;
    float pitch;
};
void CameraScroll(VoxCam& cam);
void CameraOrbit(VoxCam& cam);
void CameraPan(VoxCam& cam);