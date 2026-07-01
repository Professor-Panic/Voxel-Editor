#include "camera/camera.hpp"

void CameraScroll(VoxCam& cam){
    if(GetMouseWheelMoveV().y>0){
        float dist=Vector3Distance(cam.camera.position,cam.camera.target);
        if(dist>0.0){
            Vector3 dir=Vector3Scale((cam.camera.target-cam.camera.position),1.0/dist);
            cam.camera.position+=dir;
        }
            
    }
    if(GetMouseWheelMoveV().y<0){
        float dist=Vector3Distance(cam.camera.position,cam.camera.target);
        if(dist>0.0){
            Vector3 dir=Vector3Scale((cam.camera.target-cam.camera.position),1.0/dist);
            cam.camera.position-=dir;
        }
    }   
}
void CameraOrbit(VoxCam& cam){
    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            cam.yaw-=GetMouseDelta().x*0.005; 
            cam.pitch+=GetMouseDelta().y*0.005;
            float distance=Vector3Distance(cam.camera.position,cam.camera.target);
            cam.camera.position.x = cam.camera.target.x + distance * cos(cam.pitch) * sin(cam.yaw);
            cam.camera.position.y = cam.camera.target.y + distance * sin(cam.pitch);
            cam.camera.position.z = cam.camera.target.z + distance * cos(cam.pitch) * cos(cam.yaw);
            if(cam.yaw>2*PI||cam.yaw<-2*PI){
                cam.yaw=0;
            }
            if(cam.pitch>=PI/2-5*DEG2RAD){
                cam.pitch=PI/2-5*DEG2RAD;
            }
            if(cam.pitch<=-PI/2+5*DEG2RAD){
                cam.pitch=-PI/2+5*DEG2RAD;
            }

        }
}
void CameraPan(VoxCam& cam){
    if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){
            float dx=GetMouseDelta().x*0.01;
            float dy=GetMouseDelta().y*0.01;
            Vector3 forward = Vector3Normalize(Vector3Subtract(cam.camera.target, cam.camera.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, cam.camera.up));
            Vector3 up = cam.camera.up;

            Vector3 drag =
                Vector3Scale(right, -dx) +
                Vector3Scale(up, dy);

            cam.camera.position += drag;
            cam.camera.target += drag;
        }
}