//
// Created by Darwin Yuan on 2022/6/22.
//
#include <nano-caf/util/ObjectRegistry.h>

namespace nano_caf {
    auto ObjectRegistry::Object::DeregisterFrom(ObjectRegistry &registry) noexcept -> void {
        if(m_registered) {
            m_registered = false;
            registry.RemoveObject(this);
        }
    }

    auto ObjectRegistry::AddObject(ElemType object) noexcept -> void {
        auto [obj, inserted] = m_objects.insert(std::move(object));
        if(inserted) {
            (*obj)->OnRegistered();
        }
    }

    auto ObjectRegistry::RemoveObject(Object* object) noexcept -> void {
        auto found = std::find_if(m_objects.begin(), m_objects.end(), [target = object](auto&& obj) {
            return obj.get() == target;
        });
        if(found != m_objects.end()) {
            m_objects.erase(found);
        }
    }
}