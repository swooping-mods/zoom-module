/* Provider-owned optics. Keep this exact code executable in the host fixture. */
static struct {PTR get_fov,set_fov;SURoot camera;float original,range,timer;int captured,zooming,returning;} zoom;
static struct {SURoot world,walker;} zoom_targets;
typedef struct {PTR world,walker,camera;} ZoomCapture;
static const MoonCameraApiV1 *zoom_api;
static MoonCameraSession zoom_session;

static int zoom_read_fov(PTR camera,float *value){
    int ok=1;PTR boxed=call(zoom.get_fov,camera,0,&ok);
    float *number=ok&&boxed?unbox(boxed):0;
    if(!number)return 0;
    /* Positive bounded comparisons also reject NaN and infinities. A zoomed
     * 10-degree native FOV can legitimately reach 10/3 degrees. */
    float fov=*number;if(!(fov>=1.f&&fov<=179.f))return 0;
    *value=fov;return 1;
}
static void zoom_drop_targets(void){su_drop(&zoom_targets.world);su_drop(&zoom_targets.walker);}
static int zoom_capture(void *context){
    ZoomCapture *c=context;
    if(!zoom_read_fov(c->camera,&zoom.original)||!(zoom.original>=10.f&&zoom.original<=150.f))return 0;
    zoom.camera=su_hold(c->camera);zoom_targets.world=su_hold(c->world);zoom_targets.walker=su_hold(c->walker);
    if(!zoom.camera||!zoom_targets.world||!zoom_targets.walker){su_drop(&zoom.camera);zoom_drop_targets();return 0;}
    zoom.captured=1;return 1;
}
#include "camera_restore.h"
static void zoom_finish(void){
    zoom_restore();
    if(!zoom_session.token){zoom_drop_targets();zoom.range=zoom.timer=0;}
}
static void feature_drop(void){zoom_finish();}
static void zoom_hint(int active){
    char binding[MOON_INPUT_LABEL_CAPACITY]="?",b[256];
    if(!moon_input_label("zoom-module.zoom","Z",binding,sizeof(binding)))cat(binding,"?");
    char *p;
    if(!active){p=cat(b,"Hold ");p=cat(p,binding);cat(p,": 3x zoom  |  release: restore view");}
    else{
        p=cat(b,"[  +  ]  3x OPTICAL ZOOM\nRange: ");
        if(zoom.range>0&&zoom.range<=500){p=num(p,(U64)zoom.range);p=cat(p," m");}else p=cat(p,">500 m");
        p=cat(p,"  |  release ");p=cat(p,binding);cat(p," to restore");
    }
    tt_shop_text(b);
}
static void feature_tick(PTR walker,PTR world,int allow){
    /* Sample even during failed cleanup, so closing a gate consumes edges. */
    int active=moon_input_action("zoom-module.zoom","Z",allow&&tt.owned).held;
    if(zoom.returning){zoom_finish();if(zoom_session.token)return;}
    PTR camera=su_get(tt.camera);
    if(zoom.captured&&(su_get(zoom.camera)!=camera||su_get(zoom_targets.world)!=world||su_get(zoom_targets.walker)!=walker)){
        zoom_finish();if(zoom_session.token)return;
    }
    if(!active){zoom_finish();zoom_hint(0);return;}
    if(!zoom.captured){
        ZoomCapture capture={world,walker,camera};
        if(!moon_camera_begin(&zoom_session,zoom_api,"zoom-module",world,walker,camera,MOON_CAMERA_FOV,zoom_capture,&capture)){
            /* A refused release after read-only capture failure still owns
             * the lease, even though there is no camera snapshot to restore. */
            if(zoom_session.token)zoom.returning=1;
            return;
        }
        tt_event("zoom-start");
    }
    float current;
    if(!zoom_read_fov(camera,&current)){zoom_finish();return;}
    float dt=tt.dt;if(!(dt>=0&&dt<=.1f))dt=0;
    float target=zoom.original/3.f,blend=dt*14.f;if(blend>1)blend=1;
    float next=current+(target-current)*blend;int ok=1;void *args[]={&next};call(zoom.set_fov,camera,args,&ok);
    if(!ok){zoom_finish();return;}
    zoom.zooming=1;zoom.timer+=dt;
    if(zoom.timer>=.12f){
        zoom.timer=0;PTR tr=tt_transform(camera);SPVec3 origin=su_vector(su.tr_get_position,tr),forward=su_vector(su.tr_forward,tr);
        unsigned char hit[64];zoom.range=0;if(tt_ray(origin,forward,500,hit))su_copy(&zoom.range,hit+su.hit_distance,4);
    }
    zoom_hint(1);
}
