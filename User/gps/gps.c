#include "gps.h"

// 变量定义
char gps_line_buffer[GPS_BUF_SIZE];
uint8_t gps_line_ready = 0;
uint8_t gps_buf_index = 0;
uint8_t gps_rx_byte = 0;

/**
 * @brief 初始化 GPS 接收
 */
void GPS_Init(void) {
    memset(gps_line_buffer, 0, GPS_BUF_SIZE);
    gps_line_ready = 0;
    gps_buf_index = 0;
    // 启动第一次中断接收
    HAL_UART_Receive_IT(&huart2, &gps_rx_byte, 1);
}

/**
 * @brief 在主循环中调用的解析函数
 */
void GPS_Process(void) {
    if (gps_line_ready) {
        // 仅处理推荐定位信息 (RMC)，可以根据需要添加 GGA 或 GSV
        if (strstr(gps_line_buffer, "$GNRMC") || strstr(gps_line_buffer, "$GPRMC")) {
            // 这里可以添加更复杂的解析逻辑，比如提取经纬度
            // 目前先保持打印，确认数据正确
            printf("GPS_Data: %s", gps_line_buffer);
        }

        // 处理完毕，重置状态
        gps_line_ready = 0;
        gps_buf_index = 0;
    }
}

/**
 * @brief 串口中断回调处理逻辑
 * 将此函数放在 main.c 的 HAL_UART_RxCpltCallback 中调用
 */
void GPS_Callback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) { // 确保是 GPS 连接的串口
        if (gps_buf_index < GPS_BUF_SIZE - 1) {
            gps_line_buffer[gps_buf_index++] = gps_rx_byte;
            
            // 检测到换行符，说明一条指令结束
            if (gps_rx_byte == '\n') {
                gps_line_buffer[gps_buf_index] = '\0';
                gps_line_ready = 1;
            }
        } else {
            gps_buf_index = 0; // 溢出保护
        }

        // 继续开启下一次中断接收
        HAL_UART_Receive_IT(&huart2, &gps_rx_byte, 1);
    }
}
