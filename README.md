# Video Hardware Accelerated
Example of using FFmpeg for decode encode and rendering with HW acceleration.

### Windows Build
#### Needs:
* MS Visual Studio 2015/2017
* Qt 5.7 or greater
* FFmpeg (tested with n3.4.2)
* Mysys64
* Mingw

#### Steps:
1. Download MS Visual Studio 2015/2017
2. Download & install Qt for Windows and msvc kit
3. Download Msys64
4. Setup Msys64 with developer tools
5. Download & build FFmpeg

```shell
./configure --prefix={Your/FFmpeg/build/Path} --disable-doc --disable-debug --enable-shared --disable-static --enable-runtime-cpudetect --disable-postproc --toolchain=msvc --enable-pic --extra-cflags="-MD" --extra-libs=user32.lib --disable-avresample --enable-hwaccels --enable-dxva2 --enable-libmfx --enable-nonfree --enable-gpl --enable-libx264
```

```shell
make -j4 install
```

6. Put your FFmpeg build into your Qt dir
