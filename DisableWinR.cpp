#include <windows.h>
#include <stdio.h>
#include <wtsapi32.h>
#include <userenv.h>


HHOOK g_hook = NULL;
BOOL g_win_down = FALSE;
BOOL is_system = FALSE;
char* open_url = NULL;

BOOL IsRunningAsSystem();
int open_url_as_user(const char* url);

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
void cleanup();


void HandleWinR() {
    printf("Blocked Win+R\n");

    if (open_url != NULL) {
        if (is_system) {
            open_url_as_user(open_url);
        }
        else {
            ShellExecuteA(NULL, "open", open_url, NULL, NULL, SW_SHOWNORMAL);
        }
    }
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

        // Track Win key
        if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN) {
            if (wParam == WM_KEYDOWN)
                g_win_down = TRUE;
            else if (wParam == WM_KEYUP)
                g_win_down = FALSE;
        }

        // Intercept Win+R
        if (wParam == WM_KEYDOWN && p->vkCode == 'R' && g_win_down) {
            HandleWinR();

            // Block key (r)
            return 1;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


BOOL InstallHook() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    if (!g_hook) {
        printf("ERROR: Failed to install low level hook\n");
        return false;
    }
    printf("Hook installed.\n");
    return true;
}


int main(int argc, char** argv) {
    if (IsRunningAsSystem()) {
		printf("INFO: running as SYSTEM\n");
		is_system = TRUE;
    }
	
    if (argc == 2) {
        open_url = argv[1];
        printf("URL to open: %s\n", open_url);
        printf("Note: doesnt work with local admin. Needs service or SYSTEM.\n");
    }

    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        printf("WARNING: Could not set ctrl-c handler\n");
    }

    if (!InstallHook()) {
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    cleanup();
    return 0;
}


void cleanup() {
    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
        printf("Ctrl-C pressed!\n");
        cleanup();
        break;

    case CTRL_CLOSE_EVENT:
        printf("Console window is closing.\n");
        cleanup();
        break;

    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        printf("User is logging off or system is shutting down.\n");
        cleanup();
        break;
    }

    return false;
}

