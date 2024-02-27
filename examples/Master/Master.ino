/**
 * @file Master.ino
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

#define TX_TASK_PRIO 8  // Receiving task priority
#define TX_GPIO_NUM  gpio_num_t(21)
#define RX_GPIO_NUM  gpio_num_t(22)

static const twai_general_config_t g_config =
    TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
// static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

#define ID_MASTER_CMD     0x0A1
#define ID_SLAVE_RESPONSE 0x0B1

static const twai_message_t slave_1_on = {.identifier       = ID_MASTER_CMD,
                                          .data_length_code = 8,
                                          .data = {1, 2, 3, 4, 5, 6, 7, 8}};

static const twai_message_t slave_1_off = {.identifier       = ID_MASTER_CMD,
                                           .data_length_code = 8,
                                           .data = {0, 0, 0, 0, 0, 0, 0, 0}};

void sendData(const twai_message_t *msg) {
    twai_transmit(msg, portMAX_DELAY);
    Serial.printf("send cmd on to 0x%02X\r\n", ID_MASTER_CMD);
    Serial.print("send data: ");
    for (int i = 0; i < msg->data_length_code; i++) {
        Serial.printf("0x%02X ", msg->data[i]);
    }
    Serial.println();
}

void receiveData() {
    twai_message_t rx_msg;
    if (twai_receive(&rx_msg, pdMS_TO_TICKS(300)) == ESP_OK) {
        Serial.printf("received identifier: 0x%02X\r\n", rx_msg.identifier);
        // rx_msg.data
        Serial.print("received data: ");
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.printf("0x%02X ", rx_msg.data[i]);
        }
        Serial.println();
    };
}

static void twai_transmit_task(void *arg) {
    while (1) {
        sendData(&slave_1_on);
        receiveData();
        vTaskDelay(pdMS_TO_TICKS(1000));
        sendData(&slave_1_off);
        receiveData();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    Serial.println("Driver installed");
    ESP_ERROR_CHECK(twai_start());
    xTaskCreatePinnedToCore(twai_transmit_task, "twai_transmit_task", 4096,
                            NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);
}

void loop() {
}