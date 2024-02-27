/**
 * @file SeltTest.ino
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
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"

/* --------------------- Definitions and static variables ------------------ */

// Example Configurations
#define NO_OF_MSGS    100
#define NO_OF_ITERS   3
#define TX_GPIO_NUM   gpio_num_t(21)
#define RX_GPIO_NUM   gpio_num_t(22)
#define TX_TASK_PRIO  8      // Sending task priority
#define RX_TASK_PRIO  9      // Receiving task priority
#define CTRL_TSK_PRIO 10     // Control task priority
#define MSG_ID        0x555  // 11 bit standard format ID
#define EXAMPLE_TAG   "TWAI Self Test"

// static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
// Filter all other IDs except MSG_ID
static const twai_filter_config_t f_config = {
    .acceptance_code = (uint32_t)(MSG_ID << 21),
    .acceptance_mask = (uint32_t) ~(TWAI_STD_ID_MASK << 21),
    .single_filter   = true};
// Set to NO_ACK mode due to self testing with single module

static const twai_general_config_t g_config =
    TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);

static void twai_transmit_task(void *arg) {
    twai_message_t rx_msg;
    twai_message_t tx_msg = {
        .self = 1, .identifier = MSG_ID, .data_length_code = 1};
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        for (int i = 0; i < NO_OF_MSGS; i++) {
            // Transmit messages using self reception request
            tx_msg.data[0] = i;
            ESP_ERROR_CHECK(twai_transmit(&tx_msg, portMAX_DELAY));
            Serial.printf("Msg Send - Data = %d\r\n", tx_msg.data[0]);
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_ERROR_CHECK(twai_receive(&rx_msg, portMAX_DELAY));
            Serial.printf("Msg received - Data = %d\r\n", rx_msg.data[0]);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}

void setup(void) {
    Serial.begin(115200);
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    Serial.println("Driver installed");
    ESP_ERROR_CHECK(twai_start());
    xTaskCreatePinnedToCore(twai_transmit_task, "twai_transmit_task", 4096,
                            NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);
}

void loop() {
}