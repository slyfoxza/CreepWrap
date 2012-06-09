/* ********************************************************************************************** */
/*
 * Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
 * LICENSE.txt in the source distribution for the full terms of the license.
 */
/* ********************************************************************************************** */
#include <cstdlib>
#include <iostream>

#include <JNI.h>
#include <TChar.h>
#include <Windows.h>
#include <StrSafe.h>

#include "JVMInvoke.h"

/* ********************************************************************************************** */
int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE previousInstance, LPTSTR commandLine,
		int showCommand)
{
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if(argc > 2)
	{
		MessageBox(NULL, _T("CreepWrap accepts a single, optional, argument, which should be the ")
					_T("path to Minecraft.exe.\n\nIn the absence of this argument, CreepWrap will ")
					_T("attempt to load Minecraft.exe from the current working directory."),
				_T("CreepWrap usage"), MB_ICONWARNING | MB_OK);
		return 1;

	}

	try
	{
		jvminvoke::JavaVM javaVM;

		const std::string mainClass("net/minecraft/LauncherFrame");
		if(argc == 1)
		{
			javaVM.executeJAR("Minecraft.exe", mainClass);

		}
		else
		{
			LPSTR jarPathBuffer = static_cast<LPSTR>(std::malloc((std::wcslen(argv[1]) + 1) *
						sizeof(char)));
			jarPathBuffer[std::wcslen(argv[1])] = '\0';
			CharToOemBuff(argv[1], jarPathBuffer, static_cast<DWORD>(std::wcslen(argv[1])));
			std::string jarPath(jarPathBuffer);
			std::free(jarPathBuffer);
			javaVM.executeJAR(jarPath, mainClass);

		}

	}
	catch(const jvminvoke::JavaException &e)
	{
		std::wstring message = e.wideMessage() + L"\n\nJava exception details:\n" + e.javaMessage();
		MessageBoxW(NULL, message.c_str(), L"Error", MB_ICONERROR | MB_OK);
		return -2;

	}
	catch(const jvminvoke::RuntimeException &e)
	{
		MessageBoxA(NULL, e.message().c_str(), "Error", MB_ICONERROR | MB_OK);
		return e.returnCode();

	}
	catch(const jvminvoke::Exception &e)
	{
		MessageBoxA(NULL, e.message().c_str(), "Error", MB_ICONERROR | MB_OK);
		return -1;

	}

	return 0;

} // _tWinMain

/* ********************************************************************************************** */
// vim: set ts=4 sw=4 noet:
