/* Restore the captured targets before releasing the shared camera lease. */
static int zoom_restore_state(void *context){
 (void)context;PTR camera=su_get(zoom.camera);int ok=1;
 if(!moon_camera_target_alive(su.obj_alive,camera,&ok))return ok;
 void *args[]={&zoom.original};call(zoom.set_fov,camera,args,&ok);return ok;
}
static void zoom_restore(void){zoom.returning=1;if(!moon_camera_finish(&zoom_session,zoom_restore_state,0))return;su_drop(&zoom.camera);zoom.captured=zoom.zooming=zoom.returning=0;}
