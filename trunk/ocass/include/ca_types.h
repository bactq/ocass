
#ifndef _CA_TYPES_H_
#define _CA_TYPES_H_ 1

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "ca_errno.h"

#if defined(__cplusplus)
#define CA_DECLARE(_type)  __declspec(dllexport) _type __stdcall
#define CA_DECLARE_NOSTD(_type)  __declspec(dllexport) _type
#else
#define CA_DECLARE(_type)  extern "C" __declspec(dllexport) _type __stdcall
#define CA_DECLARE_NOSTD(_type)  extern "C" __declspec(dllexport) _type 
#endif /* defined(__cplusplus) */



#endif /* !defined(_CA_TYPES_H_) */
