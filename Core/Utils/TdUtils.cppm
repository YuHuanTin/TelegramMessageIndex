
module;

#include <td/telegram/td_api.h>

export module TdUtils;

export namespace Utils
{
    template<typename TO, typename FROM>
    td::td_api::object_ptr<TO> MoveAs(FROM &&Obj) {
        return td::td_api::move_object_as<TO>(Obj);
    }

    template<typename TYPE, typename... ARGS>
    td::td_api::object_ptr<TYPE> Make(ARGS &&...args) {
        return td::td_api::make_object<TYPE>(std::forward<ARGS>(args)...);
    }

    template<typename TYPE>
    using TdPtr = td::td_api::object_ptr<TYPE>;

    // overloaded
    namespace detail
    {
        template<class... Fs>
        struct overload;

        template<class F>
        struct overload<F> : F {
            explicit overload(F f) : F(f) {}
        };

        template<class F, class... Fs>
        struct overload<F, Fs...>
            : overload<F>, overload<Fs...> {
            explicit overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {}
            using overload<F>::operator();
            using overload<Fs...>::operator();
        };
    } // namespace detail

    template<class... F>
    auto overloaded(F... f) {
        return detail::overload<F...>(f...);
    }

    // overloaded
}
