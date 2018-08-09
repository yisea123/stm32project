rem set TOOLS_PATH=..\..\..\tools\generate_firmware\trunk

set TOOLS_PATH=C:\Users\Public\Projekte\tools\generate_firmware\trunk
set PROJECT_PATH=C:\Users\Public\Projekte\coSource\example\boot_stm32f1_atollic

set GENERATOR=%TOOLS_PATH%\generate_firmware.exe
set OBJCOPY=%TOOLS_PATH%\mingw\objcopy
set OBJDUMP=%TOOLS_PATH%\mingw\objdump

rem set TARGET=%PROJECT_PATH%\obj
rem set COOCOX_TARGET=slave1_boot\Debug\bin
set ATOLLIC_TARGET=Debug

set TARGET=%PROJECT_PATH%\%ATOLLIC_TARGET%
set APPL=boot_slave.elf



rem check path
%GENERATOR% -?

REM %OBJDUMP% -h %TARGET%\%APPL%.elf

rem generate binary
%OBJCOPY% -I ihex -O binary --gap-fill=0xFF %TARGET%\%APPL%.hex %TARGET%\%APPL%.bin

rem check for double files and new created one
dir %TARGET%\*.hex %TARGET%\*.bin

@echo ""
@echo ""

%GENERATOR% -c512 -i %TARGET%\%APPL%.bin -o %TARGET%\%APPL%.crc
PAUSE
