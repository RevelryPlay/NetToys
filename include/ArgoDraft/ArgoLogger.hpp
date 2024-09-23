#ifndef ARGOLOGGER_HPP
#define ARGOLOGGER_HPP
#include <string>

namespace ArgoDraft {
    struct LogLevel {
        static constexpr int NONE = -1;
        static constexpr int DEBUG = 0;
        static constexpr int INFO = 5;
        static constexpr int WARNING = 10;
        static constexpr int ERROR = 15;
        static constexpr int CRITICAL = 20;
    };

    class Logger {
    public:
        Logger();

        explicit Logger(const std::string &fileName);

        void LogMessage(
            const char *message,
            int level
        ) const;

        void SetFileName(
            const std::string &fileName
        );

        void SetFileLogLevel(
            int fileLogLevel
        );

        void SetConsoleLogLevel(
            int consoleLogLevel
        );

    private:
        std::string fileName;
        int fileLogLevel = LogLevel::WARNING;
        int consoleLogLevel = LogLevel::NONE;

        void CreateLogFile() const;

        void WriteToLogFile(
            const char *message,
            int level
        ) const;

        [[nodiscard]] bool CanConsoleLog(
            int level
        ) const;

        [[nodiscard]] bool CanFileLog(
            int level
        ) const;
    };
}

#endif //ARGOLOGGER_HPP
