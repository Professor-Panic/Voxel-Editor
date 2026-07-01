#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include <algorithm>
struct Voxel{
    int index;
    Color color=RED;
};
void DrawVoxel(Voxel& voxel,BoundingBox bbox,Vector3 gridDimensions,Vector3 voxelSize){
    int x = voxel.index % (int)gridDimensions.x;
    int y = (voxel.index / (int)gridDimensions.x) % (int)gridDimensions.y;
    int z = voxel.index / (int)(gridDimensions.x * gridDimensions.y);
    Vector3 pos;
    pos.x = bbox.min.x + (x + 0.5f) * voxelSize.x;
    pos.y = bbox.min.y + (y + 0.5f) * voxelSize.y;
    pos.z = bbox.min.z + (z + 0.5f) * voxelSize.z;

    DrawCube(pos, voxelSize.x, voxelSize.y, voxelSize.z, voxel.color);
}
int main(){
    int gridX=32;
    int gridY=32;
    int gridZ=32;
    int mode=0;
    std::vector<Voxel>voxels;
    std::vector<bool>occupied_indices(64*64*64);

    InitWindow(1920,1080,"Voxel Editor");
    Camera3D cam;
    cam.position={0,5,5};
    cam.target={0,0,0};

    cam.fovy=45.0f;

    cam.projection=CAMERA_PERSPECTIVE;

    cam.up={0,1,0};

    float yaw=GetMouseDelta().x*0.01; 

    float pitch=GetMouseDelta().y*0.01;

    BoundingBox bbox;

    bbox.max={2.5,2.5,2.5};

    bbox.min={-2.5,-2.5,-2.5};
    Vector4 curPallete={0,0,0,1};
    //voxel bounds

    Vector3 voxelSize={abs(bbox.max.x-bbox.min.x)/gridX,
                      abs(bbox.max.y-bbox.min.y)/gridY,
                      abs(bbox.max.z-bbox.min.z)/gridZ};

    rlImGuiSetup(true);

    SetTargetFPS(60);
    ToggleFullscreen();
    while(!WindowShouldClose()){
        //Camera Scrolling
        if(GetMouseWheelMoveV().y>0){
            float dist=Vector3Distance(cam.position,cam.target);
            if(dist>0.0){
                Vector3 dir=Vector3Scale((cam.target-cam.position),1.0/dist);
                cam.position+=dir;
            }
            
        }
        if(GetMouseWheelMoveV().y<0){
            float dist=Vector3Distance(cam.position,cam.target);
            if(dist>0.0){
                Vector3 dir=Vector3Scale((cam.target-cam.position),1.0/dist);
                cam.position-=dir;
            }
        }
        //Movement when Right clicking
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            yaw-=GetMouseDelta().x*0.005; 
            pitch+=GetMouseDelta().y*0.005;
            float distance=Vector3Distance(cam.position,cam.target);
            cam.position.x = cam.target.x + distance * cos(pitch) * sin(yaw);
            cam.position.y = cam.target.y + distance * sin(pitch);
            cam.position.z = cam.target.z + distance * cos(pitch) * cos(yaw);
            if(yaw>2*PI||yaw<-2*PI){
                yaw=0;
            }
            if(pitch>=PI/2-5*DEG2RAD){
                pitch=PI/2-5*DEG2RAD;
            }

        }
        //Movement when middle clicking
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){
            float dx=GetMouseDelta().x*0.01;
            float dy=GetMouseDelta().y*0.01;
            Vector3 forward = Vector3Normalize(Vector3Subtract(cam.target, cam.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, cam.up));
            Vector3 up = cam.up;

            Vector3 drag =
                Vector3Scale(right, -dx) +
                Vector3Scale(up, dy);

            cam.position += drag;
            cam.target += drag;
        }
        Ray mouseRay=GetScreenToWorldRay(GetMousePosition(),cam);
        RayCollision mousehit=GetRayCollisionBox(mouseRay,bbox);  
        Voxel hoverVoxel;
        hoverVoxel.color=RAYWHITE; 
        //Form A ray using the forward direction of the cam
        //If you hit form a cube there
        if(mousehit.hit){
            Vector3 forward=Vector3Normalize(Vector3Subtract(cam.target, cam.position));
            float dot=Vector3DotProduct(forward,mousehit.normal);
            //if the dot product is negative it means you face roughly in the opposite direction 
            // maybe you want to place the voxels inside the cube 
            if(dot<0.0f){
                Ray insideRay;
                insideRay.position=mousehit.point+(mouseRay.direction*0.01);
                insideRay.direction=mouseRay.direction;
                mousehit=GetRayCollisionBox(insideRay,bbox);
                if(mousehit.hit){
                    Vector3 normPos=(mousehit.point-bbox.min)/(bbox.max-bbox.min);
                    int x = Clamp((int)floorf(normPos.x * gridX), 0, gridX - 1);
                    int y = Clamp((int)floorf(normPos.y * gridY), 0, gridY - 1);
                    int z = Clamp((int)floorf(normPos.z * gridZ), 0, gridZ - 1);
                    int index = x + y * gridX + z * gridX * gridY;
                    hoverVoxel.index=index;
                    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                        if(mode==0){
                            if(!occupied_indices[index]){
                                occupied_indices[index]=true;
                                Voxel v;
                                v.index=hoverVoxel.index;
                                voxels.push_back(v);
                            }
                        }
                        else if(mode==1){
                            if(occupied_indices[index]){
                                occupied_indices[index]=false;
                                voxels.erase(
                                std::remove_if(voxels.begin(), voxels.end(),
                                    [index](const Voxel& v) {
                                        return v.index == index;
                                    }));
                            }
                        }
                        else if(mode==2){
                            if(occupied_indices[index]){
                                for(Voxel&v:voxels){
                                    if(v.index==index){
                                        v.color=ColorFromNormalized(curPallete);
                                    }
                                }
                            }
                        }
                    }
                    
                }
            }
        }
        if(IsKeyPressed(KEY_R)){
            voxels.clear();
        }
        if(IsKeyPressed(KEY_F)){
            mode=1;
        }
        if(IsKeyPressed(KEY_T)){
            mode=0;
        }
         if(IsKeyPressed(KEY_P)){
            mode=2;
        }
        BeginDrawing();
        BeginMode3D(cam);
        ClearBackground({10,10,10,255});
        DrawBoundingBox(bbox,WHITE);
        for(auto&voxel:voxels){
           DrawVoxel(voxel,bbox,{(float)gridX,(float)gridY,(float)gridZ},voxelSize);
        }
        if(mousehit.hit){
            DrawVoxel(hoverVoxel,bbox,{(float)gridX,(float)gridY,(float)gridZ},voxelSize);
        }
        EndMode3D();
        rlImGuiBegin();
        ImGui::Begin("Cam Controls");
        ImGui::Text(TextFormat("Mouse Wheel Movement %.2f,%.2f",GetMouseWheelMoveV().x,GetMouseWheelMoveV().y));
        ImGui::SliderFloat3("Camera Controls",&cam.position.x,-5,5);
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