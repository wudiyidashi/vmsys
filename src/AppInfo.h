#pragma once

#define APP_VER_MAJOR 0
#define APP_VER_MINOR 1
#define APP_VER_PATCH 0

#define APP_VERSION (APP_VER_MAJOR * 10000 + APP_VER_MINOR * 100 + APP_VER_PATCH)

#define _APP_STR(s) #s
#define PROJECT_VERSION(major, minor, patch) "v" _APP_STR(major.minor.patch)

// clang-format off

#define APP_DEVELOPER   ""
#define APP_NAME        ""
#define APP_SUPPORT_URL ""
#define APP_UPDATER_URL ""
// clang-format on
