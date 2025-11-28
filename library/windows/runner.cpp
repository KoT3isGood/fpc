#include "runner.h"
#include "tier0/platform.h"
#include "tier1/interface.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"
#include "tier1/commandline.h"
#include "windows.h"

CUtlVector<pid_t> g_processes;

class CWindowsRunner: public IRunner
{
public:
	virtual int Run( CUtlString szName, CUtlVector<CUtlString>& args ) override;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args ) override;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment ) override;
	virtual int Wait( void ) override;
};

EXPOSE_INTERFACE(CWindowsRunner, IRunner, POSIX_RUNNER_INTERFACE_NAME);
IRunner *runner;

int CWindowsRunner::Run(CUtlString szName, CUtlVector<CUtlString>& args)
{
	pid_t pid = fork();
	if (pid < 0)
		Plat_FatalErrorFunc("Failed to fork");
	/* child */
	if (pid == 0)
	{
		CUtlVector<const char*> execargs;
		execargs.AppendTail(szName);
		if (CommandLine()->CheckParam("-fpcdebug"))
			V_printf("%s",szName.GetString());
		for (auto &arg: args)
		{
			execargs.AppendTail(arg);
			if (CommandLine()->CheckParam("-fpcdebug"))
				V_printf(" %s",arg.GetString());
		}
		if (CommandLine()->CheckParam("-fpcdebug"))
			V_printf("\n");
		execargs.AppendTail(0);
		if ( execvp(szName, (char *const*)execargs.GetData()) == -1 )
		{
			V_printf("Failed to launch %s\n",szName.GetString());
			_exit(0);
		}
	}
	g_processes.AppendTail(pid);
	return 0;
}

int CWindowsRunner::Run(CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args)
{
	pid_t pid = fork();
	if (pid < 0)
		Plat_FatalErrorFunc("Failed to fork");
	/* child */
	if (pid == 0)
	{
		CUtlVector<const char*> execargs;
		execargs.AppendTail(szName);
		if (CommandLine()->CheckParam("-fpcdebug"))
			V_printf("%s",szName.GetString());
		for (auto &arg: args)
		{
			execargs.AppendTail(arg);
			if (CommandLine()->CheckParam("-fpcdebug"))
				V_printf(" %s",arg.GetString());
		}
		if (CommandLine()->CheckParam("-fpcdebug"))
			V_printf("\n");
		execargs.AppendTail(0);
		chdir(szDirectory.GetString());
		if ( execvp(szName, (char *const*)execargs.GetData()) == -1 )
		{
			V_printf("Failed to launch %s\n",szName.GetString());
			_exit(0);
		}
	}
	g_processes.AppendTail(pid);
	/* parent */
	return 0;
}

int CWindowsRunner::Run(CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment)
{

}

int CWindowsRunner::Wait( void )
{
	for (auto &process: g_processes)
	{
		int status;
		pid_t wpid = waitpid(process, &status, 0);
	}
	g_processes = {};
	return 0;
};
