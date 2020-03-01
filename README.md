# Demo of TLS Variable Export Issues

## Test Case

The library exports an thread-local `variable`
and a `function()` assigning current thread ID to that variable.

The program creates `N` threads (default 2). Each thread receives its index
as thread function parameter, and then:

1. Prints parameter.
1. Assigns parameter to `variable`.
1. Prints `variable` to check if assignment worked.
1. Calls `function()` to change `variable`.
1. Prints `variable` to check if `function()` worked.

Expected output from each thread (XXX is thread ID, N is thread index):

```
XXX: param = N
XXX: value (old) = N
XXX: value (new) = XXX
```

## Test Results

### Clang

Version:

```
Z:\tlstest>clang --version
clang version 9.0.1
Target: x86_64-pc-windows-msvc
Thread model: posix
InstalledDir: C:\Program Files\LLVM\bin
```

#### Native TLS

Build:

```
clang lib.c -shared -Wl,/def:lib.def -Wl,/implib:lib-clang.lib -o lib-clang.dll
clang app.c -Wl,lib-clang.lib -o app-clang.exe
```

Run:

```
Z:\tlstest>app-clang.exe
8400: param = 0
8400: value (old) = 0
8400: value (new) = 0
4904: param = 1
4904: value (old) = 1
4904: value (new) = 1
```

#### Emulated TLS

Build:

```
clang lib.c -femulated-tls --rtlib=compiler-rt -shared -Wl,/def:lib-emutls.def -Wl,/implib:lib-clang-emutls.lib -o lib-clang-emutls.dll
clang app.c -femulated-tls --rtlib=compiler-rt -Wl,lib-clang-emutls.lib -o app-clang-emutls.exe
```

Option `--rtlib=compiler-rt` is required to resolve `__emutls_get_address`.

Run:

```
Z:\my\tlstest>app-clang-emutls.exe
6988: param = 0
3920: param = 1
Windows error: The parameter is incorrect.
```

### MinGW-w64

Version:

```
Z:\tlstest>gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=C:/MinGW/mingw64/bin/../libexec/gcc/x86_64-w64-mingw32/8.1.0/lto-wrapper.exe
Target: x86_64-w64-mingw32
Configured with: ../../../src/gcc-8.1.0/configure --host=x86_64-w64-mingw32 --build=x86_64-w64-mingw32 --target=x86_64-w64-mingw32 --prefix=/mingw64 --with-sysroot=/c/mingw810/x86_64-810-posix-sjlj-rt_v6-rev0/mingw64 --enable-shared --enable-static --enable-targets=all --enable-multilib --enable-languages=c,c++,fortran,lto --enable-libstdcxx-time=yes --enable-threads=posix --enable-libgomp --enable-libatomic --enable-lto --enable-graphite --enable-checking=release --enable-fully-dynamic-string --enable-version-specific-runtime-libs --enable-sjlj-exceptions --disable-libstdcxx-pch --disable-libstdcxx-debug --enable-bootstrap --disable-rpath --disable-win32-registry --disable-nls --disable-werror --disable-symvers --with-gnu-as --with-gnu-ld --with-arch-32=i686 --with-arch-64=nocona --with-tune-32=generic --with-tune-64=core2 --with-libiconv --with-system-zlib --with-gmp=/c/mingw810/prerequisites/x86_64-w64-mingw32-static --with-mpfr=/c/mingw810/prerequisites/x86_64-w64-mingw32-static --with-mpc=/c/mingw810/prerequisites/x86_64-w64-mingw32-static --with-isl=/c/mingw810/prerequisites/x86_64-w64-mingw32-static --with-pkgversion='x86_64-posix-sjlj-rev0, Built by MinGW-W64 project' --with-bugurl=https://sourceforge.net/projects/mingw-w64 CFLAGS='-O2 -pipe -fno-ident -I/c/mingw810/x86_64-810-posix-sjlj-rt_v6-rev0/mingw64/opt/include -I/c/mingw810/prerequisites/x86_64-zlib-static/include -I/c/mingw810/prerequisites/x86_64-w64-mingw32-static/include' CXXFLAGS='-O2 -pipe -fno-ident -I/c/mingw810/x86_64-810-posix-sjlj-rt_v6-rev0/mingw64/opt/include -I/c/mingw810/prerequisites/x86_64-zlib-static/include -I/c/mingw810/prerequisites/x86_64-w64-mingw32-static/include' CPPFLAGS=' -I/c/mingw810/x86_64-810-posix-sjlj-rt_v6-rev0/mingw64/opt/include -I/c/mingw810/prerequisites/x86_64-zlib-static/include -I/c/mingw810/prerequisites/x86_64-w64-mingw32-static/include' LDFLAGS='-pipe -fno-ident -L/c/mingw810/x86_64-810-posix-sjlj-rt_v6-rev0/mingw64/opt/lib -L/c/mingw810/prerequisites/x86_64-zlib-static/lib -L/c/mingw810/prerequisites/x86_64-w64-mingw32-static/lib '
Thread model: posix
gcc version 8.1.0 (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project)
```

Build:

```
gcc -shared lib.c -Wl,--version-script=lib.map -Wl,--version-script=lib-mingw.map -o lib-mingw.dll
gcc app.c -L. -llib-mingw -o app-mingw.exe
```

Run:

```
Z:\tlstest>app-mingw.exe
3012: param = 0
3332: param = 1
3332: value (old) = 1
3332: value (new) = 3332
3012: value (old) = 0
3012: value (new) = 3012
```

## Conclusion

* MinGW-w64 uses emuTLS and works as expected,
  but requires explicit `__emults_v` symbols export.
* Clang using native TLS fails the test,
  because it is library's responsibility to follow [1].
* Clang using `-femulated-tls` with DLL crashes at runtime.
    * TLS within binary work as expected.

## References

1. [Using Thread Local Storage in a Dynamic-Link Library](https://docs.microsoft.com/en-us/windows/win32/dlls/using-thread-local-storage-in-a-dynamic-link-library)
