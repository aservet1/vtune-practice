#pragma once

#include<errno.h>
#include<string.h>

#define PANIC(msg) fprintf(stderr, "Error!! | %s | %s\n", msg, strerror(errno)); exit(2);