#include <windows.h>
#include <wtsapi32.h>
#include <userenv.h>
#include <stdio.h>
#include <sddl.h>
#include <stdio.h>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Userenv.lib")


/* Mostly chatgpt generated */

BOOL IsRunningAsSystem() {
    HANDLE token = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return FALSE;
    }

    BYTE buffer[SECURITY_MAX_SID_SIZE];
    DWORD size = sizeof(buffer);
    PSID systemSid = (PSID)buffer;

    if (!CreateWellKnownSid(WinLocalSystemSid, NULL, systemSid, &size)) {
        CloseHandle(token);
        return FALSE;
    }

    BYTE tokenUserBuffer[256];
    DWORD tokenUserSize = sizeof(tokenUserBuffer);
    if (!GetTokenInformation(token, TokenUser, tokenUserBuffer, tokenUserSize, &tokenUserSize)) {
        CloseHandle(token);
        return FALSE;
    }

    TOKEN_USER* tokenUser = (TOKEN_USER*)tokenUserBuffer;
    BOOL isSystem = EqualSid(tokenUser->User.Sid, systemSid);

    CloseHandle(token);
    return isSystem;
}


int open_url_as_user(const char* url) {
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    if (sessionId == 0xFFFFFFFF) {
        printf("No active user session.\n");
        return 1;
    }

    HANDLE userToken = NULL;
    if (!WTSQueryUserToken(sessionId, &userToken)) {
        printf("WTSQueryUserToken failed: %lu\n", GetLastError());
        return 1;
    }

    HANDLE duplicatedToken = NULL;
    if (!DuplicateTokenEx(userToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &duplicatedToken)) {
        printf("DuplicateTokenEx failed: %lu\n", GetLastError());
        CloseHandle(userToken);
        return 1;
    }

    // Build command line: use "cmd.exe /c start <url>"
    char commandLine[512];
    snprintf(commandLine, sizeof(commandLine), "cmd.exe /c start %s", url);

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);
    si.lpDesktop = (LPSTR) "winsta0\\default";

    // Optional: set environment for the user
    LPVOID environment = NULL;
    if (!CreateEnvironmentBlock(&environment, duplicatedToken, FALSE)) {
        printf("CreateEnvironmentBlock failed: %lu\n", GetLastError());
    }

    BOOL result = CreateProcessAsUserA(
        duplicatedToken,
        NULL,
        commandLine,
        NULL,
        NULL,
        FALSE,
        CREATE_UNICODE_ENVIRONMENT,
        environment,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        printf("CreateProcessAsUser failed: %lu\n", GetLastError());
    }
    else {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (environment) {
        DestroyEnvironmentBlock(environment);
    }

    CloseHandle(duplicatedToken);
    CloseHandle(userToken);
    return result ? 0 : 1;
}

