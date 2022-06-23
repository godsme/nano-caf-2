
import sys

head = '''
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

'''

tail = '''
}

#endif
'''

name = '''
    template<typename T>
    struct AggregateFieldsType<{0}, T> {{ 
    private:
        constexpr static auto DeduceType() -> auto {{
            auto [{1}] = T{{}};
            return TypeList<{2}>{{}};
        }}    
    public:
        using Type = decltype(DeduceType());
    
        template <typename F>
        static auto Call(T& obj, F&& f) noexcept -> decltype(auto) {{
            auto&& [{3}] = obj;
            return f({4});
        }}
    }};
'''

def gen_one(n):
    bindings = ""
    move_bindings = ""
    decltypes = ""
    m = 0
    for i in range(n):
        bindings = bindings + "a{}".format(i+1)
        move_bindings = move_bindings + "std::move(a{})".format(i+1)
        decltypes = decltypes + "decltype(a{})".format(i+1)
        if i < n-1:
            bindings = bindings + ","
            decltypes = decltypes + ","
            move_bindings = move_bindings + ","
        m = m+1
        if m % 10 == 0:
            bindings = bindings + "\n              "
        if m % 5 == 0:
            decltypes = decltypes + "\n                            "
            move_bindings = move_bindings + "\n                     "

    return name.format(n, bindings, decltypes, bindings, move_bindings)

def gen_all(n):
    all = ""
    for i in range(n):
        all = all + gen_one(i+1)

    return all

def main():
    # print command line arguments
    if len(sys.argv) != 3:
        print("specify the num and file")
        exit(-1)

    n = int(sys.argv[1])
    lines = head + gen_all(n) + tail

    file = open(sys.argv[2], "w")
    file.writelines(lines)
    file.close()

if __name__ == "__main__":
    main()