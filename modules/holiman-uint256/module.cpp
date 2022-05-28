#include "module.h"
#include <cryptofuzz/util.h>
#include <cryptofuzz/repository.h>
#include <fuzzing/datasource/id.hpp>
#include <boost/lexical_cast.hpp>

extern "C" {
    #include "cryptofuzz.h"
}

namespace cryptofuzz {
namespace module {

holiman_uint256::holiman_uint256(void) :
    Module("holiman-uint256") {
}

std::string holiman_uint256::getResult(void) const {
    auto res = holiman_uint256_Cryptofuzz_GetResult();
    std::string ret(res);
    free(res);
    return ret;
}

std::optional<nlohmann::json> holiman_uint256::getJsonResult(void) const {
    const auto res = getResult();
    if ( res.empty() ) {
        return std::nullopt;
    }

    try {
        return nlohmann::json::parse(getResult());
    } catch ( std::exception e ) {
        /* Must always parse correctly non-empty strings */
        abort();
    }
}

template <class T> std::optional<T> holiman_uint256::getResultAs(void) const {
    std::optional<T> ret = std::nullopt;

    auto j = getJsonResult();
    if ( j != std::nullopt ) {
        ret = T(*j);
    }

    return ret;
}

static GoSlice toGoSlice(std::string& in) {
    return {in.data(), static_cast<GoInt>(in.size()), static_cast<GoInt>(in.size())};
}

std::optional<component::Bignum> holiman_uint256::OpBignumCalc(operation::BignumCalc& op) {
    if (    op.modulo == std::nullopt ||
            op.modulo->ToTrimmedString() != "115792089237316195423570985008687907853269984665640564039457584007913129639936" ) {
        return std::nullopt;
    }

    auto jsonStr = op.ToJSON().dump();
    holiman_uint256_Cryptofuzz_OpBignumCalc(toGoSlice(jsonStr));

    return getResultAs<component::Bignum>();
}

bool holiman_uint256::SupportsModularBignumCalc(void) const {
    return true;
}

} /* namespace module */
} /* namespace cryptofuzz */
