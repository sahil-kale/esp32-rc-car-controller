#include "ESPHAL_UDPServer.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "config.hpp"

ESPHAL_UDPServer::ESPHAL_UDPServer() {
}

void ESPHAL_UDPServer::init()
{
    ESP_LOGI(TAG, "Initializing UDP server...");
    struct sockaddr_in serverAddr;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket: %d", errno);
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(UDP_PORT);
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket: %d", errno);
        close(sock);
        return;
    }

    ESP_LOGI(TAG, "UDP server initialized on port %d", UDP_PORT);

}

bool ESPHAL_UDPServer::receive_data(uint8_t *data, size_t len)
{
    const bool is_connected = sock > 0;
    if (!is_connected) {
        ESP_LOGE(TAG, "Socket not initialized");
        return false;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    ssize_t bytes_received = recvfrom(sock, data, len, MSG_DONTWAIT, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (bytes_received < 0) {
        if (errno == EWOULDBLOCK)
        {
            //ESP_LOGI(TAG, "No New Data Received");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive data: %d", errno);
        }
        return false;
    }
    //ESP_LOGI(TAG, "Received %d bytes from %s:%d", bytes_received, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    return true;
}