/**
 * @file Slave.ino
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
#include "esp_err.h"

/* --------------------- Definitions and static variables ------------------ */

#define RX_TASK_PRIO 8  // Receiving task priority
#define TX_GPIO_NUM  gpio_num_t(21)
#define RX_GPIO_NUM  gpio_num_t(22)

static const twai_general_config_t g_config =
    TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
// static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

#define ID_SLAVE_RESPONSE 0x0B1

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
        rx_msg.identifier = ID_SLAVE_RESPONSE;
        twai_transmit(&rx_msg, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    Serial.println("Driver installed");
    ESP_ERROR_CHECK(twai_start());
    xTaskCreatePinnedToCore(twai_receive_task, "twai_receive_task", 4096, NULL,
                            RX_TASK_PRIO, NULL, tskNO_AFFINITY);
}

void loop() {
}