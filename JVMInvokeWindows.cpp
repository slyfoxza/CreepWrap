/* ********************************************************************************************** */
/*
 * Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
 * LICENSE.txt in the source distribution for the full terms of the license.
 */
/* ********************************************************************************************** */
#include <cstdlib>

#include <TChar.h>
#include <Windows.h>

#include "JVMInvoke.h"

/* ********************************************************************************************** */
namespace
{
	/**
	 * Retrieves a <code>REG_SZ</code> typed value from the registry.
	 *
	 * @param  keyName   the name of the registry key containing the value
	 * @param  valueName the name of the value to be returned
	 * @return a string containing the data in the specified registry value
	 *
	 * @throw jvminvoke::RuntimeException If an error occurs while interacting with the Win32
	 *                                    registry API functions.
	 * @throw jvminvoke::Exception        If an error occurs while allocating memory for the value
	 *                                    buffer.
	 */
	jvminvoke::string getRegistryStringValue(const jvminvoke::string keyName,
			const jvminvoke::string valueName)
	{
		DWORD bufferSize;
		LONG returnCode;

		// Retrieve the required buffer size for the value
		returnCode = RegGetValue(HKEY_LOCAL_MACHINE, keyName.c_str(), valueName.c_str(),
				RRF_RT_REG_SZ, NULL, NULL, &bufferSize);
		if(returnCode != ERROR_SUCCESS)
		{
			throw jvminvoke::RuntimeException("Failed to read value from registry", returnCode);

		}

		LPTSTR bufferMemory = static_cast<LPTSTR>(std::malloc(bufferSize * sizeof(TCHAR)));
		if(bufferMemory == NULL)
		{
			throw jvminvoke::Exception("Failed to allocate buffer");

		}

		// Retrieve the value
		returnCode = RegGetValue(HKEY_LOCAL_MACHINE, keyName.c_str(), valueName.c_str(),
				RRF_RT_REG_SZ, NULL, bufferMemory, &bufferSize);
		if(returnCode != ERROR_SUCCESS)
		{
			std::free(bufferMemory);
			throw jvminvoke::RuntimeException("Failed to read value from registry", returnCode);

		}

		jvminvoke::string result(bufferMemory);
		std::free(bufferMemory);
		return result;

	} // getRegistryStringValue

} // namespace <anonymous>

/* ********************************************************************************************** */
bool jvminvoke::fileExists(const std::string path)
{
	return (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES);

} // jvminvoke::fileExists

/* ********************************************************************************************** */
jvminvoke::JNI_CreateJavaVM_f jvminvoke::importCreateJavaVMFromDefaultDLL()
{
	const string jreBaseKeyName(_T("SOFTWARE\\JavaSoft\\Java Runtime Environment"));

	string jreRuntimeLibPath = getRegistryStringValue(jreBaseKeyName + _T("\\") +
			getRegistryStringValue(jreBaseKeyName.c_str(), _T("CurrentVersion")),
			_T("RuntimeLib"));

	try
	{
		return importCreateJavaVMFromDLL(jreRuntimeLibPath);

	}
	catch(RuntimeException &e)
	{
		if(e.returnCode() == ERROR_MOD_NOT_FOUND)
		{
			/* The Oracle JRE on Windows 64-bit platforms (as of version 7) has a bug where the
			 * installer will setup the registry keys in such a way that this code would try to load
			 * the JVM DLL from <code>X:\...\jre7\bin\client\jvm.dll</code>. However, the Windows
			 * 64-bit JRE only has a <em>server</em> JVM available, resulting in the "correct" way
			 * of locating the DLL to fail with a "Module not found" Win32 error.
			 *
			 * The workaround is to, when loading the DLL as stated in the registry fails, replace
			 * the "<code>\client\</code>" substring with "<code>\server\</code>" and to retry
			 * loading the DLL.
			 */
			size_t substringIndex = jreRuntimeLibPath.rfind(_T("\\client\\"));
			if(substringIndex == string::npos)
			{
				throw e;

			}
			else
			{
				return importCreateJavaVMFromDLL(jreRuntimeLibPath.replace(substringIndex, 8,
							_T("\\server\\")));

			}

		}
		else
		{
			throw e;

		}

	}

} // jvminvoke::importCreateJavaVMFromDefaultDLL

/* ********************************************************************************************** */
jvminvoke::JNI_CreateJavaVM_f jvminvoke::importCreateJavaVMFromDLL(const string jvmDLLPath)
{
	HMODULE jvmDLL = LoadLibrary(jvmDLLPath.c_str());
	if(jvmDLL == NULL)
	{
		throw RuntimeException("Failed to load JVM DLL from \"" +
				std::string().assign(jvmDLLPath.begin(), jvmDLLPath.end()) + "\"", GetLastError());

	}

	JNI_CreateJavaVM_f JNI_CreateJavaVM =
		reinterpret_cast<JNI_CreateJavaVM_f>(GetProcAddress(jvmDLL, "JNI_CreateJavaVM"));
	if(JNI_CreateJavaVM == NULL)
	{
		throw RuntimeException("Failed to locate JNI_CreateJavaVM in \"" +
				std::string().assign(jvmDLLPath.begin(), jvmDLLPath.end()) + "\"", GetLastError());

	}

	return JNI_CreateJavaVM;

} // jvminvoke::importCreateJavaVMFromDLL

/* ********************************************************************************************** */
// vim: set ts=4 sw=4 noet:
