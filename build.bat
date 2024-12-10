del output.png
del a.exe
gcc .\test.c -I ../ -l gdi32 -l winmm -l opengl32
a.exe
output.png
@REM mspaint.exe output.png