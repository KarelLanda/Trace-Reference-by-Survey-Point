#include "APIEnvir.h"
#include "ACAPinc.h"
#include "TraceShiftBySurveyPoint.hpp"

// =============================================================================
//
// Main functions
//
// =============================================================================

API_AddonType CheckEnvironment(API_EnvirParams* envir)
{
	RSGetIndString(&envir->addOnInfo.name, 32000, 1, ACAPI_GetOwnResModule());
	RSGetIndString(&envir->addOnInfo.description, 32000, 2, ACAPI_GetOwnResModule());

	return APIAddon_Normal;
}

GSErrCode RegisterInterface(void)
{
	GSErrCode err = NoError;
	err = ACAPI_MenuItem_RegisterMenu(32500, 32501, MenuCode_UserDef, MenuFlag_Default);
	return err;
}

GSErrCode Initialize(void)
{
	GSErrCode err = ACAPI_MenuItem_InstallMenuHandler(32500, MenuCommandHandler);
	if (err != NoError)
		DBPrintf("PruhledoveZobrazeniDleGB:: Initialize() ACAPI_MenuItem_InstallMenuHandler failed\n");

	return err;
}

GSErrCode FreeData(void)
{
	return NoError;
}
