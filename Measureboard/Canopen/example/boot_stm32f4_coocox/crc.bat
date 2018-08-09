rem set TOOLS_PATH=..\..\..\tools\generate_firmware\trunk

set TOOLS_PATH= C:\Projekte\generate_firmware
set PROJECT_PATH=C:\Projekte\coSource2\example\boot_stm32f4_coocox

set GENERATOR=%TOOLS_PATH%\generate_firmware.exe
set OBJCOPY=%TOOLS_PATH%\mingw\objcopy
set OBJDUMP=%TOOLS_PATH%\mingw\objdump
rem set TARGET=%PROJECT_PATH%\obj
set COOCOX_TARGET=slave1_boot\Debug\bin
set TARGET=%PROJECT_PATH%\%COOCOX_TARGET%
set APPL=slave1_bl
rem set APPL=stm32f103_olimex_bl


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
