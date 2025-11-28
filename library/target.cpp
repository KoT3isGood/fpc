#include "target.h"
#include "tier1/commandline.h"
#include "tier1/utlstring.h"
	
//-----------------------------------------------------------------------------
// Generates triplet suitable for most compilers.
//-----------------------------------------------------------------------------
CUtlString Target_t::GetTriplet()
{
	CUtlString triplet = "";

	if ( cpu == TARGET_CPU_AMD64 )
		triplet.AppendTail("x86_64");
	if ( cpu == TARGET_CPU_AARCH64 )
		triplet.AppendTail("aarch64");
	if ( cpu == TARGET_CPU_WASM32 )
		triplet.AppendTail("wasm32");
	triplet.AppendTail("-");
	if ( kernel == TARGET_KERNEL_UNKNOWN )
		triplet.AppendTail("unknown-unknown");
	if ( kernel == TARGET_KERNEL_LINUX )
		triplet.AppendTail("unknown-linux-gnu");
	if ( kernel == TARGET_KERNEL_WINDOWS_GNU )
		triplet.AppendTail("pc-windows-gnu");
	if ( kernel == TARGET_KERNEL_WINDOWS_MSVC )
		triplet.AppendTail("pc-windows-msvc");
	if ( kernel == TARGET_KERNEL_DARWIN )
		triplet.AppendTail("apple-darwin");
	if ( kernel == TARGET_KERNEL_IOS )
		triplet.AppendTail("apple-ios");
	if ( kernel == TARGET_KERNEL_ANDROID )
		triplet.AppendTail("linux-android");
	if ( kernel == TARGET_KERNEL_WASI )
		triplet.AppendTail("unknown-wasi");
	if ( kernel == TARGET_KERNEL_EMSCRIPTEN )
		triplet.AppendTail("unknown-emscripten");


	return triplet;
}

//----------------------------------------------------------------------------
// Returns target on which fpc is being run
//----------------------------------------------------------------------------
Target_t Target_t::HostTarget()
{
	ETargetKernel kernel = 
#if defined(__linux__)
		TARGET_KERNEL_LINUX
#elif defined(__APPLE__)
		TARGET_KERNEL_DARWIN
#endif
	;
	ETargetCPU cpu = TARGET_CPU_AMD64;
	return {
		.kernel = kernel,
		.cpu = cpu,
		.optimization = TARGET_DEBUG,
	};
};

//-----------------------------------------------------------------------------
// Returns default target for build, by default it will be host target
//-----------------------------------------------------------------------------
Target_t Target_t::DefaultTarget()
{
	CUtlString szDevice = CommandLine()->ParamValue("-device");
	CUtlString szOS = CommandLine()->ParamValue("-os");
	CUtlString szArch = CommandLine()->ParamValue("-arch");

	ETargetKernel kernel = HostTarget().kernel;
	ETargetCPU cpu = HostTarget().cpu;

	if ( szArch == "x86_64" )
		cpu = TARGET_CPU_AMD64;
	else if ( szArch == "aarch64" )
		cpu = TARGET_CPU_AARCH64;
	else if ( szArch == "wasm32" )
		cpu = TARGET_CPU_WASM32;

	if ( szOS == "unknown" )
		kernel = TARGET_KERNEL_UNKNOWN;
	else if ( szOS == "windows" )
		kernel = TARGET_KERNEL_WINDOWS;
	else if ( szOS == "windows-msvc" )
		kernel = TARGET_KERNEL_WINDOWS_MSVC;
	else if ( szOS == "linux" )
		kernel = TARGET_KERNEL_LINUX;
	else if ( szOS == "macos" )
		kernel = TARGET_KERNEL_DARWIN;
	else if ( szOS == "ios" )
		kernel = TARGET_KERNEL_IOS;
	else if ( szOS == "android" )
		kernel = TARGET_KERNEL_ANDROID;
	else if ( szOS != 0 )
		V_printf("Unknown OS: %s\n", szOS.GetString());
	
	return {
		.kernel = kernel,
		.cpu = cpu,
		.optimization = TARGET_DEBUG,
	};
}
