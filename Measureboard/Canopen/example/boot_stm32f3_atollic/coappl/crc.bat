set TOOLS_PATH=C:\svn\01001\tools\generate_firmware\trunk
set PROJECT_PATH=C:\svn\01004\example\boot_stm32f3_atollic

set GENERATOR=%TOOLS_PATH%\generate_firmware.exe
set OBJCOPY=%TOOLS_PATH%\mingw\objcopy
set TARGET=%PROJECT_PATH%\Debug\
set APPL=boot_stm32f3_atollic
set CONFIGBLOCK=0x200

rem check path
%GENERATOR% -?

%OBJCOPY% -I elf32-little -O binary --gap-fill=0xFF %TARGET%\%APPL%.elf %TARGET%\%APPL%.bin

dir %TARGET%\*.hex %TARGET%\*.bin %TARGET%\*.elf 

@echo ""
@echo ""

%GENERATOR% -c%CONFIGBLOCK% -i %TARGET%\%APPL%.bin -o %TARGET%\%APPL%.crc

PAUSE
