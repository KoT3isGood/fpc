
#include "c_libclang.h"
#include "helper.h"
#include "tier1/interface.h"

#include "clang-c/Index.h"
#include <clang-c/CXFile.h>
#include <clang-c/CXString.h>

class CClangBackend: public IClangBackend
{
public:
	virtual bool AreFileDependenciesUpdated( CUtlString szFile, CUtlString szOutputFile, CUtlString szHashFile, CUtlVector<CUtlString> arguments ) override;

	bool bAnyOutdated;
	CUtlString szCurrentFile;
	CUtlString szHashFile;

private:
	static void InclusionVisitor( CXFile included_file, CXSourceLocation *include_stack, unsigned include_len, CXClientData client_data );
};

EXPOSE_INTERFACE(CClangBackend, IClangBackend, CLANG_BACKEND_INTERFACE_NAME);

bool CClangBackend::AreFileDependenciesUpdated( CUtlString szFile, CUtlString szOutputFile, CUtlString szHashFile, CUtlVector<CUtlString> arguments )
{
	szCurrentFile = szOutputFile;
	bAnyOutdated = false;

	CUtlVector<const char *> cArguments = {};
	for (auto &arg: arguments)
		cArguments.AppendTail(arg);

	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit tu;
	CXErrorCode err = clang_parseTranslationUnit2(index, szFile.GetString(), cArguments.GetData(), cArguments.GetSize(), NULL, 0, 0, &tu);

	if (err != CXError_Success) {
		printf("clang_parseTranslationUnit2 failed with error code %d\n", err);
		return false;
	}

	clang_getInclusions(tu, CClangBackend::InclusionVisitor, NULL);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);
	return bAnyOutdated;
}
	
void CClangBackend::InclusionVisitor( CXFile included_file, CXSourceLocation *include_stack, unsigned include_len, CXClientData client_data )
{
	if (((CClangBackend*)clangbackend)->bAnyOutdated)
		return;
	CXString filename = clang_getFileName(included_file);

	bool bUpdate = 
		filesystem2->ShouldRecompile(clang_getCString(filename), ((CClangBackend*)clangbackend)->szCurrentFile);
	if (bUpdate)
	{
		((CClangBackend*)clangbackend)->bAnyOutdated = true;
	}

	clang_disposeString(filename);
}

