# Disable Win-R

This disables the `Win-R` shortcut, which is used to open the Run dialog in Windows. 

Because of how common `Win-R` phishing attacks are.

No more: 

![phishing](https://github.com/dobin/DisableWinR/blob/master/verification.png?raw=true)

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

It will silently eat the `R` when `Win` is pressed.

Using `SetWindowsHookEx()` callback. 

