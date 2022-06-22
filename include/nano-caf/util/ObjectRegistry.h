//
// Created by Darwin Yuan on 2022/6/22.
//

#ifndef NANO_CAF_2_CCD0E7362D7F408BA8AA11FBF93518A1
#define NANO_CAF_2_CCD0E7362D7F408BA8AA11FBF93518A1

#include <unordered_set>
#include <memory>

namespace nano_caf {
    struct ObjectRegistry {
        struct Object {
            virtual auto OnRegistered() noexcept -> void = 0;
            virtual ~Object() = default;
        };

        using ElemType = std::shared_ptr<Object>;

        auto AddObject(ElemType object) noexcept -> void {
            auto [obj, inserted] = m_objects.insert(std::move(object));
            if(inserted) {
                (*obj)->OnRegistered();
            }
        }

        auto RemoveObject(Object* object) noexcept -> void {
            auto found = std::find_if(m_objects.begin(), m_objects.end(), [target = object](auto&& obj) {
                return obj.get() == target;
            });
            if(found != m_objects.end()) {
                m_objects.erase(found);
            }
        }

        auto Empty() const noexcept -> bool {
            return m_objects.empty();
        }

    private:
        std::unordered_set<ElemType> m_objects;
    };
}


#endif //NANO_CAF_2_CCD0E7362D7F408BA8AA11FBF93518A1
