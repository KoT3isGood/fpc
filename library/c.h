
//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: C, C++, Objective-C and Objective-C++ compiler interface.
//===========================================================================//

#ifndef C_H
#define C_H

#include "tier0/platform.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"
#include "runner.h"
#include "ld.h"
#include "target.h"
#include "helper.h"

struct C_Macro_t
{
	CUtlString szName;
	CUtlString szValue;
};


// Target C version
enum ECVersion
{
	CVERSION_89,
	CVERSION_99 = 0,
	CVERSION_11,
	CVERSION_17,
	CVERSION_23,
	CVERSION_2Y,
};

// Target C++ version
enum ECPPVersion
{
	CPPVERSION_98 = 0,
	CPPVERSION_11 = 1,
	CPPVERSION_14 = 2,
	CPPVERSION_17 = 3,
	CPPVERSION_20 = 4,
	CPPVERSION_23 = 5,
	CPPVERSION_2C = 6,
};

//----------------------------------------------------------------------------
// C project settings used in compilation
// Example usage:
//	CProject_t compileProject = {};
//	LinkProject_t ldProject = {};
//
//	compileProject.m_szName = "your project name";
//	compileProject.files = g_CompiledFiles;
//	compileProject.includeDirectories = g_IncludeDirectories;
//	ldProject = ccompiler->Compile(&compileProject);
//----------------------------------------------------------------------------
struct CProject_t : public CPUProject_t
{
public:
	// Compiled files
	CUtlVector<CUtlString> files = {};

	// Included directories
	CUtlVector<CUtlString> includeDirectories = {};
	
	// Included files
	// They are included on top of the file
	CUtlVector<CUtlString> includeFiles = {};

	// Defined macros
	CUtlVector<C_Macro_t> macros = {};

	// Is compiled as position independent executable
	bool bFPIE = false;
	
	// Is compiled as position independent code
	// Use for shared libraries
	bool bFPIC = false;

	bool bDisableStdInc = false;
	
	// Target C version
	ECVersion cVersion;
	
	// Target C++ version
	ECPPVersion cppVersion;

	// TODO: rework manifests
	// Android manifest
	AndroidManifest_t m_androidmanifest;
};

// Basic interface name
#define C_COMPILER_INTERFACE_NAME "CCompiler001"
#define CLANG_C_COMPILER_INTERFACE_NAME "Clang" C_COMPILER_INTERFACE_NAME
#define GNU_C_COMPILER_INTERFACE_NAME "GNU" C_COMPILER_INTERFACE_NAME
#define MSVC_C_COMPILER_INTERFACE_NAME "MSVC" C_COMPILER_INTERFACE_NAME

class ICCompiler
{
public:

	// Compiles all files into objects, returns linker project,
	// which can be linked into executable or library.
	virtual LinkProject_t Compile( CProject_t *pProject );
	
	virtual void GenerateLinterData() = 0;
protected:
	// Compiler internals
	
	// Returns file name of the
	CUtlString GetOutputObjectName( CProject_t *pProject, unsigned int hash, CUtlString szFileName );

	virtual CUtlVector<CUtlString> BuildCommandLine( CProject_t *pProject, const char *szFileName, const char *szOutputFileName );
	
	// Returns executable which should the OS run
	virtual const char *GetCompilerExecutable( CProject_t *pProject ) = 0;

	// returns object file format, eg .obj or .o
	virtual const char *GetOutputObjectFormat() = 0;

	virtual void IncludeDirectory( CUtlVector<CUtlString> &cmd, const char *szName ) = 0;
	virtual void IncludeFile( CUtlVector<CUtlString> &cmd, const char *szName ) = 0;
	virtual void Macro( CUtlVector<CUtlString> &cmd, const char *szName ) = 0;
	virtual void Macro( CUtlVector<CUtlString> &cmd, const char *szName, const char *szValue ) = 0;
	
	virtual void SetTarget( CUtlVector<CUtlString> &cmd, CProject_t *pProject ) = 0;
	virtual void CompileFile( CUtlVector<CUtlString> &cmd, const char *szName ) = 0;
	virtual void SetOutputFile( CUtlVector<CUtlString> &cmd, const char *szName ) = 0;

	virtual void EnableDebugSymbols( CUtlVector<CUtlString> &cmd ) = 0;
	virtual void EnablePIE( CUtlVector<CUtlString> &cmd ) = 0;
	virtual void EnablePIC( CUtlVector<CUtlString> &cmd ) = 0;
	virtual void DisableStdInc( CUtlVector<CUtlString> &cmd ) = 0;
};

extern ICCompiler *ccompiler;

#endif
