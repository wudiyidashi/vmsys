#
# Copyright (c) 2020-2021 Alex Spataru <https://github.com/alex-spataru>

#-----------------------------------------------------------------------------------------
# Make options
#-----------------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

isEmpty(PREFIX) {
    PREFIX = /usr
}

#-----------------------------------------------------------------------------------------
# Qt configuration
#-----------------------------------------------------------------------------------------

TEMPLATE = app                                           # Project template
TARGET = vmsys                                   # Set default target name
CONFIG += qtquickcompiler                                # Pre-compile QML interface
CONFIG += utf8_source                                    # Source code encoding
QTPLUGIN += qsvg                                         # Fixes issues with windeployqt

QT += xml
QT += svg
QT += core
QT += quick
QT += widgets
QT += bluetooth
QT += serialport
QT += printsupport
QT += quickcontrols2
QT += concurrent
QT += network
QT += multimedia

#equals(QT_MAJOR_VERSION, 6) {
#    QT += core5compat
#}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00

#-----------------------------------------------------------------------------------------
# Compiler options
#-----------------------------------------------------------------------------------------

CONFIG += c++17
CONFIG += silent

#-----------------------------------------------------------------------------------------
# Serial Studio compile-time settings
#-----------------------------------------------------------------------------------------

DEFINES += SERIAL_STUDIO_INCLUDE_MOC

#DEFINES += DISABLE_QSU     # If enabled, QSimpleUpdater shall not be used by the app.
                            # This is the default behaviour for MinGW.

#-----------------------------------------------------------------------------------------
# Libraries
#-----------------------------------------------------------------------------------------

include(libs/Libraries.pri)

#-----------------------------------------------------------------------------------------
# Assets
#-----------------------------------------------------------------------------------------

include(assets/Assets.pri)

#-----------------------------------------------------------------------------------------
# Deploy options
#-----------------------------------------------------------------------------------------

#临时文件存放位置
MOC_DIR         = temp/moc  #指定moc命令将含Q_OBJECT的头文件转换成标准.h文件的存放目录
RCC_DIR         = temp/rcc  #指定rcc命令将.qrc文件转换成qrc_*.h文件的存放目录
UI_DIR          = temp/ui   #指定rcc命令将.qrc文件转换成qrc_*.h文件的存放目录
OBJECTS_DIR     = temp/obj  #指定目标文件(obj)的存放目录

#指定生成的应用程序放置的目录
#DESTDIR         = bin

#指定生成的应用程序放置的目录
IDE_SOURCE_TREE = $$PWD #.pro或.pri文件所在的位置
IDE_BUILD_TREE  = $$IDE_SOURCE_TREE/../
win32:CONFIG(debug, debug|release){
    contains(DEFINES, WIN64) {
    DESTDIR = $$IDE_BUILD_TREE/_debug64
    } else {
    DESTDIR = $$IDE_BUILD_TREE/_debug86
    }
} else:win32:CONFIG(release, debug|release){
    contains(DEFINES, WIN64) {
    DESTDIR = $$IDE_BUILD_TREE/_release64
    } else {
    DESTDIR = $$IDE_BUILD_TREE/_release86
    }
}

lib_install.files += deploy/windows/resources/music
lib_install.path = $$DESTDIR/

message($$DESTDIR/)
win32* {
    TARGET = vmsys                                      # Change target name
    RC_FILE = deploy/windows/resources/info.rc           # Set applicaiton icon
    OTHER_FILES += deploy/windows/nsis/setup.nsi         # Setup script
    INSTALLS  += lib_install
}

macx* {
    TARGET = vmsys                                      # Change target name
    ICON = deploy/macOS/icon.icns                        # icon file
    RC_FILE = deploy/macOS/icon.icns                     # icon file
    QMAKE_INFO_PLIST = deploy/macOS/info.plist           # Add info.plist file
    CONFIG += sdk_no_version_check                       # Avoid warnings with Big Sur
}

linux:!android {
    PKGCONFIG += libssl                                  # Add OpenSSL library
    target.path = $$PREFIX/bin                           # Set binary installation path
    icon.path = $$PREFIX/share/pixmaps                   # icon instalation path
    desktop.path = $$PREFIX/share/applications           # *.desktop instalation path
    icon.files += deploy/linux/serial-studio.png         # Add application icon
    desktop.files += deploy/linux/serial-studio.desktop  # Add *.desktop file
    copyright.files += deploy/linux/copyright            # Libc6 file for linuxdeployqt
    copyright.path = $$PREFIX/share/doc/libc6            # libc6 copyright path
    INSTALLS += target copyright desktop icon            # make install targets
}

mingw {
    target.path = $$PREFIX/bin
    license.path = $$PREFIX/share/licenses/$$TARGET      # Set license install path
    license.files += LICENSE.md                          # Add LICENSE.md file
    INSTALLS += target license                           # Install target+licence (MSYS2)
    DEFINES += DISABLE_QSU                               # Disable QSimpleUpdater (MSYS2)
}

#-----------------------------------------------------------------------------------------
# Import source code
#-----------------------------------------------------------------------------------------

INCLUDEPATH += src

HEADERS += \
    src/AppInfo.h \
    src/DataTypes.h \
    src/IO/DataProcessUtl.h \
    src/IO/TcpSocketServer.h \
    src/IO/ThreadHandle.h \
    src/Misc/CRC.h \
    src/Misc/Log.h \
    src/IO/TcpSocketClient.h \
    src/Misc/ModuleManager.h \
    src/Misc/Music.h \
    src/Misc/QIniConfig.h \
    src/Misc/ThemeManager.h \
    src/Misc/TimerEvents.h \
    src/Misc/Translator.h \
    src/Misc/Utilities.h \
    src/Mission/Charge.h \
    src/Mission/Jack.h \
    src/Mission/MissionBase.h \
    src/Mission/MissionManager.h \
    src/Mission/RobotMove.h \
    src/Mission/RobotMoveList.h \
    src/Qml/ArrowHeads.h \
    src/Qml/CppBridge.h \
    src/Qml/EleCell.h \
    src/Robokit/BoardControl/Autodoor.h \
    src/Robokit/BoardControl/Battery.h \
    src/Robokit/BoardControl/BoardManager.h \
    src/Robokit/BoardControl/Lift.h \
    src/Robokit/BoardControl/McuBoard.h \
    src/Robokit/Map/MapElements.h \
    src/Robokit/Map/MapManager.h \
    src/Robokit/Map/MapUnit.h \
    src/Robokit/RobokitApi/RobProtocol.h \
    src/Robokit/RobokitApi/RobotBase.h \
    src/Robokit/RobokitApi/RobotConfig.h \
    src/Robokit/RobokitApi/RobotControl.h \
    src/Robokit/RobokitApi/RobotStatus.h \
    src/Robokit/RobokitApi/RobotTask.h \
    src/Robokit/RobokitApi/SCHeadData.h \
    src/Robokit/RobokitManager.h \
    src/Server/StatusManager.h

SOURCES += \
    src/IO/DataProcessUtl.cpp \
    src/IO/TcpSocketServer.cpp \
    src/IO/ThreadHandle.cpp \
    src/Misc/CRC.cpp \
    src/Misc/Log.cpp \
    src/IO/TcpSocketClient.cpp \
    src/Misc/ModuleManager.cpp \
    src/Misc/Music.cpp \
    src/Misc/QIniConfig.cpp \
    src/Misc/ThemeManager.cpp \
    src/Misc/TimerEvents.cpp \
    src/Misc/Translator.cpp \
    src/Misc/Utilities.cpp \
    src/Mission/Charge.cpp \
    src/Mission/Jack.cpp \
    src/Mission/MissionBase.cpp \
    src/Mission/MissionManager.cpp \
    src/Mission/RobotMove.cpp \
    src/Mission/RobotMoveList.cpp \
    src/Qml/ArrowHeads.cpp \
    src/Qml/CppBridge.cpp \
    src/Qml/EleCell.cpp \
    src/Robokit/BoardControl/Autodoor.cpp \
    src/Robokit/BoardControl/Battery.cpp \
    src/Robokit/BoardControl/BoardManager.cpp \
    src/Robokit/BoardControl/Lift.cpp \
    src/Robokit/BoardControl/McuBoard.cpp \
    src/Robokit/Map/MapManager.cpp \
    src/Robokit/Map/MapUnit.cpp \
    src/Robokit/RobokitApi/RobProtocol.cpp \
    src/Robokit/RobokitApi/RobotBase.cpp \
    src/Robokit/RobokitApi/RobotConfig.cpp \
    src/Robokit/RobokitApi/RobotControl.cpp \
    src/Robokit/RobokitApi/RobotStatus.cpp \
    src/Robokit/RobokitApi/RobotTask.cpp \
    src/Robokit/RobokitManager.cpp \
    src/Server/StatusManager.cpp \
    src/main.cpp
#-----------------------------------------------------------------------------------------
# Import QML source code
#-----------------------------------------------------------------------------------------

DISTFILES += \
    assets/qml/*.qml \
    assets/qml/Control/*.qml \
    assets/qml/View/*.qml

#-----------------------------------------------------------------------------------------
# Deploy files
#-----------------------------------------------------------------------------------------

OTHER_FILES += \
    deploy/linux/* \
    deploy/macOS/* \
    deploy/windows/nsis/* \
    deploy/windows/resources/* \
    updates.json

INSTALLS += lib_install
