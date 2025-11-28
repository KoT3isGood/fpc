#include "runner.h"
#include "winerunner.h"
#include "tier0/platform.h"
#include "tier1/interface.h"
#include "tier1/utlstring.h"
#include "tier1/utlvector.h"
#include "unistd.h"
#include "sys/wait.h"
#include "tier1/commandline.h"

class CWineRunner: public IWineRunner
{
public:
	CWineRunner();
	~CWineRunner();

	virtual int Run( CUtlString szName, CUtlVector<CUtlString>& args ) override;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args ) override;
	virtual int Run( CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment ) override;
	virtual int Wait( void ) override;
private:
	pid_t m_wineServerPID;
};

EXPOSE_INTERFACE(CWineRunner, IWineRunner, WINE_RUNNER_INTERFACE_NAME);

CWineRunner::CWineRunner()
{	

}
CWineRunner::~CWineRunner()
{

}

int CWineRunner::Run(CUtlString szName, CUtlVector<CUtlString>& args)
{
	return runner->Run(szName, args);
}

int CWineRunner::Run(CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args)
{
	CUtlVector<CUtlString> args2 = args;
	args2.AppendHead(szName);
	return runner->Run("wine", szDirectory, args2);
}

int CWineRunner::Run(CUtlString szName, CUtlString szDirectory, CUtlVector<CUtlString>& args, CUtlVector<CUtlString>& environment)
{
	CUtlVector<CUtlString> args2 = args;
	args2.AppendHead(szName);
	return runner->Run("wine", szDirectory, args2, environment);
}

int CWineRunner::Wait( void )
{
	return runner->Wait();
};
