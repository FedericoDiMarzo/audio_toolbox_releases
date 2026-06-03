#pragma once

/**
 * @brief Macro to disable copy and move assignment constructors and operators.
 *
 * @param CLS Class name.
 */
#define AT_DISABLE_COPY_AND_MOVE_M(CLS)                                                                                \
    CLS(const CLS&) = delete;                                                                                          \
    CLS(CLS&&) = delete;                                                                                               \
    CLS& operator=(const CLS&) = delete;                                                                               \
    CLS& operator=(CLS&&) = delete;

/**
 * @brief Macro to disable copy assignment constructors and operators.
 *
 * @param CLS Class name.
 */
#define AT_DISABLE_COPY_M(CLS)                                                                                         \
    CLS(const CLS&) = delete;                                                                                          \
    CLS& operator=(const CLS&) = delete;

/**
 * @brief Throw an exception if enabled.
 *
 * @param EXCEPTION_TYPE Exception type.
 * @param MSG Exception message.
 */
#if AT_NO_EXCEPTIONS == 1
    #define AT_THROW_M(EX_TYPE, MSG) std::terminate();
#else
    #define AT_THROW_M(EXCEPTION_TYPE, MSG) throw EXCEPTION_TYPE(MSG);
#endif
