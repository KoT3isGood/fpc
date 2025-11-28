#include "helper.h"
#include "c.h"
#include "ld.h"
#include "tier1/utlstring.h"

DECLARE_BUILD_STAGE(android_build)
{
	AndroidManifest_t manifest = {};
	manifest.SetMinSDKVersion(21);
	manifest.SetTargetSDKVersion(35);
	manifest.SetPackageName("FPC Testing facility");
	manifest.SetPackageID("com.example.testfpc");
	CUtlString szManifestDir = manifest.BuildManifest();

	CProject_t compileProject = {};
	compileProject.m_szName = "android_app";
	compileProject.m_androidmanifest = manifest;
	compileProject.bFPIC = true;
	compileProject.files = {
		"main.c",
	};
	LinkProject_t ldProject = ccompiler->Compile(&compileProject);
	ldProject.libraries = {
		"android",
		"log",
	};
	CUtlString szOutputDir = linker->Link(&ldProject);
	filesystem2->MakeDirectory(CUtlString("%s/lib/x86_64",szManifestDir.GetString()));
	filesystem2->CopyFile(CUtlString("%s/lib/x86_64/libnative-app.so",szManifestDir.GetString()), szOutputDir);
	
	APKTool()->BuildPackage(manifest, szManifestDir);

	return 0;
}

