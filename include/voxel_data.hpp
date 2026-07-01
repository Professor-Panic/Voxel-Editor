#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm>
#include "camera/camera.hpp"
#include "engine.hpp"
struct Voxel{
    int index;
    Color color=RED;
};
void DrawVoxel(Voxel& voxel,BoundingBox bbox,Vector3 gridDimensions,Vector3 voxelSize);
void UpdateVoxelArray(std::vector<Voxel>&voxels,std::vector<bool>&occupied_indices,VoxelMode v,BoundingBox &bbox,VoxCam cam,Vector3 gridDimensions,Vector4 curPallete);
int GetGridIndex(Vector3 pos,BoundingBox bbox,Vector3 gridDimensions);
Voxel GetHoverVoxel(std::vector<bool>&occupied_indices,BoundingBox &bbox,VoxCam cam,Vector3 gridDimensions);