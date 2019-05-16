#include "backtowork.h"

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <locale>
#include <string>
#include <vector>
#include <utility>

#include "Windows.h"

#include "common.h"
#include "appconfig.h"
#include "logger.h"

#define WAIT_TIME 200

bool isPriorityAppActive(const AppConfig &config) {
    UNUSED(config);
    return false;
}

bool activateWindow(HWND hwnd, int timeout) {
	WINDOWPLACEMENT place;
	memset(&place, 0, sizeof(WINDOWPLACEMENT));
	place.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &place);

	switch (place.showCmd)
	{
	case SW_SHOWMAXIMIZED:
		LOG << "Window was maximized";
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		break;
	case SW_SHOWMINIMIZED:
		LOG << "Window was minimized";
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		ShowWindow(hwnd, SW_RESTORE);
		break;
	default:
		LOG << "Window was in default state";
		ShowWindow(hwnd, SW_SHOW);
		break;
	}

    BOOL result = SetForegroundWindow(hwnd);
	if (!result) { return false; }
	BringWindowToTop(hwnd);
    SetActiveWindow(hwnd);

	HWND currentActive;
	int waited = 0;
	do {
		Sleep(WAIT_TIME);
		waited += WAIT_TIME;
		currentActive = GetForegroundWindow();
	} while ((hwnd != currentActive) && (waited <= timeout));
	
	bool succeeded = GetForegroundWindow() == hwnd;
	return succeeded;
}

struct BackToWorkData {
    const AppConfig &m_AppConfig;
    std::vector<std::pair<std::string, HWND>> m_MatchedWindows;
};

bool tryEngageMonitor(std::vector<HMONITOR> &engagedMonitors, HMONITOR hmon) {
    bool engaged = false;

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
    for (auto &app: apps) {
        if (windowTitle.find(app) != std::string::npos) {
            LOG << "Found match [" << windowTitle << "]";
            data->m_MatchedWindows.emplace_back(std::make_pair(windowTitle, hwnd));
        }
    }

    return TRUE;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::vector<HMONITOR> *monitors = (std::vector<HMONITOR> *)dwData;
    monitors->push_back(hMonitor);
    return TRUE;
}

std::vector<HMONITOR> RetrieveMonitors()
{
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&monitors);
    return monitors;
}

void activateWindow(const AppConfig &config, BackToWorkData &data, const std::function<bool(HWND hwnd)> &isWindowOk) {
    for (auto &app : config.getApps()) {
        for (auto &matches : data.m_MatchedWindows) {
            auto &windowTitle = matches.first;
            if (windowTitle.find(app) != std::string::npos) {
                HWND hwnd = matches.second;
                if (isWindowOk(hwnd)) {
                    LOG << "Activating window" << windowTitle;
					if (activateWindow(hwnd, config.getActivateTimeout() * 1000)) { return; }
					else { LOG << "ERROR:" << "failed to activate" << windowTitle; }
                }
            }
        }
    }
}

void backToWork(const AppConfig &config) {
    if (isPriorityAppActive(config)) {
        LOG << "One of the apps is already active";
        return;
    }

    BackToWorkData data { config };

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

    std::vector<HMONITOR> monitors = RetrieveMonitors();

    if (monitors.size() <= 1 || config.getWindowsToActivate() <= 1) {
        LOG << "Activating only 1 window";
        activateWindow(config, data, [](HWND) { return true; });
    } else {
        LOG << "Activating window per monitor up to" << std::to_string(config.getWindowsToActivate());
        for (size_t i = 0; i < monitors.size() && i < config.getWindowsToActivate(); i++) {
            HMONITOR hMonitor = monitors[i];
            activateWindow(config, data, [&hMonitor](HWND hwnd) {
                    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
                    return hmon == hMonitor;
                });
        }
    }

    LOG << "BackToWork finished";
}
