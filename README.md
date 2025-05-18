# Disable Win-R

This disables the `Win-R` shortcut, which is used to open the Run dialog in Windows. 

Because of how common ClickFix execbait attacks are.

No more: 

![phishing](https://github.com/dobin/DisableWinR/blob/master/verification.png?raw=true)

Update: Can disable it in windows, see [this toot](https://infosec.exchange/@mttaggart/114503066592856224):

`HKEY_CURRENT_USER\SOFTWARE\Microsoft\CurrentVersion\Explorer\Advanced\DisabledHotkeys = R`.


# Usage

Execute as user. No privileges are required.

Win-R is only blocked as long as the program is running. 

Start:
```
> DisableWinR.exe

Hook installed.
Blocked Win+R
Blocked Win+R
```

To open a URL in the browser when `Win-R` is pressed, 
give URL as first argument:
```
> DisableWinR.exe https://example.com
```

To make it start but not do anything:
```
> DisableWinR.exe --disable https://example.com
> DisableWinR.exe --disable
```


# How it works

It will silently eat the `R` when `Win` is pressed.

Using `SetWindowsHookEx()` callback. 

