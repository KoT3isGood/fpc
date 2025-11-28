#include "tier0/mem.h"
#include "winerunner.h"
#include "c.h"
#include "helper.h"
#include "obj.h"
#include "target.h"
#include "tier0/lib.h"
#include "tier0/mem.h"
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

class CMSVCCompiler : public ICCompiler
{
public:

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
};

const char *CMSVCCompiler::GetOutputObjectFormat()
{
	return ".o";
}

CUtlVector<CUtlString> CMSVCCompiler::BuildCommandLine( CProject_t *pProject, const char *szFileName, const char *szOutputFileName )
{
	CUtlVector<CUtlString> cmd;
	cmd = ICCompiler::BuildCommandLine(pProject, szFileName, szOutputFileName);
	cmd.AppendHead("/c");
	return cmd;
}


const char *CMSVCCompiler::GetCompilerExecutable( CProject_t *pProject )
{
	if (!g_pConfig)
		Plat_FatalErrorFunc(".fpccfg was not found\n");
	static IINISection *pSection = g_pConfig->GetSection("MSVC_C_COMPILER_INTERFACE_NAME");
	if (!pSection)
		Plat_FatalErrorFunc("MSVC_C_COMPILER_INTERFACE_NAME was not found in .fpccfg\n");
	static CUtlString szExePath = pSection->GetStringValue("exe");
	return szExePath;
}


void CMSVCCompiler::IncludeDirectory( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail(CUtlString("/I%s",szName));
}

void CMSVCCompiler::IncludeFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
}

void CMSVCCompiler::Macro( CUtlVector<CUtlString> &cmd, const char *szName )
{
}

void CMSVCCompiler::Macro( CUtlVector<CUtlString> &cmd, const char *szName, const char *szValue )
{
	cmd.AppendTail(CUtlString("/D%s=%s", (char*)szName, (char*)szValue));
}

void CMSVCCompiler::EnableDebugSymbols( CUtlVector<CUtlString> &cmd )
{
}

void CMSVCCompiler::SetTarget( CUtlVector<CUtlString> &cmd, CProject_t *pProject )
{
}

void CMSVCCompiler::CompileFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail(szName);
}
void CMSVCCompiler::SetOutputFile( CUtlVector<CUtlString> &cmd, const char *szName )
{
	cmd.AppendTail("/Fo");
	cmd.AppendTail(szName);
}
void CMSVCCompiler::EnablePIE( CUtlVector<CUtlString> &cmd )
{
}

void CMSVCCompiler::EnablePIC( CUtlVector<CUtlString> &cmd )
{
}
void CMSVCCompiler::DisableStdInc( CUtlVector<CUtlString> &cmd )
{

}


EXPOSE_INTERFACE(CMSVCCompiler, ICCompiler, MSVC_C_COMPILER_INTERFACE_NAME);

void CMSVCCompiler::GenerateLinterData()
{
	/*
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
	*/
};

