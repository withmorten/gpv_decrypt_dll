#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

typedef int32_t ssize_t;
#include "util.h"

typedef unsigned char byte;

#define streq(str1, str2) !strcmp(str1, str2)

void init_gpv_decrypt();
