clang lib.c -shared -Wl,/def:lib.def -Wl,/implib:lib-clang.lib -o lib-clang.dll
clang app.c -Wl,lib-clang.lib -o app-clang.exe
gcc -shared lib.c -Wl,--version-script=lib.map -Wl,--version-script=lib-mingw.map -o lib-mingw.dll
gcc app.c -L. -llib-mingw -o app-mingw.exe