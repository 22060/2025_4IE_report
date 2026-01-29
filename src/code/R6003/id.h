#pragma once
#include <stdint.h>

namespace id
{
    // [6bit:boardid]-[5bit:dataid]
    // boardid: 0-63
    // dataid: 0-31
    typedef uint16_t id_t;
    // top priority: 0~9
    constexpr id_t R6005a = 1 << 5;

    // mid priority: 10~49
    constexpr id_t R6003a = 10 << 5;
    constexpr id_t R6003b = 11 << 5;
    constexpr id_t R6003c = 12 << 5;
    // low priority: 50~63

    namespace R6005
    {
        enum
        {
            ID_STATUS0 = 0,
            ID_MU = 1,
        };
    };

};