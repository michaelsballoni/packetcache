#pragma once

#define WIN32_LEAN_AND_MEAN 1

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <atomic>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "zlib.h"

#include "MurmurHash2.h"

