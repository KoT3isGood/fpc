#ifndef C_LIBCLANG_H
#define C_LIBCLANG_H

#include "tier0/platform.h"
#include "tier1/utlstring.h"

#define CLANG_BACKEND_INTERFACE_NAME "ClangBackend001"

abstract_class IClangBackend
{
public:
	virtual bool AreFileDependenciesUpdated( CUtlString szFile, CUtlString szOutputFile, CUtlString szHashFile, CUtlVector<CUtlString> arguments ) = 0;	
};

extern IClangBackend *clangbackend;

#endif
