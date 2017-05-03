#include <iostream>
#include "appconfig.h"

#include <Windows.h>

#include "logger.h"
#include "winkeys.h"
#include "backtowork.h"

int main(int argc, char *argv[]) {
#ifndef _DEBUG
    FreeConsole();
#endif

    LOG << "Started";
    
    std::string configPath(DEFAULT_CONFIG_NAME);
    if (argc > 1) {
        configPath = std::string(argv[2]);
    }

    AppConfig config(configPath);
    if (!config.parse()) {
        LOG << "Can't make use of config. Falling back to defaults...";
        config.initDefaults();
    }

    int hotKeyModifier = config.getHotKeyModifier();
    int hotKey = config.getHotKey();

    if (RegisterHotKey(
                NULL,
                1,
                hotKeyModifier | MOD_NOREPEAT,
                hotKey))
    {
        LOG << "Registered Ctrl+Shift+0";
    } else {
        LOG << "Failed to register key combination. Exiting..";
        return 1;
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            LOG << "Received hotkey";
            backToWork(config);
        }
    }
    
    return 0;
}
