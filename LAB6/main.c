#include "stm32g0xx.h"
#include "stdio.h"

#define MPU6050_ADDRESS 0x68
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define MPU_PHYSICAL_ADD 0x68
#define WHO_AM_I_REG 0x75

double a;
uint32_t b;

uint32_t count = 0;
uint8_t old_data = 0;

void delay(volatile uint32_t);
void USART2_IRQHandler();
void UartTX(uint8_t);
uint8_t UartRX(void);
void UartInit(uint32_t);
void I2C1_IRQHandler();
int _write(int, char *, int );
void print(char *);
void printchar(uint8_t);
uint8_t ReadI2C(uint8_t, uint8_t);
void write_I2C(uint8_t , uint8_t, uint8_t);
void Multi_ReadI2C(uint8_t, uint8_t, uint8_t *, uint32_t);
void Init_I2C(void);


int main(void)
{
    //System_Init(); // for all initializes
    UartInit(9600);
    Init_I2C();
    uint8_t data[10];
    // single data transfer
    uint16_t data_ = ReadI2C(MPU_PHYSICAL_ADD, WHO_AM_I_REG);
    printf("WHO AM I :%x\r\n", data_);
    data_ = ReadI2C(MPU_PHYSICAL_ADD, MPU6050_PWR_MGMT_1);
    printf("POWER MANAGEMENT:%x\r\n", data_);
    write_I2C(MPU_PHYSICAL_ADD, MPU6050_PWR_MGMT_1, 0x00);
    data_ = ReadI2C(MPU_PHYSICAL_ADD, MPU6050_PWR_MGMT_1);
    printf("POWER MANAGEMENT:%x\r\n", data_);
    while (1)
    {
        delay(10);
        Multi_ReadI2C(MPU_PHYSICAL_ADD, GYRO_XOUT_L, data, 2);
        b = ((uint32_t)data[0]) | ((uint32_t)data[1]);
        a = b / 131.0;
        printf("gyrox_h:%f\r\n", a);
    }
    return 0;
}

// -------Function definitions-------

void delay(volatile uint32_t s)
{
    for (; s > 0; s--);
}

void USART2_IRQHandler()
{
    uint8_t data = (uint8_t)USART2->RDR;
    printchar(data);
}

void UartTX(uint8_t c)
{
    if (c != 0)
    {
        USART2->TDR = (uint16_t)c;
        old_data = c;
    }
}

uint8_t UartRX(void)
{
    uint8_t data = (uint8_t)USART2->RDR;
    if (old_data != data)
        return data;
    else
        return 0;
}

void UartInit(uint32_t baud)
{
    /* GPIOA Set*/
    RCC->IOPENR |= (1 << 0);
    RCC->APBENR1 |= (1 << 17);         //UART2 clock config
    GPIOA->MODER &= ~(3U << 2 * 2);    //PA2 RESET
    GPIOA->MODER |= (2 << 2 * 2);      //PA2 SET AS alternate function
    GPIOA->MODER &= ~(3U << 2 * 3);    //PA3 RESET
    GPIOA->MODER |= (2 << 2 * 3);      //PA3 SET AS alternate function
    GPIOA->AFR[0] &= ~(0xFU << 4 * 2); // AF1... reset
    GPIOA->AFR[0] |= (1 << 4 * 2);     //PA2 AF1
    GPIOA->AFR[0] &= ~(0xFU << 4 * 3); //AF1... reset
    GPIOA->AFR[0] |= (1 << 4 * 3);     //PA3 AF1
    USART2->CR1 = 0;
    USART2->CR1 |= (1 << 3);
    USART2->CR1 |= (1 << 2);
    USART2->BRR = (uint16_t)(SystemCoreClock / baud);
    USART2->CR1 |= (1 << 0); //USART enable
}

void I2C1_IRQHandler()
{
}

int _write(int fd, char *ptr, int len)
{
    (void)fd;
    for (int i = 0; i < len; ++i)
    {
        printchar(ptr[i]);
    }
    return len;
}

void print(char *buf)
{
    int len = 0;
    while (buf[len++] != '\0');
    _write(0, buf, len);
}

void printchar(uint8_t c)
{
    USART2->TDR = (uint16_t)c;
    while (!(USART2->ISR & (1 << 6)));
}

uint8_t ReadI2C(uint8_t DeviceAdd, uint8_t RegisterAdd)
{
    uint8_t ReadedData = 0;
    // Write operation
    I2C1->CR2 = 0;
    I2C1->CR2 |= ((uint32_t)DeviceAdd << 1);
    I2C1->CR2 |= (1U << 16);
    I2C1->CR2 |= (1U << 13);
    while (!(I2C1->ISR & (1 << 1))); // txis
    I2C1->TXDR = (uint32_t)RegisterAdd;
    while (!(I2C1->ISR & (1U << 6)));
    //read operation
    I2C1->CR2 = 0;
    I2C1->CR2 |= ((uint32_t)DeviceAdd << 1); //
    I2C1->CR2 |= (1U << 10);                 //read mode
    I2C1->CR2 |= (1U << 16);                 // NUMBER OF BYTE
    I2C1->CR2 |= (1U << 15);                 // NACK
    I2C1->CR2 |= (1U << 25);                 // AutoendI2C1->CR2 |= (1U << 13);
    while (!(I2C1->ISR & (1U << 2)));
    ReadedData = (uint8_t)I2C1->RXDR;
    return ReadedData;
}

void write_I2C(uint8_t DeviceAdd, uint8_t RegisterAdd, uint8_t data)
{
    // WRITE OPERATION
    I2C1->CR2 = 0;
    I2C1->CR2 |= ((uint32_t)DeviceAdd << 1); //SLAVE ADDRESS
    I2C1->CR2 |= (2U << 16);                 //NUMBER OF BYTE
    I2C1->CR2 |= (1U << 13);                 //AUTOEND
    while (!(I2C1->ISR & (1 << 1))); //TXIS
    I2C1->TXDR = (uint32_t)RegisterAdd;
    while (!(I2C1->ISR & (1 << 1)));
    I2C1->TXDR = (uint32_t)data;
}

void Multi_ReadI2C(uint8_t DeviceAdd, uint8_t RegisterAdd, uint8_t *data, uint32_t num)
{
    // Write operation
    I2C1->CR2 = 0;
    I2C1->CR2 |= ((uint32_t)DeviceAdd << 1);
    I2C1->CR2 |= (1U << 16);
    I2C1->CR2 |= (1U << 13);
    while (!(I2C1->ISR & (1 << 1))); // txis
    //read operation
    I2C1->CR2 = 0;
    I2C1->CR2 |= ((uint32_t)DeviceAdd << 1);
    I2C1->CR2 |= (1U << 10);  //read mode
    I2C1->CR2 |= (num << 16); // number of bytes
    I2C1->CR2 |= (1U << 15);  // NACK
    I2C1->CR2 |= (1U << 25);  // Autoend
    I2C1->CR2 |= (1U << 13);
    for (uint32_t i = 0; i < num; i++)
    {
        while (!(I2C1->ISR & (1U << 2)));
        data[i] = (uint8_t)I2C1->RXDR;
    }
}

void Init_I2C(void)
{
    RCC->IOPENR |= (1U << 1);
    // PB8 - PB9 GPIO Alternate Function I2C pins
    GPIOB->MODER &= ~(3U << 2 * 8);
    GPIOB->MODER |= (2 << 2 * 8);
    GPIOB->OTYPER |= (1U << 8); // AFX selection
    GPIOB->AFR[1] &= ~(0xFU << 4 * 0);
    GPIOB->AFR[1] |= (6U << 4 * 0);
    GPIOB->MODER &= ~(3U << 2 * 9);
    GPIOB->MODER |= (2U << 2 * 9);
    GPIOB->OTYPER |= (1U << 9);
    // AFX selection
    GPIOB->AFR[1] &= ~(0xFU << 4 * 1);
    GPIOB->AFR[1] |= (6U << 4 * 1);
    // Enable I2C1
    RCC->APBENR1 |= (1U << 21);
    I2C1->CR1 = 0;
    I2C1->CR1 |= (1U << 7);
    I2C1->TIMINGR |= (3U << 28);
    I2C1->TIMINGR |= (0x13U << 0);
    I2C1->TIMINGR |= (0xFU << 8);
    I2C1->TIMINGR |= (0x2U << 16);
    I2C1->TIMINGR |= (0x4U << 20);
    I2C1->CR1 |= (1U << 0);
    NVIC_SetPriority(I2C1_IRQn, 1);
    NVIC_EnableIRQ(I2C1_IRQn);
}
