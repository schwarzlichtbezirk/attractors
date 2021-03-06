// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define  _WIN32_WINNT   0x0501 // _WIN32_WINNT_WINXP
#define NTDDI_VERSION   0x05010300 // NTDDI_WINXPSP3

#include "targetver.h"

// RTL

#include <stdio.h>
#include <tchar.h>

// STL

#include <string>
#include <vector>

#include <chrono>
#include <random>
#include <functional>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <fstream>
#include <iostream>

// TODO: reference additional headers your program requires here
