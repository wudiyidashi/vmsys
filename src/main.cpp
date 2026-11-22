#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSettings>

#include <AppInfo.h>

#include <Misc/ModuleManager.h>
#include <Robokit/RobokitManager.h>
#include <Misc/Log.h>
#include <Mission/MissionManager.h>
#include <Misc/QIniConfig.h>

#pragma execution_character_set("utf-8")

/**
 * Prints the current application version to the console
 */
static void cliShowVersion()
{
    auto appver = QString("%1 version %2").arg(APP_NAME, APP_VERSION);
    auto author = QString("Wirte by FR-YJ");

    qDebug() << appver.toStdString().c_str();
    qDebug() << author.toStdString().c_str();
}

/**
 * Removes all application settings
 */
static void cliResetSettings()
{
    QSettings(APP_DEVELOPER, APP_NAME).clear();
    qDebug() << APP_NAME << "settings cleared!";
}


//在windows中存在缩放比例会影响鼠标获取的坐标的问题，比如屏幕缩放为175%，那么qt获取的鼠标坐标本来是（100，100）
//将会变成（100/1.75,100/1.75),也就是（57，57）,于是我将通过windows api获取这个比例
//需要在QGuiApplication之前调用
#include "windows.h"
#pragma comment(lib,"User32.lib")

static double GetWindowsScaleFactor()
{
    // 获取窗口当前显示的监视器
    HWND hWnd = GetDesktopWindow();//根据需要可以替换成自己程序的句柄
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // 获取监视器逻辑宽度与高度
    MONITORINFOEX miex;
    miex.cbSize = sizeof(miex);
    GetMonitorInfo(hMonitor, &miex);
    int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
    int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

    // 获取监视器物理宽度与高度
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;
    int cyPhysical = dm.dmPelsHeight;

    //缩放比例计算
    double horzScale = ((double)cxPhysical / (double)cxLogical);
    double vertScale = ((double)cyPhysical / (double)cyLogical);

    return horzScale;
}

double WINDOWSSCALEFACTOR = 1;
int main(int argc, char *argv[])
{


    WINDOWSSCALEFACTOR = GetWindowsScaleFactor();
    QGuiApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(QString::number(APP_VERSION));
    app.setOrganizationName(APP_DEVELOPER);
    app.setOrganizationDomain(APP_SUPPORT_URL);

    // set current thread name
    QThread::currentThread()->setObjectName("MainThread");

    // Read arguments
    QString arguments;
    if (app.arguments().count() >= 2)
        arguments = app.arguments().at(1);

    // arguments control
    bool _gui_open = true;

    // There are some CLI arguments, read them
    if (!arguments.isEmpty() && arguments.startsWith("-"))
    {
        if (arguments == "-v" || arguments == "--version")
        {
            cliShowVersion();
            return EXIT_SUCCESS;
        }

        else if (arguments == "-r" || arguments == "--reset")
        {
            cliResetSettings();
            return EXIT_SUCCESS;
        }

        else if (arguments == "-gui")
        {
            _gui_open = true;
            return EXIT_SUCCESS;
        }
    }

    LOG_Init();
    LOGDEBUG("System Init ...");
    // Create Robokit manager
        QQmlApplicationEngine engine;
        const QUrl url(u"qrc:/qml/main.qml"_qs);
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
        // Create module manager
        Misc::ModuleManager moduleManager(&engine);
    // Create Robokit manager
//    auto robokitManager = new RobokitManager;
//    robokitManager->RobokitConnectionInit();

    // The console arguments _gui_open control GUI open or close
    // GUI default close

    // Create module manager
    QString configFileDir =  QCoreApplication::applicationDirPath();
    QString configFile = configFileDir +"/"+ "appconfig.inf";
    QIniConfig::GetInstance()->Init(configFile);

    // Initialize QML interface
    moduleManager.registerQmlTypes();
     moduleManager.initializeQmlInterface(url);
    if (moduleManager.engine()->rootObjects().isEmpty())
    {
        qCritical() << "Critical QML error";
        return EXIT_FAILURE;
    }

    // Enter application event loop
    return app.exec();
}
