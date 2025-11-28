//================= Copyright kotofyt, All rights reserved ==================//
// Purpose: Target manager for compilers.
//===========================================================================//

#ifndef TARGET_T
#define TARGET_T

#include "tier1/utlstring.h"
#include "tier1/commandline.h"

enum ETargetKernel
{
	TARGET_KERNEL_UNKNOWN = 0,
	TARGET_KERNEL_LINUX,
	TARGET_KERNEL_WINDOWS_MSVC,
	TARGET_KERNEL_WINDOWS_GNU,
	TARGET_KERNEL_WINDOWS = TARGET_KERNEL_WINDOWS_GNU,
	TARGET_KERNEL_DARWIN,
	TARGET_KERNEL_IOS,
	TARGET_KERNEL_ANDROID,
	TARGET_KERNEL_WASI,
	TARGET_KERNEL_EMSCRIPTEN,
};

enum ETargetCPU
{
	TARGET_CPU_AMD64,
	TARGET_CPU_I286,
	TARGET_CPU_I386,
	TARGET_CPU_I486,
	TARGET_CPU_I586,
	TARGET_CPU_I686,
	TARGET_CPU_AARCH64,
	TARGET_CPU_WASM32,
};

enum ETargetOptimization
{
	TARGET_DEBUG,
	TARGET_RELEASE_SPEED,
	TARGET_RELEASE_SIZE
};

struct Target_t
{
	ETargetKernel kernel;
	ETargetCPU cpu;
	ETargetOptimization optimization;
	const char *szSysroot = CommandLine()->ParamValue("-sysroot");

	CUtlString GetTriplet();
	static Target_t HostTarget();
	static Target_t DefaultTarget();
};

enum EShaderTarget
{
	SHADER_TARGET_VULKAN_SPIRV,
	SHADER_TARGET_OPENGL_SPIRV,
	SHADER_TARGET_GLSL,
	SHADER_TARGET_HLSL,
	SHADER_TARGET_MSL,
};


#endif
