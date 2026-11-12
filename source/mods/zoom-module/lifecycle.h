/* Remote/null/failed local lookup must not consume the shared frame latch,
 * inspect native context or establish the provider's Unity thread. Also guard
 * ownership/shop entry points before their shared tt_context does native work. */
static int zoom_admit(PTR walker){
    if(!__atomic_load_n(&zoom_started,__ATOMIC_ACQUIRE))return 0;
    DWORD thread=GetCurrentThreadId(),observed=__atomic_load_n(&zoom_thread,__ATOMIC_ACQUIRE);
    if(observed&&observed!=thread)return 0;
    if(!walker||!moon_local_actor_admit(walker))return 0;
    DWORD expected=0;
    return observed==thread||__atomic_compare_exchange_n(&zoom_thread,&expected,thread,0,__ATOMIC_RELEASE,__ATOMIC_RELAXED)||expected==thread;
}
static int zoom_update(int id,Context *context){
    if(id||!context||!zoom_admit((PTR)context->rcx))return 0;
    if(zoom.returning){
        (void)moon_input_action("zoom-module.zoom","Z",0);
        zoom_finish();if(zoom_session.token)return 0;
    }
    return zoom_adapter_update(id,context);
}
static void zoom_menu_cleanup(void){
    if(!__atomic_load_n(&zoom_started,__ATOMIC_ACQUIRE))return;
    DWORD thread=__atomic_load_n(&zoom_thread,__ATOMIC_ACQUIRE);
    if(!thread||thread!=GetCurrentThreadId()||!zoom_session.token)return;
    if(!zoom.returning&&zoom.captured){
        int ok=1;PTR world=su_get(zoom_targets.world);
        /* Unknown is not a death observation. Keep every root and lease. */
        if(!world)return;
        int alive=moon_camera_target_alive(su.obj_alive,world,&ok);if(!ok)return;
        if(alive){
            PTR boxed=call(tt.frame,0,0,&ok);int *number=ok&&boxed?unbox(boxed):0;
            int frame=number?*number:-1;
            /* Menu may run just before this frame's walker. One-frame grace
             * avoids interrupting ordinary held zoom; paused walkers eventually
             * restore even when their last callback never called feature_drop. */
            if(number&&frame>=0&&tt.last_frame>=0&&frame>=tt.last_frame&&(unsigned)frame-(unsigned)tt.last_frame<=1u)return;
        }
    }
    (void)moon_input_action("zoom-module.zoom","Z",0);
    tt_teardown(); /* feature_drop retries; its own roots outlive adapter roots. */
}
static int zoom_install_hooks(void){before_hook=zoom_update;return install_hooks();}
