#include <stdint.h>
#include "stm32f10x.h"

// Register Definitions
#define RCC_CR          (*((volatile uint32_t *)(RCC_BASE + 0x00)))
#define RCC_CFGR        (*((volatile uint32_t *)(RCC_BASE + 0x04)))
#define FLASH_ACR       (*((volatile uint32_t *)(FLASH_BASE + 0x00)))
#define GPIOA_CRH       (*((volatile uint32_t *)(GPIOA_BASE + 0x04)))

// bit fields
#define RCC_IOPCEN   (1<<4)
#define GPIOC13      (1UL<<13)

#define RCC_APB2ENR   *(volatile uint32_t *)(RCC_BASE   + 0x18)
#define GPIOC_CRH     *(volatile uint32_t *)(GPIOC_BASE + 0x04)
#define GPIOC_ODR     *(volatile uint32_t *)(GPIOC_BASE + 0x0C)

void SystemClock_Config(void) {
    // 1. Enable HSE (8MHz crystal)
    RCC_CR |= (1 << 16);       // HSEON = 1
    while (!(RCC_CR & (1 << 17))); // Wait for HSERDY

    // 2. Configure FLASH latency (2 wait states for 72MHz)
    FLASH_ACR |= (0x02);       // Two wait states

    // 3. Configure PLL (HSE x9 = 72MHz)
    RCC_CFGR |= (0x7 << 18) | (1 << 16); // PLLMUL=9, PLLSRC=HSE

    // 4. Enable PLL
    RCC_CR |= (1 << 24);       // PLLON = 1
    while (!(RCC_CR & (1 << 25))); // Wait for PLLRDY

    // 5. Switch to PLL as system clock
    RCC_CFGR |= (0x02);        // SW = PLL
    while ((RCC_CFGR & (0x03 << 2)) != (0x02 << 2)); // Wait for PLL
}

void ClockOutput_Init(void) {
    // 1. Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // IOPAEN = 1
    
    // 2. Configure PA8 as Alternate Function Push-Pull (50MHz)
    GPIOA_CRH &= ~(0xF << 0);  // Clear PA8 mode bits
    GPIOA_CRH |= (0xB << 0);   // AFIO Push-Pull, 50MHz
	
	//  Configure PA9 as Alternate Function Push-Pull (50MHz)
    GPIOA_CRH &= ~(0xF << 4);  // Clear PA8 mode bits
    GPIOA_CRH |= (0xB << 4);   // AFIO Push-Pull, 50MHz
    
    // 3. Output PLL/2 (36MHz) on PA8
    RCC_CFGR |= (0x4 << 24);   // MCO = PLL/2
}

int main(void) {
    SystemClock_Config();  // Set 72MHz SYSCLK from HSE+PLL
    ClockOutput_Init();    // Route PLL/2 (36MHz) to PA8
	
    while (1) {

        GPIOA->ODR ^= GPIO_ODR_ODR8;
			  GPIOA->ODR ^= GPIO_ODR_ODR9;
        for (int i = 0; i < 500000; i++); // arbitrary delay
				GPIOA->ODR ^= GPIO_ODR_ODR8;
        GPIOA->ODR ^= GPIO_ODR_ODR9;;
        for (int i = 0; i < 500000; i++); // arbitrary delay
    }
}