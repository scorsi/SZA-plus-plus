//
// basic_conf.tcc for sza_plus_plus in /home/anthony/repository/sza_plus_plus/basic_conf.tcc
//
// Made by Anthony LECLERC
// Login   <anthony.leclerc@epitech.eu>
//
// Started on  mer. févr. 14 18:22:55 2018 Anthony LECLERC
// Last update mer. févr. 14 18:22:55 2018 Anthony LECLERC
//

#ifndef SZA_PLUS_PLUS_BASIC_CONF_TCC
#define SZA_PLUS_PLUS_BASIC_CONF_TCC

/// Y-Combinator to allow infinite-recursion with lambdas.
/// The lambda passed as parameter must take an `auto` as first parameter, which is the current
/// function called. It allows recursion in lambdas sinc lambdas cannot be self-referenced in their scope.
template<typename F>
struct fix {
public:
    fix(F function) : _function(function) {}
    fix(fix const&) = default;
    fix(fix&&) = default;

    template<typename ...Args>
    decltype(auto) operator()(Args&& ...args) {
        return _function(*this, std::forward<Args>(args)...);
    }
private:
    F _function;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template<typename TVisitor, typename TVariant>
auto recursive_visit(TVisitor&& visitor, TVariant&& variant) {
    return std::visit(
        std::forward<TVisitor>(visitor),
        std::forward<TVariant>(variant)
    );

};

template<typename TReturn, typename ...TVisitors>
auto make_recursive_visitor(TVisitors&& ...visitors)
{
    return fix([&visitors...](auto self, auto&& arg) -> TReturn {

        return overloaded { std::forward<TVisitors>(visitors)... } (
            [&self](auto&& v)
            {
                return recursive_visit(self, v);
            },
            std::forward<decltype(arg)>(arg)
        );

    });
}

#endif /* SZA_PLUS_PLUS_BASIC_CONF_TCC */
