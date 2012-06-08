/* ********************************************************************************************** */
/*
 * Copyright (C) 2012 Philip Cronje. Distributed under the terms of the BSD 2-Clause License. See
 * LICENSE.txt in the source distribution for the full terms of the license.
 */
/* ********************************************************************************************** */
#pragma once

/* ********************************************************************************************** */
#include <string>

/* ********************************************************************************************** */
namespace jvminvoke
{
	#ifdef _UNICODE
	typedef std::wstring string;
	#else
	typedef std::string string;
	#endif

	const std::string pathListSeparator = ";";

} // namespace jvminvoke

/* ********************************************************************************************** */
// vim: set ts=4 sw=4 noet:
