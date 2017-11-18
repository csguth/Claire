//
//  Integer.hpp
//  claire
//
//  Created by Chrystian Guth on 17/11/17.
//

#ifndef Integer_hpp
#define Integer_hpp

#include <cstdint>
#include <algorithm>
#include <istream>

namespace claire
{
    template <int32_t Min, int32_t Max>
    struct Integer
    {
        Integer():
        value(Min)
        {}
        explicit Integer(int32_t value):
        value(std::min(std::max(Min, value), Max))
        {
            
        }
        int32_t get() const
        {
            return value;
        }
        Integer<Min, Max> set(int32_t value)
        {
            return Integer<Min, Max>(value);
        }
        friend std::istream& operator>>(std::istream& in, Integer& self)
        {
            int32_t input;
            in >> input;
            self = std::move(self).set(input);
            return in;
        }
        static constexpr int32_t min()
        {
            return Min;
        }
        static constexpr int32_t max()
        {
            return Max;
        }
    private:
        int32_t value;
    };
}

#endif /* Integer_hpp */
