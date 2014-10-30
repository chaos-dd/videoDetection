
#ifndef DLL_DEFINES_H_H
#define DLL_DEFINES_H_H


#if (defined WIN32 || defined _WIN32)
#ifdef _EXPORTING
#define DLL_EXPORTS __declspec(dllexport)
#else
#define DLL_EXPORTS __declspec(dllimport)
#endif
#else
#define DLL_EXPORTS
#endif








#endif