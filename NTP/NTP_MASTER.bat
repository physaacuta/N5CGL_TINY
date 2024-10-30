w32tm /config /manualpeerlist:10.27.202.32,0x8 /syncfromflags:manual
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v MinPollInterval /t REG_DWORD /d 6 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v MaxPollInterval  /t REG_DWORD /d 12 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v MaxAllowedPhaseOffset  /t REG_DWORD /d 300 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v UpdateInterval  /t REG_DWORD /d 3000 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v MaxPosPhaseCorrection  /t REG_DWORD /d 172800 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\Config /v MaxNegPhaseCorrection  /t REG_DWORD /d 172800 /f

reg add HKLM\SYSTEM\CurrentControlSet\services\W32Time\TimeProviders\NtpServer /v Enabled  /t REG_DWORD /d 1  /f

sc config w32time start= delayed-auto
sc stop w32time
timeout 1
sc start w32time
timeout 3
w32tm /query /status /verbose


