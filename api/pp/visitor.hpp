
#pragma once
/// Y-Combinator to allow infinite-recursion with lambdas.
/// The lambda passed as parameter must take an `auto` as first parameter, which is the current
/// function called. It allows recursion in lambdas sinc lambdas cannot be self-referenced in their scope.
template<typename F>
struct fix {
public:
    constexpr fix(F function) : _function(function) {}
    fix(fix const&) = default;
    fix(fix&&) = default;

    template<typename ...Args>
    constexpr decltype(auto) operator()(Args&& ...args) {
        return _function(*this, std::forward<Args>(args)...);
    }
private:
    F _function;
};


namespace lambda_util {

	namespace detail {

		template <typename... lambda_ts>
		struct composer_t;

		template <typename lambda_t>
		struct composer_t<lambda_t> : lambda_t {
			composer_t(const lambda_t& lambda) : lambda_t{ lambda } { }
			composer_t(lambda_t&& lambda) : lambda_t{ std::move(lambda) } { }

			using lambda_t::operator();
		};

		template <typename lambda_t, typename... more_lambda_ts>
		struct composer_t<lambda_t, more_lambda_ts...> : lambda_t, composer_t<more_lambda_ts...> {
			using super_t = composer_t<more_lambda_ts...>;

			template <typename... lambda_ts>
			composer_t(const lambda_t& lambda, lambda_ts&&... more_lambdas) : lambda_t{ lambda }, super_t{ std::forward<lambda_ts>(more_lambdas)... } { }
			template <typename... lambda_ts>
			composer_t(lambda_t&& lambda, lambda_ts&&... more_lambdas) : lambda_t{ std::move(lambda) }, super_t{ std::forward<lambda_ts>(more_lambdas)... } { }

			using lambda_t::operator();
			using super_t::operator();
		};

	} // namespace detail

	template <typename... lambda_ts>
	auto compose(lambda_ts&&... lambdas) {
		return detail::composer_t<std::decay_t<lambda_ts>...>{std::forward<lambda_ts>(lambdas)...};
	}

} // namespace lambda_util


template<typename ...Ts>
struct overloaded;

template<typename T>
struct overloaded<T> {
private:
	T _f;
public:
	overloaded(T&& f) : _f(std::forward<T>(f)) {}

	template<typename ...Args>
	decltype(auto) operator()(Args&& ...args) {
		return _f(std::forward<Args>(args)...);
	}
};

template<typename T, typename ...Ts>
struct overloaded<T, Ts...> : overloaded<T>, overloaded<Ts...> {
public:

	overloaded(T&& f, Ts&& ...fs) : overloaded<T>(std::forward<T>(f)),
		overloaded<Ts...>(std::forward<Ts>(fs)...)
	{}

	using overloaded<T>::operator();
	using overloaded<Ts...>::operator();

};

template<typename TVisitor, typename TVariant>
constexpr auto recursive_visit(TVisitor&& visitor, TVariant&& variant) {
    return std::visit(
        std::forward<TVisitor>(visitor),
        std::forward<TVariant>(variant)
    );

};

template<typename TReturn, typename ...TVisitors>
constexpr auto make_recursive_visitor(TVisitors&& ...visitors) {
	auto selfRecursion = [&visitors...](auto self, auto&& arg)->TReturn {

		return lambda_util::compose ( std::forward<TVisitors>(visitors)... ) (
			[&self](auto&& v)
		{
			return recursive_visit(self, v);
		},
			std::forward<decltype(arg)>(arg)
			);
	};
    return fix<decltype(selfRecursion)>(selfRecursion);
}