#include "ESPHAL_Wifi.hpp"

/**
 * @file wifi_cmds.c
 * This code taken from:
 * https://github.com/sahil-kale/Wireless-Soil-Moisture-Sensor/blob/main/main/src/wifi_cmds.c
 * Originally taken from: UDP Multicast Ping Example
 *
 */

#include <lwip/netdb.h>
#include <stdint.h>
#include <string.h>

#include "config.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "esp_mac.h"

#define LISTEN_ALL_IF 1
#define ADDR_FAMILY AF_INET

static const char *TAG = "wifi station";

/* FreeRTOS event group to signal when we are connected*/
// TODO: Move this to the class!
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define MAX_RX_BUFFER 1024

// Ignore all missing field initializers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

void ESPHAL_Wifi::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "Access Point Started");
                break;

            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "Access Point Stopped");
                break;

            case WIFI_EVENT_AP_STACONNECTED: {
                wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
                ESP_LOGI(TAG, "Station connected: MAC=" MACSTR ", AID=%d", 
                         MAC2STR(event->mac), event->aid);
                break;
            }

            case WIFI_EVENT_AP_STADISCONNECTED: {
                wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
                ESP_LOGI(TAG, "Station disconnected: MAC=" MACSTR ", AID=%d", 
                         MAC2STR(event->mac), event->aid);
                break;
            }

            default:
                ESP_LOGI(TAG, "Unhandled WiFi event: %ld", event_id);
                break;
        }
    }
}

ESPHAL_Wifi::ESPHAL_Wifi() {}

ESPHAL_Wifi::~ESPHAL_Wifi() {}

void ESPHAL_Wifi::init() {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    netif_interface = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_ap_config = {
        {
            .ssid = WIFI_SSID,     
            .password = WIFI_KEY,  
            .ssid_len = strlen(WIFI_SSID),
            .channel = 6,            // Set WiFi channel (1-11)
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 1,
        },
    };

    // If no password, use open security
    if (strlen(WIFI_KEY) == 0) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi AP started with SSID: %s, Password: %s", 
        wifi_ap_config.ap.ssid, wifi_ap_config.ap.password);

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));


}

void ESPHAL_Wifi::run() {}

#pragma GCC diagnostic pop