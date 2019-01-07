if "%time:~0,1%"==" " set "time=0%time:~1%"
set fileDate=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%
echo %fileDate%>E:\rainbow\Measureboard\Debug\version.txt
echo 0.9.9>>E:\rainbow\Measureboard\Debug\version.txt
copy /y E:\rainbow\Measureboard\Debug\Measure.bin "Z:\Projects\Rainbow\1_Design\1_SW\measureboard binary code\Measure_epa.bin"
copy /y E:\rainbow\Measureboard\Debug\Measure.hex "Z:\Projects\Rainbow\1_Design\1_SW\measureboard binary code\Measure_epa.hex"
copy /y E:\rainbow\Measureboard\Debug\readme.txt "Z:\Projects\Rainbow\1_Design\1_SW\measureboard binary code"
copy /y E:\rainbow\Measureboard\Debug\version.txt "Z:\Projects\Rainbow\1_Design\1_SW\measureboard binary code"
copy /y E:\rainbow\Measureboard\Debug\Measure.bin "C:\Program Files\PuTTY"
copy /y E:\rainbow\Measureboard\Debug\Measure.hex "C:\Program Files\PuTTY"
copy /y E:\rainbow\Measureboard\Debug\readme.txt "C:\Program Files\PuTTY"
copy /y E:\rainbow\Measureboard\Debug\version.txt "C:\Program Files\PuTTY"
cd c:
c:
cd "C:\Program Files\PuTTY"
pscp -i key_p.ppk -scp Measure.bin pi@10.131.133.23:/home/pi/Desktop
pscp -i key_p.ppk -scp Measure.hex pi@10.131.133.23:/home/pi/Desktop
pscp -i key_p.ppk -scp readme.txt pi@10.131.133.23:/home/pi/Desktop
pscp -i key_p.ppk -scp Measure.bin pi@10.131.133.23:/home/pi/log/release/update/mcu/firmware.bin
pscp -i key_p.ppk -scp version.txt pi@10.131.133.23:/home/pi/log/release/update/mcu/version.txt