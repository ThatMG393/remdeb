#include <jni.h>

namespace Android {
    static JNIEnv* ApplicationEnv = { };
    static jobject ApplicationContext = nullptr;

    static JNIEnv* getApplicationEnv() {
        return ApplicationEnv;
    }

    static jobject getApplicationContext() {
        if (ApplicationContext != nullptr) return ApplicationContext;

        jclass ActivityThread = ApplicationEnv->FindClass("android/app/ActivityThread");
        jmethodID GetActivityThread = ApplicationEnv->GetStaticMethodID(ActivityThread, "currentActivityThread","()Landroid/app/ActivityThread;");
        jobject CurrentActivityThread = ApplicationEnv->CallStaticObjectMethod(ActivityThread, GetActivityThread);
        jmethodID GetApplication = ApplicationEnv->GetMethodID(ActivityThread, "getApplication", "()Landroid/app/Application;");

        ApplicationContext = ApplicationEnv->CallObjectMethod(CurrentActivityThread, GetApplication);
        return ApplicationContext;
    }

    namespace Toast {
        static void showToast(const char* message, const int length = 1) {
            getApplicationContext(); // trigger cache

            jclass Toast = ApplicationEnv->FindClass("android/widget/Toast");
			jmethodID MakeText = ApplicationEnv->GetStaticMethodID(Toast, "makeText", "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");

			jobject CreatedToast = ApplicationEnv->CallStaticObjectMethod(Toast, MakeText, ApplicationContext, message, length);

			jmethodID Show = ApplicationEnv->GetMethodID(Toast, "show", "()V;");
			ApplicationEnv->CallObjectMethod(CreatedToast, Show);
        }
    }
}