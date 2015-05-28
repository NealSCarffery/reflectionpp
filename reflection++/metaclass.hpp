#pragma once

#include "visitor_list.hpp"

namespace rpp {

// the base class of meta information containers
template <class Visitors>
struct MetaBase;

template <>
struct MetaBase<VisitorList<>> {
    virtual const char *getName() = 0;

    virtual void visit(void) {};
};

template <class Visitor, class... Args>
struct MetaBase<VisitorList<Visitor, Args...>>: public MetaBase<VisitorList<Args...>> {
    using MetaBase<VisitorList<Args...>>::visit;

    virtual typename Visitor::ReturnValue visit(Visitor &visitor) = 0;
};

// the implementation of MetaBase
template <class Visitors, class Accessor, class Base = MetaBase<Visitors>>
struct MetaImpl;

template <class Accessor, class Base>
struct MetaImpl<
    VisitorList<>, Accessor, Base
>: public Base, Accessor {
    virtual const char *getName() {
        return Accessor::getRealName();
    }
};

template <class Visitor, class... Args, class Accessor, class Base>
struct MetaImpl<
    VisitorList<Visitor, Args...>, Accessor, Base
>: public MetaImpl<VisitorList<Args...>, Accessor, Base> {
    using Base::visit;

    virtual typename Visitor::ReturnValue visit(Visitor &visitor) override {
        return visitor.visit(Accessor::access());
    }
};

}

// ======== Usage example ========

#ifdef RPP_DEBUG

    #include <typeinfo>
    #include <iostream>
    #include "visitor_chain.hpp"
    #include "accessor.hpp"

    namespace rpp {

        struct Visitor4: public VisitorBase<> {
            void visit(int &value) {
                std::cerr << "Visitor4: int, " << value << std::endl;
            }

            void visit(char &value) {
                std::cerr << "Visitor4: char, " << value << std::endl;
                value = 'B';
            }

            void visit(bool &value) {
                std::cerr << "Visitor4: bool, " << value << std::endl;
            }
        };

        struct Visitor5: public VisitorBase<> {
            template <class T>
            void visit(T &value) {
                std::cerr << "Visitor5: " << typeid(T).name() << ", " << value << std::endl;
            }
        };

        struct Accessor1: public AccessorBase<> {
            int value = 0;

            inline const char *getRealName() {
                return "value1";
            }

            int &access() {
                return value;
            }
        };

        struct Accessor2: public AccessorBase<> {
            char value = 'A';

            inline const char *getRealName() {
                return "value2";
            }

            char &access() {
                return value;
            }
        };

        RPP_VISITOR_REG(Visitor4)
        RPP_VISITOR_REG(Visitor5)
        RPP_VISITOR_COLLECT(VisitorAll3)

        static const int test1 = []() {
            MetaImpl<VisitorAll3, Accessor1> meta1;
            MetaImpl<VisitorAll3, Accessor2> meta2;

            Visitor4 v4;
            Visitor5 v5;

            std::cerr << "Meta1: " << meta1.getName() << std::endl;
            meta1.visit(v4);
            meta1.visit(v5);
            std::cerr << "Meta2: " << meta2.getName() << std::endl;
            meta2.visit(v4);
            meta2.visit(v5);

            return 0;
        }();

    }

#endif
