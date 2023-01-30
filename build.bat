@echo off

if not exist build mkdir build

pushd build
cl /nologo /Zi /FC /I..\code\ ..\code\codegen\codegen.c
popd

pushd code
..\build\codegen.exe os/
popd

pushd build
cl /nologo /Zi /FC /I..\code\ ..\code\render\opengl\render_opengl.c /link /DLL user32.lib gdi32.lib opengl32.lib /out:render_opengl.dll
cl /nologo /Zi /FC /I..\code\ ..\code\app.c /link user32.lib gdi32.lib dwmapi.lib UxTheme.lib winmm.lib Advapi32.lib shell32.lib ole32.lib
popd
