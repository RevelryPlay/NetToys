#ifndef ARGOLOGGER_HPP
#define ARGOLOGGER_HPP

namespace ArgoDraft {
    class Logger {
    public:
        static void LogMessage(
            const char *message,
            const char *fileName = "log.log",
            bool console = false
        );

    private:
        static void CreateLogFile(const char *fileName, bool console = false);

        static void WriteToLogFile(
            const char* message,
            const char *fileName,
            bool console = false
        );
    };
}

#endif //ARGOLOGGER_HPP
