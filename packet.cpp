#include <stdint.h>
#include <cstddef>

struct packet
{
    uint32_t length;
    uint8_t type;
    uint32_t requestID;
    uint8_t data[];
    // Any excess data will be ignored by the server
};
