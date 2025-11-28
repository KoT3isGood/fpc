#include "c.h"
#include "target.h"
#include <libgen.h>

ICCompiler *ccompiler;

CUtlString ICCompiler::GetOutputObjectName( CProject_t *pProject, unsigned int hash, CUtlString szFileName )
{
	CUtlString szTarget = pProject->m_target.GetTriplet();
	return CUtlString("%s/%s/cc/%u_%s/%s/%s%s",FPC_TEMPORAL_DIRNAME, szTarget.GetString(), hash, pProject->m_szName.GetString(), filesystem2->BuildDirectory(), szFileName.GetString(), GetOutputObjectFormat());
};

CUtlVector<CUtlString> ICCompiler::BuildCommandLine( CProject_t *pProject, const char *szFileName, const char *szOutputFileName )
{

	CUtlVector<CUtlString> cmd;

	EnableDebugSymbols(cmd);
	if (pProject->bFPIC)
		EnablePIC(cmd);
	if (pProject->bFPIE)
		EnablePIE(cmd);
	SetTarget(cmd, pProject);
	SetOutputFile(cmd, szOutputFileName);
	CompileFile(cmd, szFileName);
	if (pProject->bDisableStdInc)
		DisableStdInc(cmd);
	for (auto &macro: pProject->macros)
		Macro(cmd, macro.szName, macro.szValue.GetString());
	for (auto &dir: pProject->includeDirectories)
		IncludeDirectory(cmd, dir);

	return cmd;
}

LinkProject_t ICCompiler::Compile( CProject_t *pProject )
{
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
		CUtlString szOutputFile = GetOutputObjectName(pProject, hash, file);
		CUtlVector<CUtlString> args;

		args = BuildCommandLine(pProject, file, szOutputFile);
		if (!filesystem2->ShouldRecompile(file, szOutputFile))
			goto skipcompile;
		runner->Run(GetCompilerExecutable(pProject), args);
skipcompile:
		proj.objects.AppendTail((Object_t){szOutputFile});
	}
	runner->Wait();	
	return proj;
}
