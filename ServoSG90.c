/*************************************************************************************************************
***************************    Author  : Ehab Magdy Abdullah                      ****************************
***************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  ****************************
***************************    Youtube : https://www.youtube.com/@EhabMagdyy      ****************************
**************************************************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"

void Servo_Task(void *pvParameter);
uint32_t angle_to_duty_cycle(uint8_t angle);

#define TAG             "ServoMotor"

#define SERVO_GPIO      GPIO_NUM_25

void app_main(void)
{
    ESP_LOGI(TAG, "Control Servo Motor SG90 Example");
    xTaskCreate(Servo_Task, "Servo_Task", 2048, NULL, 5, NULL);
}

void Servo_Task(void *pvParameter)
{
    // Configure PWM timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Use Low-Speed mode (better stability)
        .timer_num = LEDC_TIMER_0,            // Timer 0 for PWM
        .duty_resolution = LEDC_TIMER_12_BIT, // 12-bit resolution -> 4096 levels
        .freq_hz = 50,                        // 50 Hz for SG90
        .clk_cfg = LEDC_AUTO_CLK              // Auto select clock source
    };
    ledc_timer_config(&ledc_timer);

    // Configure PWM channel
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,                          // Start with 0 duty cycle
        .gpio_num = SERVO_GPIO,             // GPIO for output
        .speed_mode = LEDC_LOW_SPEED_MODE,  
        .hpoint = 0,                        
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);

    for(;;)
    {
        for(int angle = 0 ; angle <= 180 ; angle += 10)
        {
            ESP_LOGI(TAG, "Moving to %d degrees\n", angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, angle_to_duty_cycle(angle));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        for(int angle = 170 ; angle >= 10 ; angle -= 10)
        {
            ESP_LOGI(TAG, "Moving to %d degrees\n", angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, angle_to_duty_cycle(angle));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

uint32_t angle_to_duty_cycle(uint8_t angle)
{
    if (angle > 180) angle = 180;
    // Map angle (0° -> 0.5ms, 180° -> 2.5ms)
    float pulse_width = 0.5 + (angle / 180.0) * 2.0;
    // Convert pulse width to duty cycle (12-bit resolution, 50Hz)
    uint32_t duty = (pulse_width / 20.0) * 4096;

    return duty;
}