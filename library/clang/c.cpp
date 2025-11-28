#include "c.h"
#include "c_libclang.h"
#include "helper.h"
#include "obj.h"
#include "target.h"
#include "tier0/lib.h"
#include "tier0/platform.h"
#include "tier1/commandline.h"
#include "tier1/interface.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"
#include "libgen.h"
#include "ctype.h"

struct ClangFile_t
{
	CUtlString m_szName;
	CUtlVector<CUtlString> m_szArguments;
};

class CClangCompiler : public ICCompiler
{
public:
	virtual LinkProject_t Compile( CProject_t *pProject ) override;
	
	virtual void GenerateLinterData() override;
protected:

	virtual CUtlVector<CUtlString> BuildCommandLine( CProject_t *pProject, const char *szFileName, const char *szOutputFileName ) override;
	
	// Returns executable which should the OS run
	virtual const char *GetCompilerExecutable( CProject_t *pProject ) override;

	// returns object file format, eg .obj or .o
	virtual const char *GetOutputObjectFormat() override;

	virtual void IncludeDirectory( CUtlVector<CUtlString> &cmd, const char *szName ) override;
	virtual void IncludeFile( CUtlVector<CUtlString> &cmd, const char *szName ) override;
	virtual void Macro( CUtlVector<CUtlString> &cmd, const char *szName ) override;
	virtual void Macro( CUtlVector<CUtlString> &cmd, const char *szName, const char *szValue ) override;
	
	virtual void SetTarget( CUtlVector<CUtlString> &cmd, CProject_t *pProject ) override;
	virtual void CompileFile( CUtlVector<CUtlString> &cmd, const char *szName ) override;
	virtual void SetOutputFile( CUtlVector<CUtlString> &cmd, const char *szName ) override;
	
	virtual void EnableDebugSymbols( CUtlVector<CUtlString> &cmd ) override;
	virtual void EnablePIE( CUtlVector<CUtlString> &cmd ) override;
	virtual void EnablePIC( CUtlVector<CUtlString> &cmd ) override;
	virtual void DisableStdInc( CUtlVector<CUtlString> &cmd ) override;

	static bool BTargetNeedsSysroot( Target_t target );
};

bool CClangCompiler::BTargetNeedsSysroot( Target_t target )
{
	if (target.kernel == TARGET_KERNEL_WINDOWS_MSVC)
		return true;

	return false;
};

const char *CClangCompiler::GetOutputObjectFormat()
{
	return ".o";
}

CUtlVector<CUtlString> CClangCompiler::BuildCommandLine( CProject_t *pProject, const char *szFileName, const char *szOutputFileName )
{
	CUtlVector<CUtlString> cmd;
	cmd = ICCompiler::BuildCommandLine(pProject, szFileName, szOutputFileName);
	cmd.AppendHead("-c");
	return cmd;
}


const char *CClangCompiler::GetCompilerExecutable( CProject_t *pProject )
{
	return "clang";
}


void CClangCompiler::IncludeDirectory( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail("-I");
	cmd.AppendTail(szName);
}

void CClangCompiler::IncludeFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
}

void CClangCompiler::Macro( CUtlVector<CUtlString> &cmd, const char *szName )
{
}

void CClangCompiler::Macro( CUtlVector<CUtlString> &cmd, const char *szName, const char *szValue )
{
	cmd.AppendTail("-D");
	cmd.AppendTail(CUtlString("%s=%s", (char*)szName, (char*)szValue));
}

void CClangCompiler::EnableDebugSymbols( CUtlVector<CUtlString> &cmd )
{
	cmd.AppendTail("-g");
}

void CClangCompiler::SetTarget( CUtlVector<CUtlString> &cmd, CProject_t *pProject )
{
	IINISection *pIncludeSection;
	const char *szIncludes;
	if (!g_pConfig)
	{
		if (BTargetNeedsSysroot(pProject->m_target))
			Plat_FatalErrorFunc("Target requires sysroot to be set\n Create .fpccfg\n");
		goto setTarget;
	}
	pIncludeSection = g_pConfig->GetSection("CLANG_C_COMPILER_INTERFACE_NAME.sysroot.include");
	if (!pIncludeSection)
	{
		if (BTargetNeedsSysroot(pProject->m_target))
			Plat_FatalErrorFunc("You forgot section CLANG_C_COMPILER_INTERFACE_NAME.sysroot.include\n");
		goto setTarget;
	}
	if (pProject->m_target.kernel == TARGET_KERNEL_WINDOWS_MSVC)
	{
		szIncludes = pIncludeSection->GetStringValue(pProject->m_target.GetTriplet());
		if (!szIncludes)
			Plat_FatalErrorFunc(CUtlString("Should be like this: %s = \"your_includes\"\n", pProject->m_target.GetTriplet().GetString()));
		cmd.AppendTail("-I");
		cmd.AppendTail(CUtlString("%s/shared",szIncludes));
		cmd.AppendTail("-I");
		cmd.AppendTail(CUtlString("%s/ucrt",szIncludes));
	}

setTarget:
	cmd.AppendTail("-target");
	cmd.AppendTail(pProject->m_target.GetTriplet());
}

void CClangCompiler::CompileFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail(szName);
}
void CClangCompiler::SetOutputFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail("-o");
	cmd.AppendTail(szName);
}
void CClangCompiler::EnablePIE( CUtlVector<CUtlString> &cmd )
{
	cmd.AppendTail("-fPIE");
}

void CClangCompiler::EnablePIC( CUtlVector<CUtlString> &cmd )
{
	cmd.AppendTail("-fPIC");
}

void CClangCompiler::DisableStdInc( CUtlVector<CUtlString> &cmd )
{
	cmd.AppendTail("-nostdinc");
}


EXPOSE_INTERFACE(CClangCompiler, ICCompiler, CLANG_C_COMPILER_INTERFACE_NAME);

CUtlVector<ClangFile_t> g_clangFiles;
IClangBackend *clangbackend = NULL;

LinkProject_t CClangCompiler::Compile( CProject_t *pProject )
{
	if (!clangbackend && CommandLine()->CheckParam("-experimental_header_include"))
		clangbackend = (IClangBackend*)CreateInterface(CLANG_BACKEND_INTERFACE_NAME, NULL);

	if (pProject->m_szName == 0)
	{
		Plat_FatalErrorFunc("m_szName must be present\n");
	}

	LinkProject_t proj = {};
	proj.m_szName = pProject->m_szName;
	proj.m_target = pProject->m_target;
	proj.m_androidmanifest = pProject->m_androidmanifest;
	unsigned int hash = pProject->GenerateProjectHash();

	// Get output directories
	for (auto &file: pProject->files)
	{
		CUtlString szOutputFile = GetOutputObjectName(pProject, hash, file);
		CUtlString szOutputDir = szOutputFile;
		szOutputDir = dirname(szOutputDir);
		filesystem2->MakeDirectory(szOutputDir);
	}

	// Run CC
	for (auto &file: pProject->files)
	{
		V_printf("  CC       %s\n", file.GetString());
		
		bool bAreDependenciesUpdated = false;
		CUtlString szOutputFile = GetOutputObjectName(pProject, hash, file);
		CUtlVector<CUtlString> args;
		/*
		CUtlString szTarget = pProject->m_target.GetTriplet();
		CUtlString szCompiledTarget = szTarget; 
		if (pProject->m_target.kernel == TARGET_KERNEL_ANDROID)
		{
			szCompiledTarget = CUtlString("%s%u", szTarget.GetString(), pProject->m_androidmanifest.m_nTargetVersion);
		}
		CUtlString szOutputFile = CUtlString("%s/%s/cc/%u_%s/%s/%s.o",FPC_TEMPORAL_DIRNAME, szTarget.GetString(), hash, pProject->m_szName.GetString(), filesystem2->BuildDirectory(), file.GetString());

		args = {
			"-target",
			szCompiledTarget,
		};
		*/
		
		/*
		if (!strcmp(Plat_GetExtension(file),"cpp"))
			args.AppendTail("-std=c++17"); 
		else if (!strcmp(Plat_GetExtension(file),"mm"))
			;
		else
			args.AppendTail("-std=c99");
		*/
		
		args = BuildCommandLine(pProject, file, szOutputFile);

		/*
		if (pProject->m_target.kernel == TARGET_KERNEL_DARWIN)
		{
			args.AppendTail("-isysroot");
			args.AppendTail("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk");
		} else if (pProject->m_target.kernel == TARGET_KERNEL_IOS)
		{
			args.AppendTail("-isysroot");
			args.AppendTail("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk");
			args.AppendTail("-miphoneos-version-min=18.0 ");
			args.AppendTail("-fembed-bitcode");
		}

		if (pProject->m_target.szSysroot)
		{
			args.AppendTail(CUtlString("--sysroot=%s", pProject->m_target.szSysroot));
		}
		*/

		runner->Run(GetCompilerExecutable(pProject), args);
skipcompile:
		proj.objects.AppendTail((Object_t){szOutputFile});

		ClangFile_t cfile = {};
		cfile.m_szName = file;
		cfile.m_szArguments = args;
		cfile.m_szArguments.AppendHead(GetCompilerExecutable(pProject));

		g_clangFiles.AppendTail(cfile);
	}
	runner->Wait();	
	return proj;
}

void CClangCompiler::GenerateLinterData()
{
	FILE* f = V_fopen("compile_commands.json", "wb");
	V_fprintf(f, "[\n");
	uint32_t i = 0;
	for (auto &file: g_clangFiles)
	{
		V_fprintf(f, "\t{\n");
		V_fprintf(f, "\t\t\"arguments\": [\n");
		for (auto &arg: file.m_szArguments)
			V_fprintf(f, "\t\t\t\"%s\",\n",arg.GetString());
	
		V_fseek(f, -2, SEEK_CUR);
		V_fprintf(f, "\n\t\t],\n");
		V_fprintf(f, "\t\t\"file\": \"%s\",\n", file.m_szName.GetString());
		V_fprintf(f, "\t\t\"directory\": \"%s\"\n", filesystem2->BuildDirectory());
		V_fprintf(f, "\t},\n");
	};
	V_fseek(f, -2, SEEK_CUR);
	V_fprintf(f, "\n]\n");
	V_fclose(f);
};
