//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Executable runner. It is mainly used to run compilers and linkers,
// but can be used to run anything with given executable name
//===========================================================================//

#ifndef RUNNER_H
#define RUNNER_H

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"

#define RUNNER_INTERFACE_NAME "Runner001"

abstract_class IRunner
{
public:
	virtual int Run( CUtlString szName, CUtlVector<CUtlString>& args ) = 0;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args ) = 0;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment ) = 0;
	virtual int Wait( void ) = 0;
};


extern IRunner *runner;

#endif
