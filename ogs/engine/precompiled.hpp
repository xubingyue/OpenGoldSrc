#pragma once

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#include "rehlds/osconfig.h"

#include "maintypes.h"

#include "common/commontypes.h"
#include "common/mathlib.h"
#include "common/const.h"

#include "common/usercmd.h"
#include "engine/eiface.h"
#include "common/com_model.h"

#include "rehlds/modelgen.h"
#include "rehlds/spritegn.h"

#include "engine/studio.h"
#include "engine/shake.h"
#include "engine/customentity.h"
#include "engine/custom.h"
#include "engine/edict.h"
#include "engine/progs.h"
#include "engine/progdefs.h"

#include "rehlds/bspfile.h"
#include "common/con_nprint.h"
#include "common/cvardef.h"
#include "common/kbutton.h"
#include "common/qfont.h"
#include "common/event_flags.h"
#include "common/netadr.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/entity_state.h"
#include "common/entity_types.h"
#include "common/r_efx.h"
#include "common/r_studioint.h"
#include "common/triangleapi.h"
#include "common/particledef.h"
#include "common/dlight.h"
#include "common/enums.h"
#include "common/netadr.h"
#include "common/net_api.h"
#include "common/beamdef.h"
#include "common/kbutton.h"
#include "common/screenfade.h"
#include "common/cl_entity.h"
#include "common/ref_params.h"
#include "common/crc.h"
#include "common/weaponinfo.h"
#include "common/demo_api.h"
#include "common/ivoicetweak.h"

#include "public/keydefs.h"

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_movevars.h"

//#include "version/appversion.h"

#include "public/archtypes.h"
#include "asmlib.h"
#include "math/sse_mathfun.h"
#include "common/mathlib.h"

//#include "sys_shared.h"
//#include "crc32c.h"
#include "rehlds/static_map.h"

//#include "ed_strpool.h"

#include "memory.h"
#include "engine_pch.h"
#include "platform.h"
//#include "RehldsRuntimeConfig.h"
//#include "rehlds_debug.h"

#ifdef HOOK_ENGINE
#include "hooker.h"
#endif

// valve libs stuff
#include "tier0/platform.h"
#include "dbg/dbg.h"

#include "public/interface.h"
#include "iregistry.h"

#include "tier1/utlbuffer.h"
#include "tier1/utlrbtree.h"

#include "iosutil.h"

// testsuite
/*
#include "testsuite.h"
#include "funccalls.h"
#include "recorder.h"
#include "player.h"
#include "anonymizer.h"
*/

//#include "bzip2/bzlib.h"
#include "system/igame.h"

#include "system/iengine.h"
#include "hookchains_impl.h"
#include "rehlds/rehlds_interfaces.h"
#include "rehlds_interfaces_impl.h"
#include "rehlds/rehlds_api.h"
#include "rehlds_api_impl.h"
//#include "FlightRecorderImpl.h"
//#include "flight_recorder.h"
//#include "rehlds_security.h"

#include "dlls/cdll_dll.h"