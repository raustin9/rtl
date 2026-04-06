#ifndef __RTL_DETAIL_PRELUDE
#define __RTL_DETAIL_PRELUDE

// Detect which version of C++ this is being compiled with
#if __cplusplus < 201402L
#error "Minimum compilation target of RTL is C++ 14"
#endif

#if __cplusplus >= 201402L
#define CXX_LIVE_MIN_VERSION_14
#endif

#if __cplusplus >= 201702L
#define CXX_LIVE_MIN_VERSION_17
#endif

#if __cplusplus >= 202002L
#define CXX_LIVE_MIN_VERSION_20
#endif

#if __cplusplus >= 202302L
#define CXX_LIVE_MIN_VERSION_23
#endif

// Conditional keywords/modifiers based on version of C++ being compiled
#ifdef CXX_LIVE_MIN_VERSION_17
#define CXX_17_CONSTEXPR constexpr
#else
#define CXX_17_CONSTEXPR
#endif

#ifdef CXX_LIVE_MIN_VERSION_20
#define CXX_17_CONSTEXPR constexpr
#else
#define CXX_20_CONSTEXPR
#endif

#ifdef CXX_LIVE_MIN_VERSION_23
#define CXX_17_CONSTEXPR constexpr

#else
#define CXX_23_CONSTEXPR
#endif

#endif // __RTL_DETAIL_PRELUDE