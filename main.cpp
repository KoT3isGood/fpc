#include "tier0/platform.h"
#include "tier1/commandline.h"
#include "tier1/interface.h"
#include "tier1/utlvector.h"
#include "tier2/ifilesystem.h"
#include "tier2/fileformats/ini.h"
#include "public/c.h"
#include "public/helper.h"
#include "public/ld.h"
#include "public/target.h"
#include "runner.h"
#include "winerunner.h"
#include "c.h"
#include "signal.h"
#include "libgen.h"

CUtlString owndir;
extern char *g_szBuildDir;
int build()
{



	CProject_t compileScriptProject = {};
	compileScriptProject.m_szName = "build";
	compileScriptProject.files = {"build.cpp"};
	compileScriptProject.includeDirectories = {CUtlString("%s/public",filesystem2->OwnDirectory()),CUtlString("%s/public", filesystem2->BuildDirectory()), CUtlString("%s/../public",filesystem2->OwnDirectory()),CUtlString("%s/../public", filesystem2->BuildDirectory())};
	compileScriptProject.bFPIC = true;
	compileScriptProject.m_target = Target_t::HostTarget();

	LinkProject_t linkScriptProject = ccompiler->Compile(&compileScriptProject);
	linkScriptProject.linkType = ELINK_DYNAMIC_LIBRARY;
	linkScriptProject.m_target = Target_t::HostTarget();

	CUtlString script = linker->Link(&linkScriptProject);


	void *scriptDLL = Plat_LoadLibrary(script);

	auto PreinitFn = (void(*)())Plat_GetProc(scriptDLL, "Preinit");
	if (PreinitFn)
		PreinitFn();

	for (auto &build: BuildStages())
	{
		build->m_pMainFn();
	};
	Plat_UnloadLibrary(scriptDLL);

	ccompiler->GenerateLinterData();

	return 0;
};


void IEngine_Signal(int sig)
{
	switch (sig)
	{
	case SIGSEGV:
	case SIGILL:
	case SIGABRT:
		Plat_Backtrace();
		Plat_FatalErrorFunc("Fault");
		break;
	default:
		break;
	};
	Plat_Exit(0);
};

int main(int c, char **v)
{	
	char path[1024];

	CUtlString buildcppDir = getcwd(path, 1024);
	owndir = buildcppDir;
	char *szBuildcppDir = buildcppDir.GetString();
findbuild:
	FILE* file = V_fopen("build.cpp", "rb");
	if (!file)
	{
		dirname(szBuildcppDir);
		if (buildcppDir=="/")
		{
			V_printf("build.cpp not found\n");
			return 0;
		}
		chdir(szBuildcppDir);
		goto findbuild;
	} else {
		V_fclose(file);
	}
	g_szBuildDir = szBuildcppDir;

	#ifdef __linux
	signal(SIGHUP, IEngine_Signal);
	signal(SIGINT, IEngine_Signal);
	signal(SIGQUIT, IEngine_Signal);
	signal(SIGILL, IEngine_Signal);
	signal(SIGTRAP, IEngine_Signal);
	signal(SIGIOT, IEngine_Signal);
	signal(SIGBUS, IEngine_Signal);
	signal(SIGFPE, IEngine_Signal);
	signal(SIGSEGV, IEngine_Signal);
	signal(SIGTERM, IEngine_Signal);
	#endif

	runner = (IRunner*)CreateInterface(RUNNER_INTERFACE_NAME, NULL);
	winerunner = (IWineRunner*)CreateInterface(WINE_RUNNER_INTERFACE_NAME, NULL);
	filesystem2 = (IFileSystem2*)CreateInterface(FILE_SYSTEM_2_INTERFACE_NAME, NULL);
	ccompiler = (ICCompiler*)CreateInterface(CLANG_C_COMPILER_INTERFACE_NAME, NULL);
	linker = (ILinker*)CreateInterface(CLANG_LINKER_INTERFACE_NAME, NULL);
	filesystem->Init();

	g_pConfig = INIManager()->ReadFile(".fpccfg");

	CommandLine()->CreateCommandLine(c, v);
	if (CommandLine()->CheckParam("build"))
		return build();
	return 0;
};
