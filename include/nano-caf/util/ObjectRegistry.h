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
            Object() = default;

            auto OnRegistered() noexcept -> void {
                m_registered = true;
            }

            virtual ~Object() = default;

        protected:
            auto DeregisterFrom(ObjectRegistry& registry) noexcept -> void;

        private:
            bool m_registered{};
        };

        using ElemType = std::shared_ptr<Object>;

        auto AddObject(ElemType object) noexcept -> void;
        auto RemoveObject(Object* object) noexcept -> void;

        auto Empty() const noexcept -> bool {
            return m_objects.empty();
        }

    private:
        std::unordered_set<ElemType> m_objects;
    };
}


#endif //NANO_CAF_2_CCD0E7362D7F408BA8AA11FBF93518A1
