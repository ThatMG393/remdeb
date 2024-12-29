#include "impl/common/starter.h"
#include "impl/common/logger.h"
#include "jni/utils/android.h"
#include <jni.h>
#include <android/log.h>

#define REMDEB "RemDeb"

class AndroidLogWrapper : public LogWrapper {
public:
    void info(std::string m) override {
        __android_log_print(ANDROID_LOG_INFO, REMDEB, "%s", m.c_str());
    }

    void warn(std::string m) override {
        __android_log_print(ANDROID_LOG_WARN, REMDEB, "%s", m.c_str());
    }

    void error(std::string m) override {
        __android_log_print(ANDROID_LOG_ERROR, REMDEB, "%s", m.c_str());
    }
};

extern "C" {
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
		auto logger = std::make_shared<AndroidLogWrapper>();

    	if (
    		vm->GetEnv(
    			reinterpret_cast<void**>(&Android::ApplicationEnv), 
    			JNI_VERSION_1_6
    		) != JNI_OK) {
    		logger->error("Failed to get JNI Environment!");
    	}

		Android::Toast::showToast("Starting RemDeb at port 8088!");
		
		logger->info("HIIIII!");

		Logger::setWrapper(logger);
		RemDebMain::startServer();

		logger->info("okay so we good!");
        return JNI_VERSION_1_6;
    }
}
