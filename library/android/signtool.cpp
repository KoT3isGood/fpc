#include "signtool.h"
#include "tier0/platform.h"
#include "tier1/interface.h"

class CAndroidSignTool : public ISignTool
{
public:
	virtual void SignFile( CUtlString szFile ) override;
	virtual void SignDirectory( CUtlString szDirectory ) override;
};

EXPOSE_INTERFACE(CAndroidSignTool, ISignTool, ANDROID_SIGN_TOOL_INTERFACE_NAME);

void CAndroidSignTool::SignFile( CUtlString szFile )
{
	
}

void CAndroidSignTool::SignDirectory( CUtlString szDirectory )
{
	V_printf("Android doesn't support signing of directories\n");
}

