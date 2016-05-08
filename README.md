
LoggerXX
========

A **modern** C++ logger based on Boost::Format

Copyright © [Gordon "Lee" Morgan](mailto:valk.erie.fod.der+logxx@gmail.com?subject=LogXX) May 2016. This project is released under the [MIT License](license.md)

Why another logging library
---------------------------

* **Portability**

   Traditional logging libraries which use `printf` style formatting strings work well when you use legacy `C` data types;
   `int`, `float`, `char`, pointers, etc. This breaks down for newer types such as `uint64_t` and `long long`.
   Other types such as `size_t` and `off_t` can change size between 32 and 64 bit compiles on the same platform.
   Logging macros should not need conditional compilation to compile cleanly between multiple platforms or targets.

* **Type safety**

   `C++` types such as `std::string` and `std::chrono::time_point` cannot be used with `printf` without conversion.
   User defined types almost never are useable as `printf` without helper functions.
   LogXX can be used with any type that provides a `ostream >>` operator or proveded helper function.

* **Dependencies**

   LogXX only requires two libraries to use, [Boost](http://www.boost.org) and [Date](http://howardhinnant.github.io/date_v2.html).
   Date is a "header only" library and is an external submodule.
   Boost is a very common dependency and is effectively free for a large number of projects.

* **Streaming and Format string APIs**

   LogXX allows the mixing of both format string and streaming log apis simplifying code migration or mixing source code from multiple sources.

* **Functionality**

   LogXX is designed to easily replace simplistic `printf` derived loggers and offer advanced functionality with low overhead

* **Performance**

   LogXX is intended to be enabled in both *debug* and *release* builds with a minimal performance hit.
   This has a number desirable effects:
   1. This will eliminate a number of "unused variable" warnings that typically occur in release builds when return values are logged.
   2. Functions can safely be used in logging macros, classic "nop" macros will remove these calls in release builds.
   3. You can enable a ring buffer that collects all log messages to write into a crash log, regardless of log level.

Design goals
------------

1. A drop in replacement for "C printf" style logging
2. C++ type safe
3. extensible types
4. By module, file, class, function or line log configuration
5. high performance *Always On* logging
6. Ring buffer critical error log
7. Multiple log output sinks


