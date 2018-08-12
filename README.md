cppkit
======

The goal of cppkit is to provide useful utilities for writing modern C++ based services on the Linux and Mac OSX operating systems.

building
========

cppkit depends on openssl and libuuid. On fedora systems you can install the needed packages like this:

	dnf install openssl-devel libuuid-devel

Build cppkit like this in the standard cmake way:

	mkdir build && cd build && cmake .. && make
