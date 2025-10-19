// TODO: Implement COMPILER_IS_GNU

#ifdef _MSC_VER
    #define COMPILER_IS_MSVC
#endif

#if defined(COMPILER_IS_GNU)
    // TODO: Implement
    #define __supress_shadow_warning
#elif defined(COMPILER_IS_MSVC)
    #define __supress_shadow_warning __pragma(warning(suppress:4458))
#endif
