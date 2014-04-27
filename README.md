This is README for HGE library source reconfigured with CMake

For original readme file refer to Original_README.txt

# INSTALLING

This package is configured with CMakeLists.txt for use with CMake. Once ran,
CMake generates project files for one of known IDE's for Windows (as well as
Linux/Unix/MacOSX but this version of HGE only builds on Windows).

1. Edit CMakeLists.txt in root folder, set "DIRECTX_SDK_DIR" to where you have
   "DirectX SDK Summer 2004" installed (for DirectX8 build) and some other DirectX SDK for
   DirectX9 build. If you don't have it yet - visit Microsoft website or search google
   for "DirectX SDK Summer 2004" (~230 MB download) or some fresh DirectX 9 SDK (~year 2010,
   size ~550 MB).
1. Run `cmake_msvs10_x86.bat` for Visual Studio 2010 32bit build (or other bat file,
   depending what's your IDE), note: you can create own bat file if CMake supports your IDE.
1. Project directory will be created in `HGE181/.build-%YOUR_IDE_VERSION%` (for example
   `HGE181/.build-msvs10`), open project file (solution file) inside with your IDE and
   press "BUILD" in your IDE.
1. Resulting library `HGE.DLL` and HGE.LIB will be placed in `HGE181/bin` directory, all
   intermediate libraries will be placed in `HGE181/linklib`. There is also HGE-STATIC
   library which doesn't require DLL to run, it can be found in `HGE181/linklib`.
1. All temporary build files will stay inside `.build-\*`. To clean intermediate files one
   should delete `HGE181/linklib`, `HGE181/bin` and `HGE181/.build-\*` 

## IMPORTANT

1. If you are going to change any build options, please keep in mind, that this is CMake
   generated project, editing anything inside .build-* folder will possibly be overwritten
   next time, when you compile HGE project. If you need to add any compiler options, please
   edit main or each `CMakeLists.txt` in HGE181 and `HGE181/src/*` folders (depending how big
   your changes are).
1. if you link against HGE library, you only need to mention HGE.LIB in project settings,
   and have `HGE.DLL` in your program folder. HGEHELPERS is not needed (its linked internally).
1. if you link against HGE-STATIC flavour of library, you don't need `HGE.DLL` anymore, all
   functionality will be included in your application. You need to mention HGEHELPERS, ZLIB 
   and HGELIBPNG in linker settings of your project for this to work (as well as D3D8.lib 
   D3DX8.lib or 9 if you are using DirectX9 and WINMM.lib). Consult project settings of 
   'hge' project regarding Linker input. 
