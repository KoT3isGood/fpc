//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Linker interface.
//===========================================================================//

#ifndef LD_H
#define LD_H


#include "runner.h"
#include "helper.h"
#include "obj.h"
#include "tier0/platform.h"
#include "tier1/utlstring.h"

enum ELinkType
{
	ELINK_EXECUTABLE,
	ELINK_DYNAMIC_LIBRARY,
	ELINK_STATIC_LIBRARY,
	
	// drivers
	ELINK_KERNEL_DRIVER
};

enum EWindowsSubsystem
{
	WINDOWS_SUBSYSTEM_NATIVE,
};

//----------------------------------------------------------------------------
// C project settings used in compilation
// Example usage:
//	CProject_t compileProject = {};
//	LinkProject_t ldProject = {};
//	CUtlString szOutputFile;
//
//	compileProject.m_szName = "your_project_name";
//	compileProject.files = g_CompiledFiles;
//	compileProject.includeDirectories = g_IncludeDirectories;
//
//	ldProject = ccompiler->Compile(&compileProject);
//	ldProject.linkType = ELINK_EXECUTABLE
//	szOutputFile = linker->Link(&ldProject);
//
//----------------------------------------------------------------------------
struct LinkProject_t: public CPUProject_t
{
public:
	void AddObject( Object_t& object );

	// output file
	ELinkType linkType;

	// objects, they could be libraries and compiled files
	CUtlVector<Object_t> objects = {};

	// system libraries
	CUtlVector<CUtlString> libraries ={};

	// directories for libraries
	CUtlVector<CUtlString> libraryDirectories = {};

	// not used
	CUtlVector<CUtlString> libraryObjects = {};

	// Apple framework directories
	CUtlVector<CUtlString> frameworkDirectories = {};

	const char *szEntry = "";

	// Disables C standart library
	bool bNoStdLib;
	
	// Apple frameworks
	CUtlVector<CUtlString> frameworks = {};

	// Android manifest
	AndroidManifest_t m_androidmanifest;

	// Windows subsystem
	EWindowsSubsystem m_eWindowsSubsystem;
};

// Basic interface name
#define LINKER_INTERFACE_NAME "Linker001"
#define CLANG_LINKER_INTERFACE_NAME "Clang" LINKER_INTERFACE_NAME
#define MSVC_LINKER_INTERFACE_NAME "MSVC" LINKER_INTERFACE_NAME

abstract_class ILinker
{
public:

	// Links project
	virtual CUtlString Link( LinkProject_t *pProject ) = 0;

	virtual bool IsLibraryExists( CUtlString szName ) = 0;
};

extern ILinker *linker;

#endif
