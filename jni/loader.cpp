#include "impl/common/starter.h"
#include <cstdio>
#include <jni.h>
#include <utils/android.h>
#include <android/log.h>

extern "C" {
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    	if (
    		vm->GetEnv(
    			reinterpret_cast<void**>(&Android::ApplicationEnv), 
    			JNI_VERSION_1_6
    		) != JNI_OK) {
    		printf("Failed to get JNI Environment!");
    	}

		Android::Toast::showToast("Starting RemDeb at port 8088!");
		RemDebMain::startServer();
        return JNI_VERSION_1_6;
    }
}
