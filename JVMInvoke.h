/* ********************************************************************************************** */
/*
 * Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
 * LICENSE.txt in the source distribution for the full terms of the license.
 */
/* ********************************************************************************************** */
#pragma once

/* ********************************************************************************************** */
#include <list>
#include <locale>
#include <memory>

#include <JNI.h>

#ifdef _WIN32
#include "JVMInvokeWindows.h"
#else
#error Unsupported platform
#endif

/* ********************************************************************************************** */
namespace jvminvoke
{
	/**
	 * Function pointer corresponding to the <code>JNI_CreateJavaVM</code> function of the Java
	 * Native Interface.
	 */
	typedef jint (JNICALL *JNI_CreateJavaVM_f)(::JavaVM**, void**, void*);

	/**
	 * Base exception class.
	 */
	class Exception
	{
		public:
			Exception(const std::string message) : message_(message)
			{ }

			std::string message() const { return message_; }

		protected:
			std::string message_;

	}; // class Exception

	/**
	 * Exception thrown when a file cannot be found, or is inaccessible.
	 */
	class FileNotFoundException : public Exception
	{
		public:
			FileNotFoundException(const std::string message) : Exception(message)
			{ }

	}; // class FileNotFoundException

	/**
	 * Exception thrown when a Java invocation causes an exception to be raised in the virtual
	 * machine and propagated up towards the native caller.
	 */
	class JavaException : public Exception
	{
		public:
			JavaException(const std::string message, const std::wstring javaMessage) :
				Exception(message), javaMessage_(javaMessage)
			{ }

			std::wstring javaMessage() const { return javaMessage_; }
			/**
			 * Returns a widened variant of the message property.
			 *
			 * @return the same content as the base message() method, except as a std::wstring.
			 */
			std::wstring wideMessage() const
			{
				const size_t length = message_.size();
				const char *cMessage = message_.c_str();
				wchar_t *widened = new wchar_t[length + 1];
				std::auto_ptr<wchar_t> widenedPointer(widened);
				std::locale locale("C");
				mbstate_t state = { 0 };
				const char *nextNarrow;
				wchar_t *nextWide;
				int result = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(locale)
					.in(state, cMessage, cMessage + length, nextNarrow, widened, widened + length,
							nextWide);
				widened[length] = L'\0';
				return (result != std::codecvt_base::error) ? std::wstring(widened) :
					std::wstring();

			}

		private:
			std::wstring javaMessage_;

	}; // class JavaException

	/**
	 * Exception thrown when an error is returned from any external API.
	 */
	class RuntimeException : public Exception
	{
		public:
			RuntimeException(const std::string message, const long returnCode) :
				Exception(message), returnCode_(returnCode)
			{ }

			long returnCode() const { return returnCode_; }

		private:
			long returnCode_;

	}; // class RuntimeException

	/**
	 * Java Virtual Machine wrapper class.
	 */
	class JavaVM
	{
		public:
			/**
			 * Constructs a Java Virtual Machine wrapper instance.
			 *
			 * Initialising a JVM wrapper instance using this constructor will cause the JVM
			 * dynamic-link library to be discovered at run-time using a mechanism appropriate to
			 * the run-time environment and/or platform.
			 */
			JavaVM();
			/**
			 * Constructs a Java Virtual Machine wrapper instance with an explicit JVM
			 * dynamic-link library path.
			 *
			 * @param jvmDLLPath a filesystem path locating the JVM DLL exporting the Invocation API
			 */
			JavaVM(const string jvmDLLPath);
			/**
			 * Shuts down any Java Virtual Machine instance created during the wrapper's lifetime.
			 *
			 * If the invoked JVM started any non-daemon threads, the JVM will remain active until
			 * the last non-daemon thread also terminates.
			 */
			~JavaVM();

			/**
			 * Executes the main method of the specified class, stored in the specified JAR.
			 *
			 * The class specified by @p mainClass must define a <code>static void</code> method
			 * named <code>main</code>, accepting an array of <code>java.lang.String</code> objects.
			 * That is, its method signature must be <code>([Ljava/lang/String;)V</code>.
			 *
			 * @param jarPath   a filesystem path locating the JAR containing the class file
			 *                  referenced by @p mainClass
			 * @param mainClass a fully-qualified Java class name, with components separated by
			 *                  slash characters instead of periods
			 *
			 * @throw FileNotFoundException If the specified JAR could not be found or opened.
			 * @throw JavaException If the Java Virtual Machine raises a Java exception while
			 *                      attempting to locate the specified class or during execution
			 *                      of the JAR's main method.
			 * @throw Exception     If any other error occurs not covered by the more specific
			 *                      exception types.
			 */
			void executeJAR(const std::string jarPath, const std::string mainClass);

		private:
			/**
			 * Utility method to create an instance of JavaException, given a message and a Java
			 * Throwable instance.
			 *
			 * To generate the Java exception's detail message, the <code>toString</code> Java
			 * method is called on the Throwable instance.
			 *
			 * @param  message    the standard Exception message content
			 * @param  jthrowable a reference to a Java Throwable instance
			 * @return a new instance of JavaException containing values derived from the parameters
			 */
			JavaException createJavaException(std::string message, jthrowable exception);
			/**
			 * Creates an instance of the Java Virtual Machine.
			 *
			 * @param JNI_CreateJavaVM a pointer to the <code>JNI_CreateJavaVM</code> function
			 *                         exported by the JVM dynamic-link library
			 * @param classPathEntries an optional list of filesystem paths to directories and/or
			 *                         JARs to be added to the JVM's class path
			 *
			 * @throw RuntimeException If an error occurs while interacting with the Java Native
			 *                         Interface API to create the JVM instance.
			 */
			void createJavaVM(JNI_CreateJavaVM_f JNI_CreateJavaVM,
					std::list<std::string> classPathEntries = std::list<std::string>());

			::JavaVM *javaVM;
			JNIEnv *jniEnvironment;
			string jvmDLLPath;

	}; // class JavaVM

	/**
	 * Determines if a file exists and is accessible.
	 *
	 * This is a platform-specific method.
	 *
	 * @param  path the filesystem path to test
	 * @return <code>true</code> if the file exists, or <code>false</code> if it cannot be found or
	 *         is inaccessible (e.g. due to permissions)
	 */
	bool fileExists(const std::string path);
	/**
	 * Imports the <code>JNI_CreateJavaVM</code> function from the default system Java Run-time
	 * Environment.
	 *
	 * Locating the default JVM dynamic-link library is a platform-specific process. On Windows, the
	 * registry is inspected to find the location of the system JRE, as configured by the Java
	 * installation and configuration process.
	 *
	 * @return a pointer to the <code>JNI_CreateJavaVM</code> function exported by the JVM DLL.
	 *
	 * @throw RuntimeException If an error occurs while interacting with the platform-specific APIs
	 *                         to locate and load the DLL and function.
	 * @throw Exception        If any other error occurs not covered by the more specific exception
	 *                         type.
	 */
	JNI_CreateJavaVM_f importCreateJavaVMFromDefaultDLL();
	/**
	 * Imports the <code>JNI_CreateJavaVM</code> function from the specified Java Virtual Machine
	 * dynamic-link library.
	 *
	 * Importing the function from the JVM DLL is a platform-specific process.
	 *
	 * @param  jvmDLLPath a filesystem path referencing the JVM DLL
	 * @return a pointer to the <code>JNI_CreateJavaVM</code> function exported by the JVM DLL.
	 *
	 * @throw RuntimeException If an error occurs while interacting with the platform-specific APIs
	 *                         to load the DLL and function.
	 */
	JNI_CreateJavaVM_f importCreateJavaVMFromDLL(const string jvmDLLPath);

} // namespace jvminvoke

/* ********************************************************************************************** */
// vim: set ts=4 sw=4 noet:
