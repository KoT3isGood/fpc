#include "ld.h"
#include "helper.h"
#include "libgen.h"
#include "target.h"
#include "tier0/platform.h"
#include "tier1/interface.h"
#include "tier1/utlstring.h"

class CClangLinker : public ILinker
{
public:
	virtual CUtlString Link( LinkProject_t *pProject ) override;
	virtual bool IsLibraryExists( CUtlString szName ) override;
};

EXPOSE_INTERFACE(CClangLinker, ILinker, CLANG_LINKER_INTERFACE_NAME);

CUtlString CClangLinker::Link( LinkProject_t *pProject )
{
	if (pProject->m_szName == 0)
	{
		Plat_FatalErrorFunc("m_szName must be present\n");
	}

	// Find a name for the file
	CUtlString szFileName;
	unsigned int hash = pProject->GenerateProjectHash();
	switch(pProject->linkType)
	{
	case ELINK_EXECUTABLE:
		if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS)
			szFileName = CUtlString("%s.exe", pProject->m_szName.GetString());	
		else if (pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
			szFileName = CUtlString("lib%s.so", pProject->m_szName.GetString());
		else
			szFileName = CUtlString("%s", pProject->m_szName.GetString());
		break;
	case ELINK_STATIC_LIBRARY:
		szFileName = CUtlString("lib%s.a", pProject->m_szName.GetString());
		break;
	case ELINK_DYNAMIC_LIBRARY:
		if (pProject->m_target.kernel == TARGET_KERNEL_DARWIN)
			szFileName = CUtlString("lib%s.dylib", pProject->m_szName.GetString());
		if (pProject->m_target.kernel == TARGET_KERNEL_LINUX)
			szFileName = CUtlString("lib%s.so", pProject->m_szName.GetString());
		if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS)
			szFileName = CUtlString("%s.dll", pProject->m_szName.GetString());	
		break;
	case ELINK_KERNEL_DRIVER:
		Plat_FatalErrorFunc("TODO: not supported\n");
		break;
	}

	V_printf("Filename: %s\n",szFileName.GetString());

	CUtlString szTarget = pProject->m_target.GetTriplet();
	CUtlString szOutputFile = CUtlString("%s/%s/ld/%u_%s/%s",FPC_TEMPORAL_DIRNAME, szTarget.GetString(), hash, pProject->m_szName.GetString(), szFileName.GetString());
	CUtlString szOutputDir = szOutputFile;
	szOutputDir = dirname(szOutputDir);
	filesystem2->MakeDirectory(szOutputDir);
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
			"-o",
			szOutputFile,
			"-target",
			szCompiledTarget,
		};

		// Disable stdlib
		if (pProject->bNoStdLib)
		{
			args.AppendTail("-nostdlib");
		}

		// Sysroots
		if (pProject->m_target.kernel == TARGET_KERNEL_DARWIN)
		{
			args.AppendTail("-isysroot");
			args.AppendTail("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk");

			// Shouldn't be here
			args.AppendTail("-Wl,-export_dynamic");
			args.AppendTail("-undefined");
			args.AppendTail("dynamic_lookup");
		}
		else if (pProject->m_target.kernel == TARGET_KERNEL_IOS)
		{
			args.AppendTail("-isysroot");
			args.AppendTail("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk");
			// Shouldn't be here ?
			args.AppendTail("-fembed-bitcode");
			args.AppendTail("-Wl,-rpath,@executable_path");
			args.AppendTail("-Wl,-all_load");
			args.AppendTail("-w");
			args.AppendTail("-miphoneos-version-min=18.0 ");	
		} 
		else if (pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			args.AppendTail(CUtlString("--sysroot=%s/sysroot", pProject->m_target.szSysroot));
			
			// Shouldn't be here ?
			args.AppendTail("-static-libstdc++");
		}
		else if (pProject->m_target.szSysroot)
		{
			args.AppendTail(CUtlString("--sysroot=%s", pProject->m_target.szSysroot));
		}

		// Magic for the systems
		if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS)
		{
			args.AppendTail("-fuse-ld=ld");
		}
		if (pProject->m_target.kernel == TARGET_KERNEL_LINUX || pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			args.AppendTail("-rdynamic");
		}

		// Dynamic libraries
		// Android can't run executables
		if (pProject->linkType == ELINK_DYNAMIC_LIBRARY || pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			args.AppendTail("-shared");
		}

		// All the objects
		if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS || pProject->m_target.kernel == TARGET_KERNEL_LINUX || pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
			args.AppendTail("-Wl,--whole-archive");
		for (auto object: pProject->objects)
			args.AppendTail(object.m_szObjectFile);
		if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS || pProject->m_target.kernel == TARGET_KERNEL_LINUX || pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
			args.AppendTail("-Wl,--no-whole-archive");

		// Libraries
		for (auto lib: pProject->libraries)
		{
			args.AppendTail("-l");
			args.AppendTail(lib);
		}

		// Apple frameworks
		for (auto &directory: pProject->frameworkDirectories)
		{
			args.AppendTail("-F");
			args.AppendTail(directory);
		}
		for (auto &framework: pProject->frameworks)
		{
			args.AppendTail("-framework");
			args.AppendTail(framework);
		}

		// Android SDK requires sysroot
		if (pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			if (!pProject->m_target.szSysroot)
				Plat_FatalErrorFunc("szSysroot must be specified for android\n");
			runner->Run(CUtlString("%s/bin/clang++",pProject->m_target.szSysroot), args);

		}
		else 
		{
			runner->Run("clang++", args);
		}
		runner->Wait();	
	}
compiled:
	return szOutputFile;
};

bool CClangLinker::IsLibraryExists( CUtlString szName )
{
	szName = CUtlString("lib%s.so", szName.GetString());
	void *pLib = Plat_LoadLibrary(szName.GetString());
	if (!pLib)
		return false;
	Plat_UnloadLibrary(pLib);
	return true;
}
