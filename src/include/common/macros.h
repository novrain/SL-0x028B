#if defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define COMMON_CDECL __cdecl
#define COMMON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(COMMON_HIDE_SYMBOLS) && !defined(COMMON_IMPORT_SYMBOLS) && !defined(COMMON_EXPORT_SYMBOLS)
#define COMMON_EXPORT_SYMBOLS
#endif

#if defined(COMMON_HIDE_SYMBOLS)
#define COMMON_PUBLIC(type) type COMMON_STDCALL
#elif defined(COMMON_EXPORT_SYMBOLS)
#define COMMON_PUBLIC(type) __declspec(dllexport) type COMMON_STDCALL
#elif defined(COMMON_IMPORT_SYMBOLS)
#define COMMON_PUBLIC(type) __declspec(dllimport) type COMMON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define COMMON_CDECL
#define COMMON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(COMMON_API_VISIBILITY)
#define COMMON_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define COMMON_PUBLIC(type) type
#endif
#endif