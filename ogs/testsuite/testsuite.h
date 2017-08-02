#pragma once
#ifdef _WIN32

#include "osconfig.h"
#include "memory.h"
#include "rehlds/platform_rehlds.h"

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#define TESTSUITE_PROTOCOL_VERSION_MINOR 5
#define TESTSUITE_PROTOCOL_VERSION_MAJOR 0

void TestSuite_Init(const Module* engine, const Module* executable, const AddressRef* funcRefs);

#endif