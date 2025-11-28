#include "ld.h"
#include "helper.h"
#include "libgen.h"
#include "target.h"
#include "tier0/platform.h"
#include "tier1/interface.h"
#include "tier1/utlstring.h"
#include "tier2/fileformats/ini.h"
#include "winerunner.h"

class CMSVCLinker : public ILinker
{
public:
	virtual CUtlString Link( LinkProject_t *pProject ) override;
	virtual bool IsLibraryExists( CUtlString szName ) override;
};

EXPOSE_INTERFACE(CMSVCLinker, ILinker, MSVC_LINKER_INTERFACE_NAME);

CUtlString CMSVCLinker::Link( LinkProject_t *pProject )
{
	if (pProject->m_szName == 0)
	{
		Plat_FatalErrorFunc("m_szName must be present\n");
	}

	if (pProject->m_target.kernel != TARGET_KERNEL_WINDOWS_MSVC)
	{
		Plat_FatalErrorFunc("target must be TARGET_KERNEL_WINDOWS_MSVC\n");
	}

	// Find a name for the file
	CUtlString szFileName;
	unsigned int hash = pProject->GenerateProjectHash();
	switch(pProject->linkType)
	{
	case ELINK_EXECUTABLE:
		szFileName = CUtlString("%s.exe", pProject->m_szName.GetString());	
	case ELINK_STATIC_LIBRARY:
		szFileName = CUtlString("lib%s.a", pProject->m_szName.GetString());
		break;
	case ELINK_DYNAMIC_LIBRARY:
		szFileName = CUtlString("%s.dll", pProject->m_szName.GetString());
		break;
	case ELINK_KERNEL_DRIVER:
		szFileName = CUtlString("%s.sys", pProject->m_szName.GetString());
		break;
	}

	CUtlString szTarget = pProject->m_target.GetTriplet();
	CUtlString szOutputFile = CUtlString("%s/%s/ld/%u_%s/%s",FPC_TEMPORAL_DIRNAME, szTarget.GetString(), hash, pProject->m_szName.GetString(), szFileName.GetString());
	CUtlString szOutputDir = szOutputFile;
	szOutputDir = dirname(szOutputDir);
	filesystem2->MakeDirectory(szOutputDir);

	if (!g_pConfig)
		Plat_FatalErrorFunc(".fpccfg was not found\n");
	IINISection *pSection = g_pConfig->GetSection("MSVC_LINKER_INTERFACE_NAME");
	if (!pSection)
		Plat_FatalErrorFunc("MSVC_LINKER_INTERFACE_NAME was not found in .fpccfg\n");
	CUtlString szExePath = pSection->GetStringValue("exe");
	if (!pSection)
		Plat_FatalErrorFunc("exe was not found in MSVC_LINKER_INTERFACE_NAME\n");

	if (pProject->linkType == ELINK_STATIC_LIBRARY)
	{
		V_printf("  AR       %s\n", pProject->m_szName.GetString());
		bool shouldRecompile = false;
		CUtlVector<CUtlString> args;
		for (auto object: pProject->objects)
		{
			if (filesystem2->ShouldRecompile(object.m_szObjectFile,szOutputFile))
			{
				shouldRecompile = true;
				break;
			}
		}
		if (!shouldRecompile)
			goto compiled;
		args = {
			"rcs",
			szOutputFile
		};
		for (auto object: pProject->objects)
			args.AppendTail(object.m_szObjectFile);
		runner->Run("ar", args);
		runner->Wait();

	} else {
		V_printf("  LINK     %s\n", pProject->m_szName.GetString());
		bool shouldRecompile = false;
		CUtlVector<CUtlString> args;

		// Check if any of the files have changed
		for (auto object: pProject->objects)
		{
			if (filesystem2->ShouldRecompile(object.m_szObjectFile,szOutputFile))
			{
				shouldRecompile = true;
				break;
			}
		}
		if (!shouldRecompile)
			goto compiled;


		CUtlString szTarget = pProject->m_target.GetTriplet();		
		CUtlString szCompiledTarget = szTarget; 
		if (pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			szCompiledTarget = CUtlString("%s%u", szTarget.GetString(), pProject->m_androidmanifest.m_nTargetVersion);
		}

		args = {
			"/nologo"
		};

		const char *szWindowsPath = GetWindowsPath(szOutputFile);
		args.AppendTail(CUtlString("/out:%s", szWindowsPath));
		V_free((void*)szWindowsPath);

		if (pProject->linkType == ELINK_KERNEL_DRIVER)
		{
			args.AppendTail("/driver");
			args.AppendTail(CUtlString("/entry:%s", pProject->szEntry));
		}
		switch (pProject->m_eWindowsSubsystem)
		{
		case WINDOWS_SUBSYSTEM_NATIVE:
			args.AppendTail("/subsystem:native");
		default:
			break;
		}

		// Disable stdlib
		if (pProject->bNoStdLib)
		{
		}

		for (auto object: pProject->objects)
			args.AppendTail(object.m_szObjectFile);

		for (auto lib: pProject->libraries)
		{
			/*
			args.AppendTail("-l");
			args.AppendTail(lib);
			*/
		}

		winerunner->Run(szExePath, args);
		winerunner->Wait();
	}
compiled:
	return szOutputFile;
};

bool CMSVCLinker::IsLibraryExists( CUtlString szName )
{
	szName = CUtlString("%s.dll", szName.GetString());
	void *pLib = Plat_LoadLibrary(szName.GetString());
	if (!pLib)
		return false;
	Plat_UnloadLibrary(pLib);
	return true;
}
