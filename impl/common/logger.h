#pragma once

#include <string>
#include <memory>

class LogWrapper {
public:
    virtual void info(std::string m) = 0;
    virtual void warn(std::string m) = 0;
    virtual void error(std::string m) = 0;
    virtual ~LogWrapper() = default;
};

class DefaultLogger : public LogWrapper {
public:
    void info(std::string m) override {
        printf("Info: %s\n", m.c_str());
    }

    void warn(std::string m) override {
        printf("Warn: %s\n", m.c_str());
    }

    void error(std::string m) override {
        printf("Error: %s\n", m.c_str());
    }
};

namespace Logger {
    inline std::shared_ptr<LogWrapper> lw = std::make_shared<DefaultLogger>();

    static inline void setWrapper(std::shared_ptr<LogWrapper> newLW) {
        lw = newLW;
    }

    static inline std::shared_ptr<LogWrapper> getLogger() {
        return lw;
    }
}
