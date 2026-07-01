#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

#include "voxel_data.hpp"
#include "camera/camera.hpp"
int main(){
    Vector3 gridDimensions={32,32,32};
    VoxelMode mode=VoxelMode::Attach;
    std::vector<Voxel>voxels;
    std::vector<bool>occupied_indices(int(gridDimensions.x*gridDimensions.y*gridDimensions.z));
    int numSteps=50;
    InitWindow(1920,1080,"Voxel Editor");
    VoxCam cam;
    cam.camera.position={0,5,5};
    cam.camera.target={0,0,0};

    cam.camera.fovy=45.0f;

    cam.camera.projection=CAMERA_PERSPECTIVE;

    cam.camera.up={0,1,0};

    float yaw=GetMouseDelta().x*0.01; 

    float pitch=GetMouseDelta().y*0.01;

    BoundingBox bbox;

    bbox.max={2.5,2.5,2.5};

    bbox.min={-2.5,-2.5,-2.5};
    Vector4 curPallete={0,1,1,1};
    //voxel bounds

    Vector3 voxelSize={abs(bbox.max.x-bbox.min.x)/gridDimensions.x,
                      abs(bbox.max.y-bbox.min.y)/gridDimensions.y,
                      abs(bbox.max.z-bbox.min.z)/gridDimensions.z};

    rlImGuiSetup(true);

    SetTargetFPS(60);
    ToggleFullscreen();
    while(!WindowShouldClose()){
        CameraScroll(cam);
        CameraPan(cam);
        CameraOrbit(cam);
        UpdateVoxelArray(voxels,occupied_indices,mode,bbox,cam,gridDimensions,curPallete);
        if(IsKeyPressed(KEY_R)){
            voxels.clear();
            std::fill(occupied_indices.begin(), occupied_indices.end(), false);
        }
        if(IsKeyPressed(KEY_F)){
            mode=VoxelMode::Erase;
        }
        if(IsKeyPressed(KEY_T)){
            mode=VoxelMode::Attach;
        }
         if(IsKeyPressed(KEY_P)){
            mode=VoxelMode::Paint;
        }
        Voxel hoverVoxel=GetHoverVoxel(occupied_indices,bbox,cam,gridDimensions);
        BeginDrawing();
        BeginMode3D(cam.camera);
        ClearBackground({10,10,10,255});
        DrawBoundingBox(bbox,WHITE);
        for(auto&voxel:voxels){
           DrawVoxel(voxel,bbox,gridDimensions,voxelSize);
        }
        DrawVoxel(hoverVoxel,bbox,gridDimensions,voxelSize);
        EndMode3D();
        rlImGuiBegin();
        ImGui::Begin("Cam Controls");
        ImGui::Text(TextFormat("Mouse Wheel Movement %.2f,%.2f",GetMouseWheelMoveV().x,GetMouseWheelMoveV().y));
        ImGui::SliderFloat3("Camera Controls",&cam.camera.position.x,-5,5);
        ImGui::Text(TextFormat("Pitch: %.3f",pitch));
        ImGui::Text(TextFormat("Yaw: %.3f",yaw));
        ImGui::End();
        ImGui::Begin("Box Controls");
        ImGui::ColorPicker4("Current Color: ",&curPallete.x);
        ImGui::ColorButton("Color",ImVec4(curPallete.x,curPallete.y,curPallete.z,curPallete.w));
        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }
    rlImGuiShutdown();
    CloseWindow();
}