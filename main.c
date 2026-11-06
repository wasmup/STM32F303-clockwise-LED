#include <stdint.h>

#define REG32(addr) (*(volatile uint32_t *)(addr))

#define RCC_AHBENR   REG32(0x40021014u)
#define GPIOE_MODER  REG32(0x48001000u)
#define GPIOE_BSRR   REG32(0x48001018u)


#define SYST_CSR REG32(0xE000E010u)
#define SYST_RVR REG32(0xE000E014u)
#define SYST_CVR REG32(0xE000E018u)

#define CPU_HZ 8000000u

void delay_ms(uint32_t ms) {
    // SysTick clock = CPU clock (8 MHz).
    SYST_RVR = (CPU_HZ / 1000u) - 1u;
    SYST_CVR = 0;
    SYST_CSR = (1u << 2) | (1u << 0);

    while (ms--) {
        // Wait for COUNTFLAG.
        while (!(SYST_CSR & (1u << 16))) {
        }
    }

    SYST_CSR = 0;
}


// Bit 0 = 12 o'clock, clockwise through bit 7.
// STM32F3DISCOVERY: LD3..LD10 on PE9, PE8,
// PE10, PE15, PE12, PE11, PE14, PE13.
static const uint8_t led_pins[8] = {
    13, 14, 15,8, 9, 10, 11, 12,  
};

void LED_Configure(void) {
    // Enable GPIOE clock.
    RCC_AHBENR |= (1u << 21);

    // Configure PE8..PE15 as outputs (MODER = 01).
    GPIOE_MODER =
        (GPIOE_MODER & ~0xFFFF0000u) |
        0x55550000u;

    // Turn all LEDs off.
    GPIOE_BSRR = 0xFF000000u;
}

void LED_SetByte(uint8_t value) {
    uint32_t set = 0;
    uint32_t reset = 0;

    for (unsigned bit = 0; bit < 8; ++bit) {
        uint32_t pin = 1u << led_pins[bit];

        if (value & (1u << bit))
            set |= pin;
        else
            reset |= pin;
    }

    // Atomic GPIO update.
    GPIOE_BSRR = set | (reset << 16);
}

int main(void) {
    LED_Configure();

    for (;;) {
        for (uint8_t bit = 0; bit < 8; ++bit) {
            LED_SetByte((uint8_t)(1u << bit));
            delay_ms(100);
        }
    }
}
