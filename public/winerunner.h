//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Run Windows applications using Wine. On native system this will use
// CWindowsRunner.
//===========================================================================//

#ifndef WINE_RUNNER_H
#define WINE_RUNNER_H

#include "runner.h"
#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"

#define WINE_RUNNER_INTERFACE_NAME "WineRunner001"

abstract_class IWineRunner: public IRunner
{
public:
	virtual int Run( CUtlString szName, CUtlVector<CUtlString>& args ) = 0;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args ) = 0;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment ) = 0;
	virtual int Wait( void ) = 0;
};


extern IWineRunner *winerunner;

#endif
