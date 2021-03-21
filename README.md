# TDLight
TDLight is a fork of tdlib, focused on memory footprint and performance.

TDLight is 100% compatible with tdlib, if you don't use the sqlite database.

⚠️ Memory Cleanup remove nearly every cached value, so you must use it with caution!
This function is not suggested for GUI clients, because they heavily rely on retrieving cached data!
TDLib developers strongly advise against the use of this feature, since it is not an intended behavior.

## Added features
### Memory cleanup
TDLight can clean itself and release some ram to the OS if you want. Look at **TdApi.OptimizeMemory** in "Modified features" paragraph to see how.

### (Almost) constant memory usage
TDLight if used with care doesn't grow in memory usage with time. Look at **TdApi.OptimizeMemory** in "Modified features" paragraph to see how

![memory usage](info/memory-usage.jpg)

### Custom options
We added some options:
* **disable_minithumbnails** (true/**false**) This setting removes minithumbnails everywhere. It reduces memory usage because tdlib keeps them in RAM
* **disable_document_filenames** (true/**false**) If you don't care about having the original filenames of every file stored in RAM, you can disable them using this option. It reduces memory usage
* **disable_notifications** (true/**false**) In TDLib pending notification updates are stored in ram until you "read" them. This option disables completely notifications and keeps the pending notifications queue empty, reducing memory usage
* **ignore_update_chat_last_message**  (true/**false**) If you don't care about have updateChatLastMessage updates enable this
* **ignore_update_chat_read_inbox**  (true/**false**) If you don't care about have updateChatReadInbox updates enable this
* **ignore_update_user_chat_action**  (true/**false**) If you don't care about have updateUserChatAction updates enable this
* **ignore_server_deletes_and_reads**  (true/**false**) If you don't care about receiving read receipts and remote deletes from other users, enable this, it will reduce memory usage
* **delete_chat_reference_after_seconds** (positive number) During cleanup, free the memory of the chats that have not been touched for more than X seconds
* **delete_user_reference_after_seconds** (positive number) During cleanup, free the memory of the users that have not been touched for more than X seconds
* **delete_file_reference_after_seconds** (positive number) During cleanup, free the memory of the files that have not been touched for more than X seconds
* **experiment_enable_file_reference_cleanup** (**true**/false) During cleanup, free the memory of the file references
* **experiment_enable_chat_access_hash_cleanup** (**true**/false) During cleanup, clean chats and channels access hash
* **get_channel_difference_delay_milliseconds** (**0**) Delay get_channel_difference n milliseconds every ~3000pts (~300msg).
    Don't modify this option unless you have a very large bot that struggles to keep up with start-up updates throughput.

## Custom API functions
### TdApi.OptimizeMemory
This method is used to optimize the memory usage, but it must be used carefully.
It removes almost all cached values and releases the memory back to the OS.

Removing cached values can cause problems if you don't take the following precautions:
  1. Before calling *TdApi.OptimizeMemory* you must:
      1. Read all the pending updates to empty the pending updates queue.
      2. Disable internet connection using *TdApi.SetNetworkType(TdApi.NetworkTypeNone)*
  2. Call *TdApi.OptimizeMemory*
  3. After calling *TdApi.OptimizeMemory* you must:
      1. **NOT** use the old file ids because they have been deleted! (Example: If you receive the file 12 after OptimizeMemory is not the same file 12 that you received before *TdApi.OptimizeMemory*, because the id 12 has been reused)
      2. Re-enable internet connection using *TdApi.SetNetworkType(TdApi.NetworkTypeOther)*

### TdApi.GetMemoryStatistics
This method is used to read the size of all the biggest data maps inside tdlib implementation.
The output contains a string that can be parsed as a JSON.

## Removed features
### Local databases encryption
Local databases are no longer encrypted and deleted data is no longer overwritten with zeroes. This reduces IOPS and helps TDLight preserving SSDs life.
### Local text indicization
TDLight removed completely local text indicization, so if you search for some text it will search it through telegram servers.

## Other reccomended options
* Options:
    * ignore_inline_thumbnails: true
    * disable_top_chats: true
    * ignore_platform_restrictions: true
    * ignore_sensitive_content_restrictions: true
* Disable all the databases (messages_db, users_db, files_db)


-----


The following text is the classic tdlib readme:

# TDLib

TDLib (Telegram Database library) is a cross-platform library for building [Telegram](https://telegram.org) clients. It can be easily used from almost any programming language.

## Table of Contents
- [Features](#features)
- [Examples and documentation](#usage)
- [Dependencies](#dependencies)
- [Building](#building)
- [Installing dependencies](#installing-dependencies)
- [Using in CMake C++ projects](#using-cxx)
- [Using in Java projects](#using-java)
- [Using in .NET projects](#using-dotnet)
- [Using with other programming languages](#using-json)
- [License](#license)

<a name="features"></a>
## Features

`TDLib` has many advantages. Notably `TDLib` is:

* **Cross-platform**: `TDLib` can be used on Android, iOS, Windows, macOS, Linux, FreeBSD, OpenBSD, NetBSD, Windows Phone, WebAssembly, watchOS, tvOS, Tizen, Cygwin. It should also work on other *nix systems with or without minimal effort.
* **Multilanguage**: `TDLib` can be easily used with any programming language that is able to execute C functions. Additionally it already has native Java (using `JNI`) bindings and .NET (using `C++/CLI` and `C++/CX`) bindings.
* **Easy to use**: `TDLib` takes care of all network implementation details, encryption and local data storage.
* **High-performance**: in the [Telegram Bot API](https://core.telegram.org/bots/api), each `TDLib` instance handles more than 24000 active bots simultaneously.
* **Well-documented**: all `TDLib` API methods and public interfaces are fully documented.
* **Consistent**: `TDLib` guarantees that all updates are delivered in the right order.
* **Reliable**: `TDLib` remains stable on slow and unreliable Internet connections.
* **Secure**: all local data is encrypted using a user-provided encryption key.
* **Fully-asynchronous**: requests to `TDLib` don't block each other or anything else, responses are sent when they are available.

<a name="usage"></a>
## Examples and documentation
See our [Getting Started](https://core.telegram.org/tdlib/getting-started) tutorial for a description of basic TDLib concepts.

Take a look at our [examples](https://github.com/tdlib/td/blob/master/example/README.md#tdlib-usage-and-build-examples).

See a [TDLib build instructions generator](https://tdlib.github.io/td/build.html) for detailed instructions on how to build TDLib.

See description of our [JSON](#using-json), [C++](#using-cxx), [Java](#using-java) and [.NET](#using-dotnet) interfaces.

See the [td_api.tl](https://github.com/tdlib/td/blob/master/td/generate/scheme/td_api.tl) scheme or the automatically generated [HTML documentation](https://core.telegram.org/tdlib/docs/td__api_8h.html)
for a list of all available `TDLib` [methods](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_function.html) and [classes](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_object.html).

<a name="dependencies"></a>
## Dependencies
`TDLib` depends on:

* C++14 compatible compiler (Clang 3.4+, GCC 4.9+, MSVC 19.0+ (Visual Studio 2015+), Intel C++ Compiler 17+)
* OpenSSL
* zlib
* gperf (build only)
* CMake (3.0.2+, build only)
* PHP (optional, for documentation generation)

<a name="building"></a>
## Building

The simplest way to build `TDLib` is to use our [TDLib build instructions generator](https://tdlib.github.io/td/build.html).
You need only to choose your programming language and target operating system to receive complete build instructions.

In general, you need to install all `TDLib` [dependencies](#dependencies) as described in [Installing dependencies](#installing-dependencies).
Then enter directory containing `TDLib` sources and compile them using CMake:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

To build `TDLib` on low memory devices you can run [SplitSource.php](https://github.com/tdlib/td/blob/master/SplitSource.php) script
before compiling main `TDLib` source code and compile only needed targets:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target prepare_cross_compiling
cd ..
php SplitSource.php
cd build
cmake --build . --target tdjson
cmake --build . --target tdjson_static
cd ..
php SplitSource.php --undo
```
In our tests clang 6.0 with libc++ required less than 500 MB of RAM per file and GCC 4.9/6.3 used less than 1 GB of RAM per file.

<a name="installing-dependencies"></a>
### Installing dependencies

<a name="macos"></a>
#### macOS
* Install the latest Xcode command line tools, for example, via `xcode-select --install`.
* Install other [dependencies](#dependencies), for example, using [Homebrew](https://brew.sh):
```
brew install gperf cmake openssl
```
* Build `TDLib` with CMake as explained in [building](#building). You will likely need to manually specify path to the installed OpenSSL to CMake, e.g.,
```
cmake -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl/ ..
```

<a name="windows"></a>
#### Windows
* Download and install Microsoft Visual Studio 2015 or later.
* Download and install [gperf](https://sourceforge.net/projects/gnuwin32/files/gperf/3.0.1/). Add the path to gperf.exe to the PATH environment variable.
* Install [vcpkg](https://github.com/Microsoft/vcpkg#quick-start).
* Run the following commands to install `TDLib` dependencies using vcpkg:
```
cd <path to vcpkg>
.\vcpkg.exe install openssl:x64-windows openssl:x86-windows zlib:x64-windows zlib:x86-windows
```
* Download and install [CMake](https://cmake.org/download/); choose "Add CMake to the system PATH" option while installing.
* Build `TDLib` with CMake as explained in [building](#building), but instead of `cmake -DCMAKE_BUILD_TYPE=Release ..` use
```
cmake -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg>/scripts/buildsystems/vcpkg.cmake ..
```

To build 32-bit/64-bit `TDLib` using MSVC, you will need to additionally specify parameter `-A Win32`/`-A x64` to CMake.
To build `TDLib` in Release mode using MSVC, you will need to additionally specify parameter `--config Release` to the `cmake --build .` command.

<a name="linux"></a>
#### Linux
* Install all [dependencies](#dependencies) using your package manager.

<a name="using-cxx"></a>
## Using in CMake C++ projects
For C++ projects that use CMake, the best approach is to build `TDLib` as part of your project or to install it system-wide.

There are several libraries that you could use in your CMake project:

* Td::TdJson, Td::TdJsonStatic — dynamic and static version of a JSON interface. This has a simple C interface, so it can be easily used with any programming language that is able to execute C functions.
  See [td_json_client](https://core.telegram.org/tdlib/docs/td__json__client_8h.html) and [td_log](https://core.telegram.org/tdlib/docs/td__log_8h.html) documentation for more information.
* Td::TdStatic — static library with C++ interface for general usage.
  See [Client](https://core.telegram.org/tdlib/docs/classtd_1_1_client.html) and [Log](https://core.telegram.org/tdlib/docs/classtd_1_1_log.html) documentation for more information.
* Td::TdCoreStatic — static library with low-level C++ interface intended mostly for internal usage.
  See [ClientActor](https://core.telegram.org/tdlib/docs/classtd_1_1_client_actor.html) and [Log](https://core.telegram.org/tdlib/docs/classtd_1_1_log.html) documentation for more information.

For example, part of your CMakeLists.txt may look like this:
```
add_subdirectory(td)
target_link_libraries(YourTarget PRIVATE Td::TdStatic)
```

Or you could install `TDLib` and then reference it in your CMakeLists.txt like this:
```
find_package(Td 1.7.3 REQUIRED)
target_link_libraries(YourTarget PRIVATE Td::TdStatic)
```
See [example/cpp/CMakeLists.txt](https://github.com/tdlib/td/tree/master/example/cpp/CMakeLists.txt).

<a name="using-java"></a>
## Using in Java projects
`TDLib` provides native Java interface through JNI. To enable it, specify option `-DTD_ENABLE_JNI=ON` to CMake.

See [example/java](https://github.com/tdlib/td/tree/master/example/java) for example of using `TDLib` from Java and detailed build and usage instructions.

<a name="using-dotnet"></a>
## Using in .NET projects
`TDLib` provides native .NET interface through `C++/CLI` and `C++/CX`. To enable it, specify option `-DTD_ENABLE_DOTNET=ON` to CMake.
.NET Core supports `C++/CLI` only since version 3.1 and only on Windows, so if older .NET Core is used or portability is needed, then `TDLib` JSON interface should be used through P/Invoke instead.

See [example/csharp](https://github.com/tdlib/td/tree/master/example/csharp) for example of using `TDLib` from C# and detailed build and usage instructions.
See [example/uwp](https://github.com/tdlib/td/tree/master/example/uwp) for example of using `TDLib` from C# UWP application and detailed build and usage instructions for Visual Studio Extension "TDLib for Universal Windows Platform".

When `TDLib` is built with `TD_ENABLE_DOTNET` option enabled, `C++` documentation is removed from some files. You need to checkout these files to return `C++` documentation back:
```
git checkout td/telegram/Client.h td/telegram/Log.h td/tl/TlObject.h
```

<a name="using-json"></a>
## Using from other programming languages
`TDLib` provides efficient native C++, Java, and .NET interfaces.
But for most use cases we suggest to use the JSON interface, which can be easily used with any programming language that is able to execute C functions.
See [td_json_client](https://core.telegram.org/tdlib/docs/td__json__client_8h.html) and [td_log](https://core.telegram.org/tdlib/docs/td__log_8h.html) documentation for detailed JSON interface description,
the [td_api.tl](https://github.com/tdlib/td/blob/master/td/generate/scheme/td_api.tl) scheme or the automatically generated [HTML documentation](https://core.telegram.org/tdlib/docs/td__api_8h.html) for a list of
all available `TDLib` [methods](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_function.html) and [classes](https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1_object.html).

`TDLib` JSON interface adheres to semantic versioning and versions with the same major version number are binary and backward compatible, but the underlying `TDLib` API can be different for different minor and even patch versions.
If you need to support different `TDLib` versions, then you can use a value of the `version` option to find exact `TDLib` version to use appropriate API methods.

See [example/python/tdjson_example.py](https://github.com/tdlib/td/tree/master/example/python/tdjson_example.py) for an example of such usage.

<a name="license"></a>
## License
`TDLib` is licensed under the terms of the Boost Software License. See [LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt) for more information.
