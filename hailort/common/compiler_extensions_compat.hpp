/**
 * Copyright (c) 2019-2026 Hailo Technologies Ltd. All rights reserved.
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
/**
 * @file compiler_extensions_compat.hpp
 * @brief Defines compiler extensions and preprocessor macros used by Linux compilers.
 **/

#ifndef __COMPILER_EXTENSIONS_COMPAT_HPP__
#define __COMPILER_EXTENSIONS_COMPAT_HPP__


#ifdef __cplusplus
    #define COMPAT__INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#else
    #define COMPAT__INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

#if !defined(UNREFERENCED_PARAMETER)
    #define UNREFERENCED_PARAMETER(param)   \
        do {                                \
            (void)(param);                  \
        } while(0)
#endif

#define LAMBDA_CONSTANT(constant_name) constant_name=constant_name

#endif /* __COMPILER_EXTENSIONS_COMPAT_HPP__ */
