/**
 * 最小LED测试程序 - 完全绕过NuttX
 * 用于验证硬件和基础启动流程
 */

#include <stdint.h>

/* RCC寄存器 */
#define RCC_BASE            0x40023800
#define RCC_CR              (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR         (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR            (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + 0x30))

/* RCC_CR 位定义 */
#define RCC_CR_HSION        (1 << 0)
#define RCC_CR_HSIRDY       (1 << 1)
#define RCC_CR_HSEON        (1 << 16)
#define RCC_CR_HSERDY       (1 << 17)
#define RCC_CR_PLLON        (1 << 24)
#define RCC_CR_PLLRDY       (1 << 25)

/* RCC_AHB1ENR 位定义 */
#define RCC_AHB1ENR_GPIOAEN (1 << 0)
#define RCC_AHB1ENR_GPIOCEN (1 << 2)

/* GPIOA寄存器 */
#define GPIOA_BASE          0x40020000
#define GPIOA_MODER         (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_OTYPER        (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_OSPEEDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_PUPDR         (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_ODR           (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA_BSRR          (*(volatile uint32_t *)(GPIOA_BASE + 0x18))

/* GPIOC寄存器 */
#define GPIOC_BASE          0x40020800
#define GPIOC_MODER         (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_OTYPER        (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_OSPEEDR       (*(volatile uint32_t *)(GPIOC_BASE + 0x08))
#define GPIOC_PUPDR         (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIOC_ODR           (*(volatile uint32_t *)(GPIOC_BASE + 0x14))
#define GPIOC_BSRR          (*(volatile uint32_t *)(GPIOC_BASE + 0x18))

/* 延时函数 */
static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm__ volatile ("nop");
    }
}

/* 复位处理函数 */
void Reset_Handler(void) __attribute__((naked, noreturn));
void Reset_Handler(void)
{
    /* PA2保持高电平（常亮），PC3闪烁 */
    
    __asm__ volatile (
        /* 1. 开启GPIOA和GPIOC时钟 */
        "ldr r0, =0x40023830  \n"  /* RCC_AHB1ENR */
        "ldr r1, [r0]         \n"
        "orr r1, r1, #0x05    \n"  /* GPIOAEN(bit 0) + GPIOCEN(bit 2) */
        "str r1, [r0]         \n"
        
        /* 小延时 */
        "movw r2, #1000       \n"
        "1:                   \n"
        "subs r2, r2, #1      \n"
        "bne 1b               \n"
        
        /* 2. 配置PA2为输出 - GPIOA_MODER */
        "ldr r0, =0x40020000  \n"  /* GPIOA_MODER */
        "ldr r1, [r0]         \n"
        "bic r1, r1, #0x30    \n"  /* 清除bits[5:4] (PA2) */
        "orr r1, r1, #0x10    \n"  /* 设置bits[5:4] = 01 (Output) */
        "str r1, [r0]         \n"
        
        /* 3. 配置PC3为输出 - GPIOC_MODER */
        "ldr r0, =0x40020800  \n"  /* GPIOC_MODER */
        "ldr r1, [r0]         \n"
        "bic r1, r1, #0xC0    \n"  /* 清除bits[7:6] (PC3) */
        "orr r1, r1, #0x40    \n"  /* 设置bits[7:6] = 01 (Output) */
        "str r1, [r0]         \n"
        
        /* 4. 设置PA2为高电平（常亮）- GPIOA_BSRR */
        "ldr r0, =0x40020018  \n"  /* GPIOA_BSRR */
        "mov r1, #0x04        \n"  /* bit 2 = PA2 */
        "str r1, [r0]         \n"
        
        /* 5. 无限循环翻转PC3 - GPIOC_ODR */
        "ldr r3, =0x40020814  \n"  /* GPIOC_ODR地址 */
        
        "loop:                \n"
        /* 设置PC3高电平 (bit 3 = 1) */
        "ldr r1, [r3]         \n"
        "orr r1, r1, #0x08    \n"  /* bit 3 */
        "str r1, [r3]         \n"
        
        /* 延时 */
        "movw r2, #0xFFFF     \n"
        "movt r2, #0x000F     \n"  /* 约1M cycles */
        "2:                   \n"
        "subs r2, r2, #1      \n"
        "bne 2b               \n"
        
        /* 清除PC3低电平 (bit 3 = 0) */
        "ldr r1, [r3]         \n"
        "bic r1, r1, #0x08    \n"  /* bit 3 */
        "str r1, [r3]         \n"
        
        /* 延时 */
        "movw r2, #0xFFFF     \n"
        "movt r2, #0x000F     \n"
        "3:                   \n"
        "subs r2, r2, #1      \n"
        "bne 3b               \n"
        
        /* 跳回循环 */
        "b loop               \n"
        
        ::: "r0", "r1", "r2", "r3", "memory"
    );
    
    /* 永远不会到这里 */
    while(1);
}

/* 默认中断处理函数 */
void Default_Handler(void) __attribute__((noreturn));
void Default_Handler(void)
{
    while (1);
}

/* 中断向量表 */
extern uint32_t _estack;  /* 栈顶，由链接脚本定义 */

__attribute__((section(".isr_vector")))
const uint32_t vector_table[] = {
    (uint32_t)&_estack,           /* 0x00: 初始栈指针 */
    (uint32_t)Reset_Handler,      /* 0x04: 复位处理函数 */
    (uint32_t)Default_Handler,    /* 0x08: NMI */
    (uint32_t)Default_Handler,    /* 0x0C: HardFault */
    (uint32_t)Default_Handler,    /* 0x10: MemManage */
    (uint32_t)Default_Handler,    /* 0x14: BusFault */
    (uint32_t)Default_Handler,    /* 0x18: UsageFault */
    0,                            /* 0x1C: Reserved */
    0,                            /* 0x20: Reserved */
    0,                            /* 0x24: Reserved */
    0,                            /* 0x28: Reserved */
    (uint32_t)Default_Handler,    /* 0x2C: SVCall */
    (uint32_t)Default_Handler,    /* 0x30: Debug Monitor */
    0,                            /* 0x34: Reserved */
    (uint32_t)Default_Handler,    /* 0x38: PendSV */
    (uint32_t)Default_Handler,    /* 0x3C: SysTick */
};
