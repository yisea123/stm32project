set TOOLS_PATH=C:\svn\01001\tools\generate_firmware\trunk
set PROJECT_PATH=C:\svn\01004\example\boot_stm32f1_keil

set GENERATOR=%TOOLS_PATH%\generate_firmware.exe
set OBJCOPY=%TOOLS_PATH%\mingw\objcopy
set OBJDUMP=%TOOLS_PATH%\mingw\objdump
set TARGET=%PROJECT_PATH%\boot_stm32f1_keil
set APPL=boot_stm32f1_keil
set CONFIGBLOCK=0x200

rem check path
%GENERATOR% -?

rem remove unused section with address 0 by using ihex
rem %OBJCOPY% -I elf32-big -O ihex %TARGET%\%APPL%.elf %TARGET%\%APPL%.hex
rem %OBJDUMP% -h %TARGET%\%APPL%.hex

rem generate binary
%OBJCOPY% -I ihex -O binary --gap-fill=0xFF %TARGET%\%APPL%.hex %TARGET%\%APPL%.bin

rem check for double files and new created one
dir %TARGET%\*.hex %TARGET%\*.elf %TARGET%\*.bin
%GENERATOR% -c%CONFIGBLOCK%  -i %TARGET%\%APPL%.bin -o %TARGET%\%APPL%.crc

REM %OBJCOPY% --help
PAUSE