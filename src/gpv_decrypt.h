#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

typedef int32_t ssize_t;
#include "util.h"

typedef unsigned char byte;

#define streq(str1, str2) !strcmp(str1, str2)

void init_gpv_decrypt();

#define DE2_VERSION 107882

#if DE2_VERSION == 107882
#define CAMPAIGN_CTOR_ADDRESS (0x0000000140EDD430)
#define CAMPAIGN_CTOR_CALLLOC (0x0000000141012769)
#define GPV_DECRYPT_ADDRESS (0x0000000141718DD0)
#define GPV_DECRYPT_CALLLOC (0x0000000140EDDD7D)
#endif
