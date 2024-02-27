/**
 * @file Listen.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief
 * @version 0.1
 * @date 2024-02-27
 *
 *
 * @Hardwares: M5Core + M5Unit-CAN
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 */
#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"

/* --------------------- Definitions and static variables ------------------ */
// Example Configuration
#define RX_TASK_PRIO 9
#define TX_GPIO_NUM  gpio_num_t(21)
#define RX_GPIO_NUM  gpio_num_t(22)
#define EXAMPLE_TAG  "TWAI Listen Only"

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
// static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
// Set TX queue length to 0 due to listen only mode
static const twai_general_config_t g_config = {
    .mode           = TWAI_MODE_LISTEN_ONLY,
    .tx_io          = TX_GPIO_NUM,
    .rx_io          = RX_GPIO_NUM,
    .clkout_io      = TWAI_IO_UNUSED,
    .bus_off_io     = TWAI_IO_UNUSED,
    .tx_queue_len   = 0,
    .rx_queue_len   = 5,
    .alerts_enabled = TWAI_ALERT_NONE,
    .clkout_divider = 0};

/* --------------------------- Tasks and Functions -------------------------- */

static void twai_receive_task(void *arg) {
    twai_message_t rx_msg;

    while (1) {
        twai_receive(&rx_msg, portMAX_DELAY);

        Serial.printf("received identifier: 0x%02X\r\n", rx_msg.identifier);
        // rx_msg.data
        Serial.print("received data: ");
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.printf("0x%02X ", rx_msg.data[i]);
        }
        Serial.println();
    }

    vTaskDelete(NULL);
}

void setup(void) {
    Serial.begin(115200);
    // Install and start TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(EXAMPLE_TAG, "Driver started");

    xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL,
                            RX_TASK_PRIO, NULL, tskNO_AFFINITY);
}

void loop() {
}