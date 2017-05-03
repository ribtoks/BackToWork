#include "backtowork.h"
#include "Windows.h"
#include <locale>
#include <codecvt>
#include <algorithm>

#include "common.h"
#include "appconfig.h"
#include "logger.h"

bool isPriorityAppActive(const AppConfig &config) {
    UNUSED(config);
    return false;
}

void activateWindow(HWND hwnd) {
    BringWindowToTop(hwnd);
    SetActiveWindow(hwnd);
    SetForegroundWindow(hwnd);
}

struct BackToWorkData {
    const AppConfig &m_AppConfig;
    int m_ActivatedWindowsCount;
    std::vector<HMONITOR> m_EngagedMonitors;
};

bool tryEngageMonitor(BackToWorkData *data, HMONITOR hmon) {
    bool engaged = false;
    auto &engagedMonitors = data->m_EngagedMonitors;

    if (std::find(engagedMonitors.begin(), engagedMonitors.end(), hmon) == engagedMonitors.end()) {
        engagedMonitors.push_back(hmon);
        engaged = true;
    }

    return engaged;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindow(hwnd) == 0) { return TRUE; }
    if (IsWindowVisible(hwnd) == 0) { return TRUE; }

    int len = GetWindowTextLength(hwnd) + 1;
    std::vector<wchar_t> titleBuffer(len);
    GetWindowTextW(hwnd, &titleBuffer[0], len);
    std::wstring windowTitleW(titleBuffer.begin(), titleBuffer.end());

    // wchar -> char (utf8)
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    std::string windowTitle = converter.to_bytes(windowTitleW);
    tolower(windowTitle);

    // LOG << "Cycling through window with title:" << windowTitle;

    BackToWorkData *data = (BackToWorkData *)lParam;
    auto &config = data->m_AppConfig;
    const int minWindowsToActivate = config.getWindowsToActivate();
    bool found = false;

    auto &apps = config.getApps();
    int i = 0;
    for (auto &app: apps) {
        if (windowTitle.find(app) != std::string::npos) {
            LOG << "Found match [" << windowTitle << "]";

            HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);

            if (tryEngageMonitor(data, hmon)) {
                LOG << "Activating window [" << windowTitle << "]";
                activateWindow(hwnd);
                data->m_ActivatedWindowsCount++;
            } else {
                LOG << "Seems to be the same monitor";
            }

            if (data->m_ActivatedWindowsCount >= minWindowsToActivate) {
                found = true;
            }

            break;
        }

        i++;
    }

    return found ? FALSE : TRUE;
}

void backToWork(const AppConfig &config) {
    if (isPriorityAppActive(config)) {
        LOG << "One of the apps is already active";
        return;
    }

    BackToWorkData data { config, 0 };

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

    LOG << "BackToWork finished";
}
