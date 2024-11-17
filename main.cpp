#include <iostream>
#include <format>
#include <vector>


struct IntWrapper {
    int64_t value;
};

struct Between {
    int64_t from, to;
};

template<typename CallbacT>
struct Overload {
    Between cond;
    std::decay_t<CallbacT> call;
};


IntWrapper operator""_c(unsigned long long value) {
    return { static_cast<int64_t>(value) };
}

Between operator-(IntWrapper from, IntWrapper to) {
    return {
        .from = from.value,
        .to = to.value
    };
}

auto operator<=>(const Between &lhs, auto &&rhs) {
    return Overload<decltype(rhs)> {
        .cond = lhs,
        .call = rhs
    };
}


template<typename... Funcs>
struct Matcher : Overload<Funcs>... {
    void ForEach(int64_t val) const {
        ([this] <typename Func> (int64_t val) {
            const auto &casted_ref = static_cast<const Overload<Func>&>(*this);
            if (casted_ref.cond.from <= val && val < casted_ref.cond.to) {
                casted_ref.call();
                return true;
            }
            return false;
        }.template operator()<Funcs>(val) || ...);
    }
};


template<typename... Funcs>
void operator+(const uint64_t val, const Matcher<Funcs...> &cases) {
    cases.ForEach(val);
}

#define match(val) val + Matcher



int main() {
    match(10) {
        1_c - 5_c <=> [] { std::println("1-5"); },
        1_c - 12_c <=> [] { std::println("1-12"); }
    };

    return 0;
}
