etrhz.exe hztable.h Global.c Setup.c iSetup.c
::ehz24.exe /s hzk24s.h myhz.c
ehz24.exe /k hzk24k.h _HZ24STR.c
::ehz24.exe /h hzk24h.h myhz.c
::ehz24.exe /f hzk24f.h myhz.c


make
if exist foxdisk.exe copy foxdisk.exe D:\MySoft\VirtualPC\DOS\driveE
make clean