cl.exe /c /EHsc /I .\external\glew\include src\OGL.cpp

link.exe OGL.obj /LIBPATH:.\external\glew\lib\Release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

OGL

del *.exe *.obj
