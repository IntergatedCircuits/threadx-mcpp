# AzureRTOS (ThreadX) Modern C++ Wrappers

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

**threadx-mcpp** is a C++ wrapper library that allows developers to use the ubiquitous [ThreadX][ThreadX] kernel
while simplifying its use by a new API that closely follows the C++ standard classes.

## Features

* No virtual classes, the wrapper classes accurately encapsulate the underlying data structures
* Public API closely matches the standard C++ thread support library

## Compatibility

* C++11 and above
* Tested with [ThreadX][ThreadX source] 6.1.7, its public API is stable enough to enable the use on a wide range of versions

## Porting

This library requires certain configuration values to be set for correct operation.
Consider the recommended settings for `tx_user.h`:

```C
// required as long as TX_DISABLE_NOTIFY_CALLBACKS is NOT defined
// used for thread termination signalling, see thread::join
#define TX_THREAD_USER_EXTENSION       void* entry_exit_param_;
```

[ThreadX]: https://docs.microsoft.com/en-us/azure/rtos/threadx/
[ThreadX source]: https://github.com/azure-rtos/threadx
