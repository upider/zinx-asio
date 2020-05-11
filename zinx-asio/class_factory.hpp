#ifndef CLASS_FACTORY_HPP
#define CLASS_FACTORY_HPP

#include <unordered_map>
#include <typeinfo>
#include <boost/core/demangle.hpp>

namespace zinx_asio {//namespace zinx_asio

template <
    class AbstractProduct,
    typename IdentifyType,
    typename ProductCreator >
class ClassFactory {
    public:
        typedef std::unordered_map<IdentifyType, ProductCreator> AssocMap;

        static ClassFactory& Instance() {
            static ClassFactory instance;
            return instance;
        }
        bool Register(const IdentifyType& id, ProductCreator creator) {
            return assocMap_.emplace(id, creator).second;
        }
        bool Unregister(const IdentifyType& id) {
            return assocMap_.erase(id) == 1;
        }
        AbstractProduct CreateObject(const IdentifyType & id) {
            auto it = assocMap_.find(id);
            if (it != assocMap_.end()) {
                return (it->second)();
            } else {
                return nullptr;
            }
        }

        AssocMap GetClassMap() const {
            return assocMap_;
        }

    private:
        ClassFactory() {}
        ~ClassFactory() {}

    private:
        AssocMap assocMap_;
};

}//namespace zinx_asio
#endif /* CLASS_FACTORY_HPP */
