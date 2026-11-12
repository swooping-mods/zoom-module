/* Provider-local Unity boundary, shared native item framework integration. */
#include "unity.h"
#include "../../runtime/native/local_actor.h"
static DWORD zoom_thread;
static unsigned zoom_started;
static int zoom_admit(PTR walker);
/* Keep cleanup retries reachable after the adapter has cleared its world. */
static int zoom_install_hooks(void);
#define tt_update zoom_adapter_update
#define install_hooks zoom_install_hooks
#define TOOL_CONTEXT_ADMIT zoom_admit
#include "../equipment-framework/sdk/tool_runtime.h"
#undef TOOL_CONTEXT_ADMIT
#undef install_hooks
#undef tt_update
