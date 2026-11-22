#ifndef LOG_H
#define LOG_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <QObject>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QDir>



class QLog : public QObject
{
    Q_OBJECT
public:
    explicit QLog(QObject *parent = nullptr);
    ~QLog();
    static QLog *instance();
    void InitLog();
    void error(const QString &string);
    void warn(const QString &string);
    void info(const QString &string);
    void debug(const QString &string);

    std::shared_ptr<spdlog::logger> getLogger();
private:
    std::shared_ptr<spdlog::logger> m_logger;

    void loadConfFile(const QString &filepath);
};

#define LOG_Init() QLog::instance()
#define LOG_DeInit() spdlog::drop_all();

#ifdef DJ
#define LOGTRACE(...) SPDLOG_LOGGER_CALL(QLog::instance()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define LOGDEBUG(...) SPDLOG_LOGGER_CALL(QLog::instance()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define LOGINFO(...) SPDLOG_LOGGER_CALL(QLog::instance()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define LOGWARN(...) SPDLOG_LOGGER_CALL(QLog::instance()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define LOGERROR(...) SPDLOG_LOGGER_CALL(QLog::instance()->getLogger().get(), spdlog::level::err, __VA_ARGS__)
#else

#define LOGTRACE(...)
#define LOGDEBUG(...)
#define LOGINFO(...)
#define LOGWARN(...)
#define LOGERROR(...)

#endif // 0


#endif // LOG_H
