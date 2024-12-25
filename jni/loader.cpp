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



        return JNI_VERSION_1_6;
    }
}
