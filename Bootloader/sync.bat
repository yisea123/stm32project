copy /y E:\rainbow\Bootloader\Release\Bootloader.bin "Z:\Projects\Rainbow\1_Design\1_SW\measureboard binary code"
copy /y E:\rainbow\Bootloader\Release\Bootloader.bin "C:\Program Files\PuTTY"
cd c:
c:
cd "C:\Program Files\PuTTY"
pscp -i key_p.ppk -scp Bootloader.bin pi@10.131.133.23:/home/pi/Desktop