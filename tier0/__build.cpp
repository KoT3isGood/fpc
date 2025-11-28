#include "helper.h"
#include "c.h"
#include "ld.h"
#include "tier1/utlstring.h"
#include "tier1/commandline.h"

CUtlVector<CUtlString> tier0_CompiledFiles = {
	"tier0/lib.cpp",
	"tier0/mem.cpp",
	"tier0/platform.cpp",
	"tier0/network.cpp",
};

DECLARE_BUILD_STAGE(tier0)
{
	CProject_t compileProject = {};
	LinkProject_t ldProject = {};

	compileProject.m_szName = "tier0";
	compileProject.files = tier0_CompiledFiles;
	compileProject.includeDirectories = all_IncludeDirectories;
	compileProject.bFPIC = true;
	ldProject = ccompiler->Compile(&compileProject);
	if (bStaticBuild)
		ldProject.linkType = ELINK_STATIC_LIBRARY;
	else
		ldProject.linkType = ELINK_DYNAMIC_LIBRARY;

	CUtlString outputProject = linker->Link(&ldProject);
	
	if (!bStaticBuild)
	{
		filesystem2->MakeDirectory(CUtlString("%s/bin",szOutputDir.GetString()));
		filesystem2->CopyFile(CUtlString("%s/bin", szOutputDir.GetString()), outputProject);
	} else {
		tier0_lib = outputProject;
	}

	return 0;
};
