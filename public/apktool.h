//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Set Android metadata aboth the package.
//===========================================================================//

#ifndef APK_TOOL_H
#define APK_TOOL_H

#include "tier0/platform.h"
#include "tier1/utlstring.h"

struct AndroidManifest_t
{
public:
	void SetPackageVersion( CUtlString szVersion );
	void SetPackageBuild( uint64_t nBuild );
	void SetPackageID( CUtlString szPackageID );
	void SetPackageName( CUtlString szPackageName );

	void SetTargetSDKVersion( uint64_t nTargetVersion );
	void SetMinSDKVersion( uint64_t nMinVersion );

	void AddUserFeature( CUtlString szName, bool bIsRequired, uint64_t nVersion );
	void AddUserLibrary( CUtlString szPath );
	CUtlString BuildManifest();

	CUtlString m_szPackageName;
	CUtlString m_szPackageID;
	uint64_t m_nBuild;
	CUtlString m_szVersion;

	uint64_t m_nTargetVersion;
	uint64_t m_nMinVersion;
};

abstract_class IAPKTool
{
public:
	virtual CUtlString BuildPackage( AndroidManifest_t manifest, CUtlString szManifestDir ) = 0;
};

IAPKTool *APKTool();

#endif
