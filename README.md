项目文件结构
|src|
|----|文件夹 IO
namespace IO
通信物理层实现，例如TCP,串口。类应该提供open(),close(),isOpen(),wirte(),read(),状态监测，自动重连功能，断开信号。
1. TcpSocketClient.cpp
实现TcpSocket，实现keepalive机制，监测连接状态，自动重连。
2. DataProcessUtl.cpp
数据处理相关的静态函数实现，例如CRC
|----|文件夹 Misc
namespace Misc
系统级功能的实现，涉及程序前端管理、日志系统、翻译、自动更新等
1. Log.cpp
提供线程安全、灵活、以及多粒度控制的特点，通过将日志划分优先级使其可以面向程序调试、运行、测试、和维护等全生命周期。日志将输出到屏幕、文件、远程服务器；通过文件修改日志策略。
2. ModuleManager.cpp
前端相关实例的位置
3. ThemeManager.cpp
前端主题管理
4. TimerEvents.cpp
系统级Timer任务循环
5. Translator.cpp
翻译器
6. Utilities.cpp
自动更新、远程更新的实现
|----|文件夹 Robokit
机器人的全部接口实现及管理
|----|----|文件夹 BoardControl
namespace boardDevice
自制设备的接口管理，例如继电器主板、超声波、倒车红外。文件下有名为BoardManager的类，该类管理所有boardDevice,所有设备应该是BoardManager的成员，外部通过BoardManager初始化设备，状态查询，状态控制。
不同设备有不同的协议与通信方式，为了保持文件结构简洁，每个独立设备下使用名为Protocol的类完成协议层设计。