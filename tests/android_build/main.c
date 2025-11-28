
#include "android_native_app_glue.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeApp", __VA_ARGS__))

// Handle lifecycle events
static void handle_cmd(struct android_app* app, int32_t cmd) {
	switch (cmd) {
		case APP_CMD_INIT_WINDOW:
			LOGI("Window created");
			// You could init OpenGL/Vulkan here
			break;
		case APP_CMD_TERM_WINDOW:
			LOGI("Window destroyed");
			break;
	}
}

// Entry point for native app
void android_main(struct android_app* app) {
	app->onAppCmd = handle_cmd;

	LOGI("Native app started");

	int events;
	struct android_poll_source* source;

	// Main event loop
	while (1) {
		int ident;
		while ((ident = ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) {
			if (source) source->process(app, source);
			if (app->destroyRequested) {
				LOGI("App destroy requested");
				return;
			}
		}

		// Here you can update your game/render loop
	}
}
