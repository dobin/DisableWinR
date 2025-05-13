# Disable Win-R

This disables the Win-R shortcut, which is used to open the Run dialog in Windows. 

Because of copy Win-R paste phishing attacks


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


# How it works

It will silently eat the `R` when `Win` is pressed when doing `Win-R`.

Using `SetWindowsHookEx()` callback. 

