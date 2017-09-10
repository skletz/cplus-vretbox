@echo off
setlocal

FOR /r I:\dataset_keckstein_2015\temporal-evaluation\segments\test %%i IN (*.mp4) DO (
	ffmpeg -i %%i -codec:v libx264 -profile:v high -preset slow -b:v 500k -maxrate 180k -bufsize 1000k -vf scale=-1:280 -threads 0 -codec:a libfdk_aac -b:a 128k %%~di%%~pismall_%%~ni.mp4
	
)