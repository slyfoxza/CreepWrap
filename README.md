# CreepWrap

CreepWrap is a simple wrapper for [Minecraft](http://www.minecraft.net/) to allow it to run in the context of an executable other than the usual `java.exe`/`javaw.exe`. This allows system tools that operate on the executable name of a process (such as the NVIDIA Control Panel, when applying per-application 3D settings) to directly target Minecraft instead of applying its settings to _all_ Java applications running on the system.

As an aside, the potential exists for the `JVMInvoke` source to be re-used in any other Java project to create an executable wrapper in the same way.

## Installation

CreepWrap can be installed anywhere on a system, but may require some configuration depending on its location relative to the Minecraft Launcher (`Minecraft.exe`). If CreepWrap and the Minecraft Launcher are installed in the same directory, no further configuration is necessary, as CreepWrap will default to trying to find the Minecraft Launcher in its working directory.

To explicitly point CreepWrap to the Minecraft Launcher, simply create a shortcut to CreepWrap, and pass it the path to `Minecraft.exe` as an argument, for example:

    C:\Path\To\CreepWrap.exe "C:\My Games\Minecraft.exe"

Note that CreepWrap requires the [Microsoft Visual C++ 2010 Redistributable Package](http://www.microsoft.com/en-us/download/details.aspx?id=14632) to be installed. Obviously, it also requires a Java Runtime Environment to be installed (currently, only Oracle's JRE is officially supported).

## Building from source

Currently, CreepWrap is known to successfully build using the [MinGW-w64](http://mingw-w64.sourceforge.net/) toolchain (installed via [Cygwin](http://cygwin.com)), as well as Microsoft Visual C++ 2010 Express (built from the command line using MSBuild). Only 64-bit builds of CreepWrap have been tested.

### MinGW-w64

Building with MinGW-w64 requires the creation of a `Makefile.x86_64_mingw.local` file containing a `CXXFLAGS` modification to add `-I` arguments pointing GCC to the location of your JDK's include directory.

To build a debug build, issue a `make debug` command. For a release build, `make` or `make release` will suffice.

### Microsoft Visual C++ 2010 Express with MSBuild

Before building with MSVC++, copy the `CreepWrap.JNIHeaderPath.example` file to `CreepWrap.JNIHeaderPath.props` and edit the path to the JDK include directory as appropriate for your build environment.

To build with MSVC++, start up a Visual Studio Command Prompt, or use the similar command prompt from the Windows SDK. To build, run the following:

    MSBuild /p:Platform=x64 /p:Configuration=Debug

Supported values for `Configuration` are `Debug` and `Release`. Only `x64` is currently supported for the `Platform` property.

## Licensing

Copyright (c) 2012, Philip Cronje
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
