#include "voxel_data.hpp"
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
int GetGridIndex(Vector3 pos,BoundingBox bbox,Vector3 gridDimensions){
    Vector3 normPos=(pos-bbox.min)/(bbox.max-bbox.min);
    int x = Clamp((int)floorf(normPos.x * gridDimensions.x), 0, gridDimensions.x - 1);
    int y = Clamp((int)floorf(normPos.y * gridDimensions.y), 0, gridDimensions.y - 1);
    int z = Clamp((int)floorf(normPos.z * gridDimensions.z), 0, gridDimensions.z - 1);
    return x + y * gridDimensions.x + z * gridDimensions.x * gridDimensions.y;
};
void UpdateVoxelArray(std::vector<Voxel>&voxels,std::vector<bool>&occupied_indices,VoxelMode v,BoundingBox &bbox,VoxCam cam,Vector3 gridDimensions,Vector4 curPallete){
        Ray mouseRay=GetScreenToWorldRay(GetMousePosition(),cam.camera);
        RayCollision mousehit=GetRayCollisionBox(mouseRay,bbox);  
        Voxel hoverVoxel;
        hoverVoxel.color=RAYWHITE; 
        //Form A ray using the forward direction of the cam
        //If you hit form a cube there
        if(mousehit.hit){
            Vector3 forward=Vector3Normalize(Vector3Subtract(cam.camera.target, cam.camera.position));
            float dot=Vector3DotProduct(forward,mousehit.normal);
            //if the dot product is negative it means you face roughly in the opposite direction 
            // maybe you want to place the voxels inside the cube 
            if(dot<0.0f){
                Ray insideRay;
                insideRay.position=mousehit.point+(mouseRay.direction*0.01);
                insideRay.direction=mouseRay.direction;
                mousehit=GetRayCollisionBox(insideRay,bbox);
                if(mousehit.hit){
                    bool collide=false;
                    int index=GetGridIndex(mousehit.point,bbox,gridDimensions);
                    int placeIndex=-1;
                    for(int i=0;i<numSteps;i++){
                            Vector3 stepPoint=insideRay.position+insideRay.direction*(mousehit.distance/numSteps)*i;
                            int stepIndex=GetGridIndex(stepPoint,bbox,gridDimensions);
                            hoverVoxel.index=stepIndex;
                            if(occupied_indices[stepIndex]){
                                collide=true;
                                break;
                            }
                            else{
                                placeIndex=stepIndex;
                                
                            }
                    }
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        if(v==VoxelMode::Attach) {
                                int finalIndex = collide ? placeIndex : hoverVoxel.index;
                                if (finalIndex >= 0 &&finalIndex < (int)occupied_indices.size() &&!occupied_indices[finalIndex]){
                                    occupied_indices[finalIndex] = true;
                                    Voxel vox;
                                    vox.index = finalIndex;
                                    vox.color=ColorFromNormalized(curPallete);
                                    voxels.push_back(vox);
                                }
                        }
                        else if(v==VoxelMode::Erase){
                            int finalIndex = collide ? hoverVoxel.index : placeIndex;
                            if (finalIndex >= 0 &&finalIndex < (int)occupied_indices.size() &&occupied_indices[finalIndex]){
                                    occupied_indices[finalIndex] = false;
                                     voxels.erase(
                                    std::remove_if(voxels.begin(), voxels.end(),
                                        [finalIndex](const Voxel& v) {
                                            return v.index == finalIndex;
                                        }),voxels.end());
                            }
                        }
                        else if(v==VoxelMode::Paint){
                            int finalIndex = collide ? hoverVoxel.index : placeIndex;
                            if (finalIndex >= 0 &&finalIndex < (int)occupied_indices.size() &&occupied_indices[finalIndex]){
                                    for(Voxel&v:voxels){
                                    if(v.index==finalIndex){
                                        v.color=ColorFromNormalized(curPallete);
                                    }
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
Voxel GetHoverVoxel(std::vector<bool>&occupied_indices,BoundingBox &bbox,VoxCam cam,Vector3 gridDimensions){
        Ray mouseRay=GetScreenToWorldRay(GetMousePosition(),cam.camera);
        RayCollision mousehit=GetRayCollisionBox(mouseRay,bbox);  
        Voxel hoverVoxel;
        hoverVoxel.color=RAYWHITE; 
        //Form A ray using the forward direction of the cam
        //If you hit form a cube there
        if(mousehit.hit){
            Vector3 forward=Vector3Normalize(Vector3Subtract(cam.camera.target, cam.camera.position));
            float dot=Vector3DotProduct(forward,mousehit.normal);
            //if the dot product is negative it means you face roughly in the opposite direction 
            // maybe you want to place the voxels inside the cube 
            if(dot<0.0f){
                Ray insideRay;
                insideRay.position=mousehit.point+(mouseRay.direction*0.01);
                insideRay.direction=mouseRay.direction;
                mousehit=GetRayCollisionBox(insideRay,bbox);
                if(mousehit.hit){
                    bool collide=false;
                    int index=GetGridIndex(mousehit.point,bbox,gridDimensions);
                    int placeIndex=-1;
                    for(int i=0;i<numSteps;i++){
                        Vector3 stepPoint=insideRay.position+insideRay.direction*(mousehit.distance/numSteps)*i;
                        int stepIndex=GetGridIndex(stepPoint,bbox,gridDimensions);
                        hoverVoxel.index=stepIndex;
                        if(occupied_indices[stepIndex]){
                            collide=true;
                            break;
                        }
                    }
                    
                }
            }
           
        }
        return hoverVoxel;
    }