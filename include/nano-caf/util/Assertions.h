//
// Created by Darwin Yuan on 2022/8/11.
//

#ifndef NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
#define NANO_CAF_2_39CA41233EED45A0A7000DDF69571375

#include <spdlog/spdlog.h>
#include <nano-caf/Status.h>

#define __CAF_ASSERT(expr, result) do {                          \
    if(auto status_ = expr; status_ != nano_caf::Status::OK) {   \
        SPDLOG_ERROR("{} failed, status = {}", #expr, status_);  \
        result;                                                  \
    }                                                            \
} while(0)

#define CAF_ASSERT_R(expr, result) \
__CAF_ASSERT(expr, return result)

#define CAF_ASSERT(expr) \
CAF_ASSERT_R(expr, status_)

#define CAF_ASSERT_VOID(expr) \
CAF_ASSERT_R(expr, )

#define CAF_ASSERT_BOOL(expr) \
CAF_ASSERT_R(expr, false)

#define CAF_ASSERT_NIL(expr) \
CAF_ASSERT_R(expr, nullptr)

#define CAF_ASSERT_WARN(expr) \
__CAF_ASSERT(expr, )

///////////////////////////////////////////////////////////////////////////
#define __CAF_ASSERT_TRUE(expr, result) do {                         \
    if(!(expr)) {                                                    \
        result;                                                      \
    }                                                                \
} while(0)

#define CAF_ASSERT_TRUE_R(expr, result) \
__CAF_ASSERT_TRUE(expr, return result)

#define CAF_ASSERT_TRUE(expr) \
CAF_ASSERT_TRUE_R(expr, nano_caf::Status::FAILED)

#define CAF_ASSERT_TRUE_VOID(expr) \
CAF_ASSERT_TRUE_R(expr, )

#define CAF_ASSERT_TRUE_NIL(expr) \
CAF_ASSERT_TRUE_R(expr, nullptr)

#define CAF_ASSERT_TRUE_BOOL(expr) \
CAF_ASSERT_TRUE_R(expr, false)

#define CAF_ASSERT_TRUE_WARN(expr) \
__CAF_ASSERT_TRUE(expr, )

///////////////////////////////////////////////////////////////////////////
#define __CAF_ASSERT_VALID_PTR_R(p, result) do {                     \
    if((p) == nullptr) {                                             \
        SPDLOG_ERROR("{} = nullptr", #p);                            \
        result;                                                      \
    }                                                                \
} while(0)

#define CAF_ASSERT_VALID_PTR_R(p, result)   \
__CAF_ASSERT_VALID_PTR_R(p, return result)

#define CAF_ASSERT_VALID_PTR(p) \
CAF_ASSERT_VALID_PTR_R(p, nano_caf::Status::NULL_PTR)

#define CAF_ASSERT_VALID_PTR_VOID(p) \
CAF_ASSERT_VALID_PTR_R(p, )

#define CAF_ASSERT_VALID_PTR_NIL(p) \
CAF_ASSERT_VALID_PTR_R(p, nullptr)

#define CAF_ASSERT_VALID_PTR_BOOL(p) \
CAF_ASSERT_VALID_PTR_R(p, false)

#define CAF_ASSERT_VALID_PTR_WARN(p) \
__CAF_ASSERT_VALID_PTR_R(p, )

#define CAF_ASSERT_NEW_PTR(p) \
CAF_ASSERT_VALID_PTR_R(p, nano_caf::Status::OUT_OF_MEM)

#define CAF_ASSERT_NEW_PTR_VOID(p) CAF_ASSERT_VALID_PTR_VOID(p)

#endif //NANO_CAF_2_39CA41233EED45A0A7000DDF69571375
