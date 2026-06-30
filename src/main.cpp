#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
struct Voxel{
    Vector3 position;
};
int main(){
    int gridX=64;
    int gridY=64;
    int gridZ=64;
    std::vector<Voxel>voxels;
    InitWindow(1200,800,"Voxel Editor");
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
    //voxel bounds
    float voxelSizeX=abs(bbox.max.x-bbox.min.x)/gridX;
    float voxelSizeY=abs(bbox.max.y-bbox.min.y)/gridY;
    float voxelSizeZ=abs(bbox.max.z-bbox.min.z)/gridZ;
    rlImGuiSetup(true);
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        //Camer Scrolling
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
        //Form A ray using the forward direction of the cam
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Ray mouseRay=GetScreenToWorldRay(GetMousePosition(),cam);
            RayCollision mousehit=GetRayCollisionBox(mouseRay,bbox);
            //If you hit form a cube there
                    if(mousehit.hit){
                Vector3 forward=Vector3Normalize(Vector3Subtract(cam.target, cam.position));
                float dot=Vector3DotProduct(forward,mousehit.normal);
                //if the dot product is negative it means you face roughly in the opposite direction 
                // maybe you want to place the voxels inside the cube 
                if(dot<-0.1f){
                    Ray insideRay;
                    insideRay.position=mousehit.point+mouseRay.direction;
                    insideRay.direction=mouseRay.direction;
                    mousehit=GetRayCollisionBox(insideRay,bbox);
                    if(mousehit.hit){
                        Vector3 normPos=(mousehit.point-bbox.min)/(bbox.max-bbox.min);
                        normPos.x=floor(normPos.x*gridX+0.5f)/gridX;
                        normPos.y=floor(normPos.y*gridY+0.5f)/gridY;
                        normPos.z=floor(normPos.z*gridZ+0.5f)/gridZ;
                        normPos=(normPos*(bbox.max-bbox.min))+bbox.min;
                        Voxel v;
                        v.position=normPos;
                        voxels.push_back(v);
                    }
                }
            }

        }
        
        BeginDrawing();
        BeginMode3D(cam);
        ClearBackground(BLACK);
        DrawBoundingBox(bbox,WHITE);
        //If you did hit something first check what face you hit
        for(auto&voxel:voxels){
            DrawCube(voxel.position,voxelSizeX,voxelSizeY,voxelSizeZ,RED);
        }
        EndMode3D();
        rlImGuiBegin();
        ImGui::Begin("Cam Controls");
        ImGui::Text(TextFormat("Mouse Wheel Movement %.2f,%.2f",GetMouseWheelMoveV().x,GetMouseWheelMoveV().y));
        ImGui::SliderFloat3("Camera Controls",&cam.position.x,-5,5);
        ImGui::End();
        ImGui::Begin("Box Controls");
        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }
    rlImGuiShutdown();
    CloseWindow();
}