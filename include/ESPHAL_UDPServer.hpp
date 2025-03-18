#ifndef ESPHAL_UDP_SERVER_HPP
#define ESPHAL_UDP_SERVER_HPP
#include <stdint.h>
#include <stddef.h>

class ESPHAL_UDPServer {
    public:
        ESPHAL_UDPServer();

        void init();

        /**
         * @brief Receive data from the UDP server
         */
        bool receive_data(uint8_t *data, size_t len);

    private:
    constexpr static char *TAG = "UDP_SERVER";
    int8_t sock = 0;
};

#endif // ESPHAL_UDP_SERVER_HPP