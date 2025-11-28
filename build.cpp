#include "c.h"
#include "ld.h"
#include "helper.h"
#include "tier0/platform.h"
#include "signal.h"

CUtlVector<CUtlString> g_CompiledFiles = {
	
	"tier0/lib.cpp",
	"tier0/mem.cpp",
	"tier0/platform.cpp",
	"tier1/interface.cpp",
	"tier1/utlbuffer.cpp",
	"tier1/utlstring.cpp",
	"tier1/utlvector.cpp",
	"tier1/utlmap.cpp",
	"tier1/commandline.cpp",
	"tier2/filesystem.cpp",
	"tier2/filesystem_libc.cpp",
	"tier2/fileformats/ini.cpp",

	"main.cpp",
	"library/runner.cpp",
	"library/helper.cpp",
	"library/target.cpp",
	
	"library/winerunner.cpp",
	
	"library/c.cpp",
	"library/ld.cpp",	

	"library/android/apktool.cpp",
	
	"library/clang/c.cpp",
	"library/clang/ld.cpp",

	"library/windows/c.cpp",
	"library/windows/ld.cpp",
	
};

CUtlVector<CUtlString> g_IncludeDirectories = {
	"public",
	"../public",
};


DECLARE_BUILD_STAGE(fpc)
{
	if (linker->IsLibraryExists("clang"))
		g_CompiledFiles.AppendTail("library/clang/c_libclang.cpp");
	else
		V_printf("Warning: to support included files libclang must be installed.");
	CProject_t compileProject = {};
	LinkProject_t ldProject = {};

	compileProject.m_szName = "fpc";
	compileProject.files = g_CompiledFiles;
	compileProject.includeDirectories = g_IncludeDirectories;
	ldProject = ccompiler->Compile(&compileProject);
	if (linker->IsLibraryExists("clang"))
		ldProject.libraries.AppendTail("clang");

	CUtlString outputProject = linker->Link(&ldProject);

	filesystem2->MakeDirectory("../build/tools");
	filesystem2->CopyFile("fpc_temp", outputProject);

	return 0;
};
