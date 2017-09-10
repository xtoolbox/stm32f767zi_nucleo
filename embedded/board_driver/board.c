/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rtthread.h>
#include "board.h"
//#include "sram.h"
#include "drv_mpu.h"

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
    /* Enable branch prediction */
    SCB->CCR |= (1 << 18);
    __DSB();

    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    /* tick for HAL Library */
    HAL_IncTick();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}


//void HAL_Delay(__IO uint32_t Delay)
//{
//    rt_thread_delay(Delay);
//}

void HAL_SuspendTick(void)
{
    /* we should not suspend tick */
}

void HAL_ResumeTick(void)
{
    /* we should not resume tick */
}

void cubemx_hal_init(void);
void cubemx_board_init(void);
/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* Configure the MPU attributes as Write Through */
    //mpu_init();

    /* Enable the CPU Cache */
    CPU_CACHE_Enable();
    cubemx_board_init();

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef RT_USING_EXT_SDRAM
    rt_system_heap_init((void*)EXT_SDRAM_BEGIN, (void*)EXT_SDRAM_END);
    sram_init();
#else
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)HEAP_END);
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}

/*@}*/
