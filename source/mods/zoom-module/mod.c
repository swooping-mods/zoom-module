#include "../../runtime/native/gameplay.h"
#define TOOL_ID "zoom-module"
#define TOOL_DESCRIPTION "Optical zoom module. Select it in Equipment; hold Z to zoom and release to restore your view."
#define TOOL_NAME "Zoom Module"
#define TOOL_PRICE 125
#define TOOL_KEY 280
#define TOOL_HOTKEY "PageUp"
#define TOOL_ROW 0
#include "tool_runtime.h"
#include "../../runtime/native/camera.h"
#include "../../runtime/native/input.h"
int _fltused=0;
static struct {PTR get_fov,set_fov;SURoot camera;float original,range,timer;int captured,zooming,returning;} zoom;
static const MoonCameraApiV1 *zoom_api;static MoonCameraSession zoom_session;
static int zoom_capture(void *context){
 PTR camera=context;zoom.original=su_number(zoom.get_fov,camera);
 if(zoom.original<10||zoom.original>150)return 0;
 zoom.camera=su_hold(camera);zoom.captured=zoom.camera!=0;return zoom.captured;
}
#include "camera_restore.h"
static void feature_drop(void){zoom_restore();zoom.range=zoom.timer=0;}
static void feature_tick(PTR walker,PTR world,int allow){
    if(zoom.returning){zoom_restore();if(zoom_session.token)return;}
    int active=moon_input_action("zoom-module.zoom","Z",allow&&tt.owned).held;PTR camera=su_get(tt.camera);
    if(zoom.captured&&su_get(zoom.camera)!=camera){zoom_restore();if(zoom.captured)return;}
    if(!active){zoom_restore();tt_shop_text("Hold Z: 3x zoom  |  release: restore view");return;}
    if(!zoom.captured){if(!moon_camera_begin(&zoom_session,zoom_api,"zoom-module",world,walker,camera,MOON_CAMERA_FOV,zoom_capture,camera))return;tt_event("zoom-start");}
    float current=su_number(zoom.get_fov,camera),target=zoom.original/3.f,blend=tt.dt*14.f;if(blend>1)blend=1;
    su_float(zoom.set_fov,camera,current+(target-current)*blend);zoom.zooming=1;zoom.timer+=tt.dt;
    if(zoom.timer>=.12f){zoom.timer=0;PTR tr=tt_transform(camera);SPVec3 origin=su_vector(su.tr_get_position,tr),forward=su_vector(su.tr_forward,tr);unsigned char hit[64];zoom.range=0;if(tt_ray(origin,forward,500,hit))su_copy(&zoom.range,hit+su.hit_distance,4);}
    char b[256],*p=cat(b,"[  +  ]  3x OPTICAL ZOOM\nRange: ");if(zoom.range>0){p=num(p,(U64)zoom.range);p=cat(p," m");}else p=cat(p,">500 m");cat(p,"  |  release Z to restore");tt_shop_text(b);
}
__declspec(dllexport) int MoonLab_ModStart(PTR domain,PTR game){if(!tt_start(domain,game))return 0;zoom_api=moon_camera_connect();if(!zoom_api)return 0;zoom.get_fov=su_method("UnityEngine","Camera","get_fieldOfView","");zoom.set_fov=su_method("UnityEngine","Camera","set_fieldOfView","System.Single");return zoom.get_fov&&zoom.set_fov&&tt_install(game);}
