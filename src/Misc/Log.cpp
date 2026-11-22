#include "Log.h"

#include <string>
#include <iostream>

#if defined (Q_OS_LINUX) || defined (Q_OS_MACOS)

#elif defined (Q_OS_WIN)
inline std::basic_string<wchar_t> QStringToBasicChar(QString qStr)
{
    return (wchar_t*)reinterpret_cast<const wchar_t *>(qStr.utf16());
}
#endif


QLog::QLog(QObject *parent) : QObject(parent)
{
    this->InitLog();
}

QLog::~QLog()
{

     //spdlog::drop_all();
}

QLog *QLog::instance(){
    static QLog log;
    return &log;
}

std::shared_ptr<spdlog::logger> QLog::getLogger()
{
    return m_logger;
}

//初始化日志spdlog,https://github.com/gabime/spdlog
void QLog::InitLog()
{
    spdlog::init_thread_pool(8192, 1);

   auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
   stdout_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%@ %!][%l] : %v");

   //输出格式请参考https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
   stdout_sink->set_color_mode(spdlog::color_mode::always);
//   stdout_sink->set_color(spdlog::level::info,0x3580BB);
   auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/Mainlog.txt", 1024*1024*10, 3);
   std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
   m_logger = std::make_shared<spdlog::async_logger>("Log", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
   spdlog::register_logger(m_logger);
   m_logger->set_level(spdlog::level::trace);
   m_logger->debug("Log init succesd");
    try
   {

   }
   catch (const spdlog::spdlog_ex& ex)
   {
       std::cout << "Log initialization failed: " << ex.what() << std::endl;
   }
     auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
}


void QLog::loadConfFile(const QString& filepath)
{

}
void QLog::error( const QString&  string)
{

}

void QLog::warn( const QString&  string)
{

}

void QLog::info( const QString&  string)
{

}

void QLog::debug( const QString&  string)
{

}
//TOOD
//LOG4CPLUS_TRACE
