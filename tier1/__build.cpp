#include "helper.h"
#include "c.h"
#include "ld.h"
#include "tier1/utlstring.h"
#include "tier1/commandline.h"

CUtlVector<CUtlString> tier1_CompiledFiles = {
	"tier1/interface.cpp",
	"tier1/commandline.cpp",
	"tier1/utlbuffer.cpp",
	"tier1/utlmap.cpp",
	"tier1/utlstring.cpp",
	"tier1/utlvector.cpp",
};
CUtlString tier1_lib;

DECLARE_BUILD_STAGE(tier1)
{
	CProject_t compileProject = {};
	LinkProject_t ldProject = {};

	compileProject.m_szName = "tier1";
	compileProject.files = tier1_CompiledFiles;
	compileProject.includeDirectories = all_IncludeDirectories;
	compileProject.bFPIC = true;
	ldProject = ccompiler->Compile(&compileProject);
	ldProject.linkType = ELINK_STATIC_LIBRARY;

	CUtlString outputProject = linker->Link(&ldProject);
	tier1_lib = outputProject;

	return 0;
};
