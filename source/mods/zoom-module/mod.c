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
#include "optics.h"
#include "lifecycle.h"
__declspec(dllexport) int MoonLab_ModStart(PTR domain,PTR game){if(__atomic_load_n(&zoom_started,__ATOMIC_ACQUIRE))return 1;if(!tt_start(domain,game)||!moon_local_actor_bind())return 0;zoom_api=moon_camera_connect();if(!zoom_api)return 0;zoom.get_fov=su_method("UnityEngine","Camera","get_fieldOfView","");zoom.set_fov=su_method("UnityEngine","Camera","set_fieldOfView","System.Single");const MoonEquipmentMenuCleanupV1 *cleanup=me_menu_cleanup_connect();if(!zoom.get_fov||!zoom.set_fov||!cleanup||!cleanup->register_cleanup("zoom-module",zoom_menu_cleanup)||!tt_install(game))return 0;__atomic_store_n(&zoom_started,1,__ATOMIC_RELEASE);return 1;}
