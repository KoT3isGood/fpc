//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Helper functions for compilers, filesystem2 and build stages.
//===========================================================================//

#ifndef HELPER_H
#define HELPER_H

#include "apktool.h"
#include "tier1/utlstring.h"
#include "target.h"
#include "tier2/fileformats/ini.h"

#define FPC_TEMPORAL_DIRNAME ".fpc"

//-----------------------------------------------------------------------------
// A base for all projects
//-----------------------------------------------------------------------------
struct BaseProject_t
{
public:
	CUtlString m_szName;
	// Creates a hash for the project
	unsigned int GenerateProjectHash( void );
};

//-----------------------------------------------------------------------------
// A base for cpu projects
//-----------------------------------------------------------------------------
struct CPUProject_t : public BaseProject_t
{
public:
	Target_t m_target = Target_t::DefaultTarget();

};

//-----------------------------------------------------------------------------
// A base for shader projects
//-----------------------------------------------------------------------------
struct ShaderProject_t : public BaseProject_t
{
public:
	EShaderTarget m_eTarget;
};

//-----------------------------------------------------------------------------
// File system manager.
//-----------------------------------------------------------------------------
#define FILE_SYSTEM_2_INTERFACE_NAME "FileSystem2_001"

abstract_class IFileSystem2
{
public:
	// Returns a directory of fpc executable
	virtual char *OwnDirectory() = 0;

	// Returns directory of build.cpp
	virtual char *BuildDirectory() = 0;

	// Creates new directory at path
	virtual void MakeDirectory( const char *psz ) = 0;
	
	// UNIX-style file copy
	virtual void CopyFile( const char *szDestination, const char *szOrigin ) = 0;
	
	// UNIX-style recursive directory copy
	virtual void CopyDirectory( const char *szDestination, const char *szOrigin ) = 0;

	// Compares timestamps of 2 files
	virtual bool ShouldRecompile( const char *szSource, const char *szOutput ) = 0;
};

extern IFileSystem2 *filesystem2;
char *GetWindowsPath( const char *szPath );
char *GetPOSIXPath( const char *szPath );


//-----------------------------------------------------------------------------
// Build stage.
//-----------------------------------------------------------------------------
class CBuildStage
{
public:
	CBuildStage( CUtlString sz, int(*pMainFn)() );
	CUtlString m_sz;
	int(*m_pMainFn)();
};

//-----------------------------------------------------------------------------
// Declares new build stage.
// example:
//	DECLARE_BUILD_STAGE(your_build_stage_name)
//	{
//		return 0;
//	}
//-----------------------------------------------------------------------------
#define DECLARE_BUILD_STAGE(sz) \
int __build_stage_##sz(); \
CBuildStage __##sz##_build_stage(#sz, __build_stage_##sz); \
int __build_stage_##sz()

// Returns all available build stages
// Used internally
CUtlVector<CBuildStage*>& BuildStages();

extern IINIFile *g_pConfig;

#endif
