
#ifndef NANO_CAF_2_DEDD159F43C5440295CE21B9760AB502
#define NANO_CAF_2_DEDD159F43C5440295CE21B9760AB502

#include <nano-caf/util/TypeList.h>
#include <type_traits>

namespace nano_caf::detail {
    template<size_t N, typename T>
    struct AggregateFieldsType;

    template<typename T>
    struct AggregateFieldsType<0, T> {
       using Type = TypeList<>;
   
       template <typename F>
       static auto Call(T&, F&& f) noexcept -> decltype(auto) {
          return f();
       }
    };


    template<typename T>
    struct AggregateFieldsType<1, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1] = T{};
            return TypeList<decltype(a1)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1] = obj;
            return f(std::move(a1));
        }
    };

    template<typename T>
    struct AggregateFieldsType<2, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2] = T{};
            return TypeList<decltype(a1),decltype(a2)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2] = obj;
            return f(std::move(a1),std::move(a2));
        }
    };

    template<typename T>
    struct AggregateFieldsType<3, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3));
        }
    };

    template<typename T>
    struct AggregateFieldsType<4, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4));
        }
    };

    template<typename T>
    struct AggregateFieldsType<5, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5)
                            >{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5)
                     );
        }
    };

    template<typename T>
    struct AggregateFieldsType<6, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5,a6] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5),
                            decltype(a6)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5,a6] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5),
                     std::move(a6));
        }
    };

    template<typename T>
    struct AggregateFieldsType<7, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5,a6,a7] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5),
                            decltype(a6),decltype(a7)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5,a6,a7] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5),
                     std::move(a6),std::move(a7));
        }
    };

    template<typename T>
    struct AggregateFieldsType<8, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5,a6,a7,a8] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5),
                            decltype(a6),decltype(a7),decltype(a8)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5,a6,a7,a8] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5),
                     std::move(a6),std::move(a7),std::move(a8));
        }
    };

    template<typename T>
    struct AggregateFieldsType<9, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5,a6,a7,a8,a9] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5),
                            decltype(a6),decltype(a7),decltype(a8),decltype(a9)>{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5,a6,a7,a8,a9] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5),
                     std::move(a6),std::move(a7),std::move(a8),std::move(a9));
        }
    };

    template<typename T>
    struct AggregateFieldsType<10, T> { 
    private:
        constexpr static auto DeduceType() -> auto {
            auto [a1,a2,a3,a4,a5,a6,a7,a8,a9,a10
              ] = T{};
            return TypeList<decltype(a1),decltype(a2),decltype(a3),decltype(a4),decltype(a5),
                            decltype(a6),decltype(a7),decltype(a8),decltype(a9),decltype(a10)
                            >{};
        }    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {
            auto&& [a1,a2,a3,a4,a5,a6,a7,a8,a9,a10
              ] = obj;
            return f(std::move(a1),std::move(a2),std::move(a3),std::move(a4),std::move(a5),
                     std::move(a6),std::move(a7),std::move(a8),std::move(a9),std::move(a10)
                     );
        }
    };

}

#endif
