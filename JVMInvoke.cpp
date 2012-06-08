/* ********************************************************************************************** */
/*
 * Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
 * LICENSE.txt in the source distribution for the full terms of the license.
 */
/* ********************************************************************************************** */
#include <string.h>

#include <memory>

#include "JVMInvoke.h"

/* ********************************************************************************************** */
jvminvoke::JavaVM::JavaVM() : javaVM(NULL), jvmDLLPath()
{
} // jvminvoke::JavaVM::JavaVM

/* ********************************************************************************************** */
jvminvoke::JavaVM::JavaVM(const string jvmDLLPath) : javaVM(NULL), jvmDLLPath(jvmDLLPath)
{
} // jvminvoke::JavaVM::JavaVM

/* ********************************************************************************************** */
jvminvoke::JavaVM::~JavaVM()
{
	if(javaVM != NULL)
	{
		javaVM->DestroyJavaVM();

	}

} // jvminvoke::JavaVM::~JavaVM

/* ********************************************************************************************** */
jvminvoke::JavaException jvminvoke::JavaVM::createJavaException(std::string message,
		jthrowable exception)
{
	jclass throwableClass = jniEnvironment->FindClass("java/lang/Throwable");
	jmethodID toString = jniEnvironment->GetMethodID(throwableClass, "toString",
			"()Ljava/lang/String;");
	jstring javaMessageString = static_cast<jstring>(jniEnvironment->CallObjectMethod(exception,
				toString));
	const jchar *javaMessageCString = jniEnvironment->GetStringChars(javaMessageString, NULL);
	std::wstring javaMessage(reinterpret_cast<const wchar_t*>(javaMessageCString));
	jniEnvironment->ReleaseStringChars(javaMessageString, javaMessageCString);

	return JavaException(message, javaMessage);

} // jvminvoke::JavaVM::createJavaException

/* ********************************************************************************************** */
void jvminvoke::JavaVM::createJavaVM(JNI_CreateJavaVM_f JNI_CreateJavaVM,
		std::list<std::string> classPathEntries)
{
	JavaVMInitArgs jvmArguments;
	jvmArguments.ignoreUnrecognized = JNI_FALSE;
	jvmArguments.version = JNI_VERSION_1_6;

	std::auto_ptr<JavaVMOption> jvmOptions;
	std::auto_ptr<char> classPathPointer;
	if(classPathEntries.empty())
	{
		jvmArguments.nOptions = 0;
		jvmArguments.options = NULL;

	}
	else
	{
		std::string classPath("-Djava.class.path=");
		std::list<std::string>::iterator i;
		std::list<std::string>::size_type j;
		std::list<std::string>::size_type classPathEntriesCount = classPathEntries.size();
		for(i = classPathEntries.begin(), j = 0; i != classPathEntries.end(); ++i, ++j)
		{
			classPath.append(*i);
			if(j != classPathEntriesCount - 1)
			{
				classPath.append(pathListSeparator);

			}

		}

		jvmOptions.reset(new JavaVMOption);
		classPathPointer.reset(_strdup(classPath.c_str()));
		jvmOptions->optionString = classPathPointer.get();
		jvmArguments.nOptions = 1;
		jvmArguments.options = jvmOptions.get();

	}

	jint returnCode = JNI_CreateJavaVM(&javaVM, reinterpret_cast<void**>(&jniEnvironment),
			&jvmArguments);
	if(returnCode != JNI_OK)
	{
		throw RuntimeException("Failed to create Java virtual machine", returnCode);

	}

} // jvminvoke::JavaVM::createJavaVM

/* ********************************************************************************************** */
void jvminvoke::JavaVM::executeJAR(const std::string jarPath, const std::string mainClassName)
{
	if(!fileExists(jarPath))
	{
		throw FileNotFoundException("\"" + jarPath + "\" cannot be found or opened");

	}

	if(javaVM != NULL)
	{
		throw Exception("JVM already instantiated");

	}

	createJavaVM(jvmDLLPath.empty() ? importCreateJavaVMFromDefaultDLL() :
			importCreateJavaVMFromDLL(jvmDLLPath), std::list<std::string>(1, jarPath));

	jthrowable javaException;

	jclass mainClass = jniEnvironment->FindClass(mainClassName.c_str());
	javaException = jniEnvironment->ExceptionOccurred();
	if(javaException != NULL)
	{
		throw createJavaException("Failed to load " + mainClassName, javaException);

	}

	jmethodID mainMethod = jniEnvironment->GetStaticMethodID(mainClass, "main",
			"([Ljava/lang/String;)V");
	javaException = jniEnvironment->ExceptionOccurred();
	if(javaException != NULL)
	{
		throw createJavaException("Failed to load main method from " + mainClassName,
				javaException);

	}

	jniEnvironment->CallStaticVoidMethod(mainClass, mainMethod);
	javaException = jniEnvironment->ExceptionOccurred();
	if(javaException != NULL)
	{
		throw createJavaException("An error occurred while executing the JAR's main method",
				javaException);

	}

} // jvminvoke::JavaVM::executeJAR

/* ********************************************************************************************** */
// vim: set ts=4 sw=4 noet:
