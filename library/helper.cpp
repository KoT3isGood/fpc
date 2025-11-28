#include "helper.h"
#include "runner.h"
#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"
#include "unistd.h"
#include "libgen.h"
#include "sys/stat.h"
#include "tier1/interface.h"
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

unsigned int g_hashState = 102851263;
unsigned int BaseProject_t::GenerateProjectHash( void )
{
	unsigned int hash = 5381+g_hashState;
	int c;
	char *szName = m_szName;

	while( (c = *szName++) )
		hash = (hash * 33) + c;

	g_hashState = g_hashState * 1664525 + 1013904223;

	return hash;
};

static char path[1024];
#ifdef __linux__
static ssize_t pathSize = readlink("/proc/self/exe", path, sizeof(path) - 1);
#endif
#ifdef __APPLE__
static uint32_t pathSize = sizeof(path);
static int pathResult = _NSGetExecutablePath(path, &pathSize);
#endif
static char *szPathDir = dirname(path);
char *g_szBuildDir = 0;

class CPOSIXFileSystem2: public IFileSystem2
{
public:
	virtual char *OwnDirectory() override;
	virtual char *BuildDirectory() override;
	virtual void MakeDirectory( const char *psz ) override;
	virtual void CopyFile( const char *szDestination, const char *szOrigin ) override;
	virtual void CopyDirectory( const char *szDestination, const char *szOrigin ) override;
	virtual bool ShouldRecompile( const char *szSource, const char *szOutput ) override;
};

char *GetWindowsPath( const char *szPath )
{
	char *szNewPath = (char*)V_malloc(V_strlen(szPath)+1);
	int i = 0;
	V_strcpy(szNewPath, szPath);
	while(szNewPath[i])
	{
		if (szNewPath[i] == '/')
			szNewPath[i] = '\\';
		i++;
	}
	return szNewPath;
}

char *GetPOSIXPath( const char *szPath )
{
	char *szNewPath = (char*)V_malloc(V_strlen(szPath)+1);
	int i = 0;
	V_strcpy(szNewPath, szPath);
	while(szNewPath[i])
	{
		if (szNewPath[i] == '\\')
			szNewPath[i] = '/';
		i++;
	}
	return szNewPath;
}

EXPOSE_INTERFACE(CPOSIXFileSystem2, IFileSystem2, FILE_SYSTEM_2_INTERFACE_NAME);
IFileSystem2 *filesystem2;

char *CPOSIXFileSystem2::OwnDirectory()
{
	return szPathDir;
};
char *CPOSIXFileSystem2::BuildDirectory()
{
	return g_szBuildDir;
};
	
void CPOSIXFileSystem2::CopyFile( const char *szDestination, const char *szOrigin )
{
	CUtlVector<CUtlString> args = {
		CUtlString(szOrigin),
		CUtlString(szDestination),
	};
	runner->Run("cp", args);
	runner->Wait();	
}
void CPOSIXFileSystem2::CopyDirectory( const char *szDestination, const char *szOrigin )
{
	CUtlVector<CUtlString> args = {
		"-r",
		CUtlString(szOrigin),
		CUtlString(szDestination),
	};
	runner->Run("cp", args);
	runner->Wait();	
}

void CPOSIXFileSystem2::MakeDirectory( const char *psz )
{
	CUtlVector<CUtlString> args = {
		"-p",
		CUtlString(psz),
	};
	runner->Run("mkdir", args);
	runner->Wait();	
};

bool CPOSIXFileSystem2::ShouldRecompile(const char *szSource, const char *szOutput)
{
	struct stat srcbuf;
	struct stat outbuf;
	if (stat(szSource, &srcbuf) != 0) {
		return true;
	}
	if (stat(szOutput, &outbuf) != 0) {
		return true;
	}
	return outbuf.st_mtime < srcbuf.st_mtime;
};

CUtlVector<CBuildStage*> g_buildStages;

CBuildStage::CBuildStage( CUtlString sz, int(*pMainFn)() )
{
	m_sz = sz;
	m_pMainFn = pMainFn;
	if (sz == 0 || pMainFn == 0)
		Plat_FatalErrorFunc("Name and function pointer must be set\n");
	
	g_buildStages.AppendTail(this);
};

CUtlVector<CBuildStage*>& BuildStages()
{
	return g_buildStages;
}

IINIFile *g_pConfig;
