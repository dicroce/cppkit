cppkit
======

The goal of cppkit is to provide useful utilities for writing modern C++ based services on the Linux, Mac OSX and mobile operating systems.

building
========

The linux version of cppkit depends on libuuid. On fedora systems you can install the needed packages like this:

	dnf install libuuid-devel

Build cppkit like this in the standard cmake way:

	mkdir build && cd build && cmake .. && make

building for ios
================

Install XCode and the XCode command line utilities packages. Then in the cppkit dir

	mkdir build
	cd build
	cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DPLATFORM=SIMULATOR64
    cmake --build .

	Note: -DPLATFORM=SIMULATOR64
	Other valid settings are:
		OS = Build for iPhoneOS.
		OS64 = Build for arm64 iphoneOS.
		OS64COMBINED = Build for arm64 x86_64 iphoneOS. Combined into FAT STATIC lib (supported on 3.14+ of CMakewith "-G Xcode" argument ONLY)
		SIMULATOR = Build for x86 i386 iphoneOS Simulator.
		SIMULATOR64 = Build for x86_64 iphoneOS Simulator.
		TVOS = Build for arm64 tvOS.
		TVOSCOMBINED = Build for arm64 x86_64 tvOS. Combined into FAT STATIC lib (supported on 3.14+ of CMake with "-G Xcode" argument ONLY)
		SIMULATOR_TVOS = Build for x86_64 tvOS Simulator.
		WATCHOS = Build for armv7k arm64_32 for watchOS.
		WATCHOSCOMBINED = Build for armv7k arm64_32 x86_64 watchOS. Combined into FAT STATIC lib (supported on 3.14+ of CMake with "-G Xcode" argument ONLY)
		SIMULATOR_WATCHOS = Build for x86_64 for watchOS Simulator.

notices
=======

This software uses the following 3rd party code:


FastLZ

	FastLZ - lightning-fast lossless compression library

	Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)
	Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
	Copyright (C) 2005 Ariya Hidayat (ariya@kde.org)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.


JSON for Modern C++

	MIT License 

	Copyright (c) 2013-2018 Niels Lohmann

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.