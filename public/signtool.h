//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Automatic signature generator for iOS, Android and other devices,
// which require signed executables, files etc.
//===========================================================================//

#ifndef SIGN_TOOL_H
#define SIGN_TOOL_H

#include "tier0/platform.h"
#include "tier1/utlstring.h"

#define APPLE_SIGN_TOOL_INTERFACE_NAME "SignToolApple001"
#define ANDROID_SIGN_TOOL_INTERFACE_NAME "SignToolAndroid001"

abstract_class ISignTool
{
public:
	virtual void SetSignPassword( CUtlString szPassword ) = 0;
	virtual void SignFile( CUtlString szFile ) = 0;
	virtual void SignDirectory( CUtlString szDirectory ) = 0;
};

extern ISignTool *signtool_android;
extern ISignTool *signtool_apple;

#endif
