/*
 * Copyright (c) 2020-2022 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <AppInfo.h>

#include <Misc/ModuleManager.h>
#include <Misc/Log.h>
#include <Misc/QIniConfig.h>
#include <Robokit/RobokitManager.h>
#include <QQmlContext>
#include <QQuickWindow>

#include <Robokit/Map/MapManager.h>
#include <Robokit/Map/MapUnit.h>

#include <Qml/ArrowHeads.h>
#include <Qml/CppBridge.h>

/**
 * Configures the application font and configures application signals/slots to destroy
 * singleton classes before the application quits.
 */
Misc::ModuleManager::ModuleManager(QQmlApplicationEngine* engine)
{
    // Init translator

    // Load Roboto fonts from resources
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/RobotoMono-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/RobotoMono-Regular.ttf");

    // Set Roboto as default app font
    QFont font("Roboto");
#if defined(Q_OS_WIN)
    font.setPointSize(9);
#elif defined(Q_OS_MAC)
    font.setPointSize(13);
#elif defined(Q_OS_LINUX)
    font.setPointSize(10);
#endif
//    qApp->setFont(font);

    // Enable software rendering
#ifdef SERIAL_STUDIO_SOFTWARE_RENDERING
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
#endif
    m_engine = engine;
    // Stop modules when application is about to quit
    connect(engine, &QQmlApplicationEngine::quit, this, &ModuleManager::onQuit);

    connect(engine, &QQmlApplicationEngine::quit, &QGuiApplication::quit);
}

/**
 * Returns a pointer to the QML application engine
 */
QQmlApplicationEngine *Misc::ModuleManager::engine()
{
    return m_engine;
}

/**
 * Sets the default options for QSimpleUpdater, which are:
 * - Notify user when a new update is found
 * - Do not notify user when we finish checking for updates
 * - Do not close application if update is found
 */
void Misc::ModuleManager::configureUpdater()
{
    if (!autoUpdaterEnabled())
        return;

//    QSimpleUpdater::getInstance()->setNotifyOnUpdate(APP_UPDATER_URL, true);
//    QSimpleUpdater::getInstance()->setNotifyOnFinish(APP_UPDATER_URL, false);
//    QSimpleUpdater::getInstance()->setMandatoryUpdate(APP_UPDATER_URL, false);
}



/**
 * Register custom QML types, for the moment, we have:
 * - JSON Frame object
 * - JSON Group object
 * - JSON Dataset object
 */

void Misc::ModuleManager::registerQmlTypes()
{

    //map.readMapFile("daTing20221107.smap");

    qmlRegisterType<MapUnit>("MapUnit", 1, 0, "MapUnit");

    qmlRegisterType<ArrowHeads>("ArrowHeads", 1, 0, "ArrowHeads");

    qmlRegisterType<CppBridge>("CppBridge", 1, 0, "CppBridge");
//    qmlRegisterType<UI::DashboardWidget>("SerialStudio", 1, 0, "DashboardWidget");
}

/**
 * Enables or disables the auto-updater system (QSimpleUpdater).
 *
 * To disable QSimpleUpdater, you need to add DEFINES += DISABLE_QSU in the qmake project
 * file. This option is provided for package managers, users are expected to update the
 * application using the same package manager they used for installing it.
 */
bool Misc::ModuleManager::autoUpdaterEnabled()
{
#ifdef DISABLE_QSU
    return false;
#else
    return true;
#endif
}

/**
 * Initializes all the application modules, registers them with the QML engine and loads
 * the "main.qml" file as the root QML file.
 */
void Misc::ModuleManager::initializeQmlInterface(QUrl url)
{
   
//    auto projectCodeEditor = &Project::CodeEditor::instance();
//    auto ioBluetoothLE = &IO::Drivers::BluetoothLE::instance();

    // Initialize third-party modules
//    auto updater = QSimpleUpdater::getInstance();

    // Operating system flags
    bool isWin = false;
    bool isMac = false;
    bool isNix = false;
    QString osName = tr("Unknown OS");
#if defined(Q_OS_MAC)
    isMac = true;
    osName = "macOS";
#elif defined(Q_OS_WIN)
    isWin = true;
    osName = "Windows";
#elif defined(Q_OS_LINUX)
    isNix = true;
    osName = "GNU/Linux";
#else
    isNix = true;
    osName = "UNIX";
#endif

    // Qt version QML flag
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const bool qt6 = false;
#else
    const bool qt6 = true;
#endif



    // Register C++ modules with QML
    auto c = engine()->rootContext();
    c->setContextProperty("Cpp_Qt6", qt6);
    c->setContextProperty("Cpp_IsWin", isWin);
    c->setContextProperty("Cpp_IsMac", isMac);
    c->setContextProperty("Cpp_IsNix", isNix);
    c->setContextProperty("Cpp_OSName", osName);


 

    c->setContextProperty("Cpp_UpdaterEnabled", autoUpdaterEnabled());
    c->setContextProperty("Cpp_ModuleManager", this);

    auto RobokitManagerd = &RobokitManager::instance();
    RobokitManagerd->RobokitConnectionInit();
    auto RobotStatus = &RobokitManagerd->robot_status;
    auto McuBoard = &RobokitManagerd->board_manager.mcu_board;
    auto RobServer = &RobokitManagerd->status_manager;
    auto Bat =  &RobokitManagerd->board_manager.bat;
    auto MissionManagerd = &MissionManager::instance();
    //MissionManagerd->InitConnection();

    c->setContextProperty("RobokitManagerd", RobokitManagerd);
    c->setContextProperty("RobotStatus", RobotStatus);
    c->setContextProperty("RobotMap", &SeerrMap::GetInstance());
    c->setContextProperty("McuBoard", McuBoard);
     c->setContextProperty("RobServer", RobServer);
    c->setContextProperty("Bat", Bat);
    c->setContextProperty("MissionManager", MissionManagerd);

    // Register app info with QML
    c->setContextProperty("Cpp_AppName", qApp->applicationName());
    c->setContextProperty("Cpp_AppUpdaterUrl", APP_UPDATER_URL);
    c->setContextProperty("Cpp_AppVersion", qApp->applicationVersion());
    c->setContextProperty("Cpp_AppOrganization", qApp->organizationName());
    c->setContextProperty("Cpp_AppOrganizationDomain", qApp->organizationDomain());

    // Load main.qml
    engine()->load(url);
}

/**
 * Calls the functions needed to safely quit the application
 */
void Misc::ModuleManager::onQuit()
{
    //CSV::Export::instance().closeFile();
    //CSV::Player::instance().closeFile();
    //IO::Manager::instance().disconnectDriver();

    //Plugins::Server::instance().removeConnection();
   // QIniConfig::GetInstance()-> ~QIniConfig();
    LOG_DeInit();
}

