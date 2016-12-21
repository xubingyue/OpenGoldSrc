#include "precompiled.h"
#include "system/engineapi.h"

static CEngineAPI g_CEngineAPI;

/*
IBaseInterface *CreateCEngineAPI(void)
{
	return &g_CEngineAPI;
};

InterfaceReg g_CreateCEngineAPI = InterfaceReg(CreateCEngineAPI, "VENGINE_LAUNCHER_API_VERSION002");
*/

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineAPI, IEngineAPI, VENGINE_LAUNCHER_API_VERSION, g_CEngineAPI);

int CEngineAPI::Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	if(!gpEngine->Load(false, basedir, cmdline))
		return 1;
	
	while(!gpEngine->GetQuitting()) // ?
		gpEngine->Frame();
	
	gpEngine->Unload();
	return 0;
};