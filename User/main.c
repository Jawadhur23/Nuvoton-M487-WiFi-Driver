/**************************
  * @file     main.c
  * @version  V1.00
  * @brief    A project template for M031 MCU.
  *
  * SPDX-License-Identifier: Apache-2.0
  * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
***************************/
#include <stdio.h>
#include "NuMicro.h"
#include "timer.h"
#define MAX 255
#define PB_MODE (*(volatile uint32_t *)(0x40004000+0x040))
/*PBMODE reg add */
#define SYS_GPB_MFPL (*(volatile uint32_t*)(0x40000000+0x38))
/*SYS_GPB_MFPL reg add */
#define UART_FUNCSEL (*(volatile uint32_t*)(0x40070000 + 0x30))
/*UARTFUNCSEL reg add */
#define UART_LINE (*(volatile uint32_t*)(0x40070000 + 0x0C))
/*UARTLINE reg add */
#define UART_BAUD (*(volatile uint32_t*)(0x40070000 + 0x24))
/*UARTBAUD reg add */
#define CLK_CLKSEL1 (*(volatile uint32_t*)(0X40000200+0x14))//Both Timer and UART
/*CLKSEL1 reg add */
#define CLK_APBCLK0 (*(volatile uint32_t*)(0x40000200+0x08))
/*APBCLK0 reg add */
#define SYS_GPB_MFPH (*(volatile uint32_t*)(0x40000000+0x3C))
/* reg add */
#define UART_DAT 	(*(volatile uint32_t *)(0x40070000 + 0x00))
// 0 - 7 is DAT and 8 is PARITY
#define SC_INTEN 	(*(volatile uint32_t *)(0x40070000 + 0x04))
// to Enable RDAIEN | 1 = Receive data reach trigger level interrupt Enabled.
#define UART_INTEN 	(*(volatile uint32_t *)(0x40070000 + 0x04))
//RDAIEN (UART_INTEN [0])
#define UART_INTSTS (*(volatile uint32_t *)(0x40070000 + 0x1C))
// to enable| 1 = RDA interrupt flag is generated.
#define UART_FIFO   (*(volatile uint32_t *)(0x40070000 + 0x08))
//When the number of bytes in the receive FIFO equals the RFITL, the RDAIF (UART_INTSTS[0]) will be set (if RDAIEN (UART_INTEN [0]) enabled, and an interrupt will be generated).
#define NVIC_ISER1  (*(volatile uint32_t *)(0xE000E100 + 0x04))
// Interrupt
#define TIMER0_CMP (*(volatile uint32_t *)(0x40050000 + 0x04))
// Timer0 Comparator Register
#define TIMER0_INTSTS (*(volatile uint32_t *)(0x40050000 + 0x08))
//Timer0 Interrupt Status Register
#define TIMER0_CTL (*(volatile uint32_t *)(0x40050000 + 0x00))
//Timer0 COntrol Register
#define NVIC_ISER0 (*(volatile uint32_t *)(0xE000E100 + 0x00))
//IRQ0 -IRQ108 Set Enable Control Register
#define PAn_PDIO (*(volatile uint32_t *)(0x40004000 + 0x800 + (0x04 * 0)))
//GPIO PA.n Pin Data Input/Output Register
#define PBn_PDIO (*(volatile uint32_t*)(0x40004000 + 0x840 + (0x04 * 1)))
//GPIO PB.n Pin Data Input.Output Register
#define PCn_PDIO (*(volatile uint32_t*)(0x40004000 + 0x880 + (0x04 * 2)))
//GPIO PC.n Pin Data Input.Output Register

#define SYS_GPC_MFPL (*(volatile uint32_t *)(0x40004000+0x040))


uint32_t T1ms=0xFF;
uint32_t T10ms=0xFF;
uint32_t T100ms= 0xFF;
int tx_buffer[MAX];// Declaration of tx_buffer array
int rx_buffer[MAX];
uint32_t counter;
volatile uint32_t timer5msCount = 0;
volatile uint32_t  write_tx_buffer = 0; // Initialization of write_index
volatile uint32_t read_tx_buffer= 0;  // Initialization of read_index
volatile uint32_t write_rx_buffer = 0;
volatile uint32_t read_rx_buffer = 0;
unsigned char available_data;

void SYS_Init()
{
           CLK_CLKSEL1 = (CLK_CLKSEL1 &(~(0X3<<24))) | (0X3<<24); //Setting up an CLK i.e, 24th bit will be assigned with 0x03 - 011
                CLK_APBCLK0 = (CLK_APBCLK0 & (~(1<<16))) | ((1<<16));//UART0CKEN UART0 Clock Enable bit
                CLK_CLKSEL1 = (CLK_CLKSEL1 & (~(0x7<<8))) | (0x7<<8);// Configuring Timer
                CLK_APBCLK0= (CLK_APBCLK0 & (~(0x1<<2))) | (0x1<<2);//configuring Timer

                PB_MODE= (PB_MODE & ~(0x03<<14));
         SYS_GPB_MFPL = ((SYS_GPB_MFPL & (~(1<<28)))|(0x1<<28));
         SYS_GPB_MFPH = ((SYS_GPB_MFPH & (~(0x6<<16)|~(0X6<<20)))|((0x6<<16)|(0X6<<20)));
         SYS_GPC_MFPL = ((SYS_GPC_MFPL & (~(1<<28)))|(0x1<<28));}
void UART0_Init()
{
        UART_FUNCSEL = (UART_FUNCSEL & (~(0x7<<0)));//UART Function
        UART_LINE = (UART_LINE & (~(0X3<<0))) | (0X3<<0);//setting UART word length 8 bit
        UART_BAUD = (UART_BAUD & (~(0X3<<28)));//setting up Baud Rate
        UART_BAUD = (UART_BAUD & (~(255<<0))) | (76<<0);// when 9600 is Baud Rate by applying to the formula it is 76
        //Formula is Baud Rate= CLK/[16*(BRD+2)]
        UART_INTEN= (UART_INTEN &(~(0x1<<0))) |(0x1<<0);//use of RDAIEN bit
        UART_INTSTS=(UART_INTSTS &(~(0x1<<31))) | (0x1<<31);//since use of RDIF we need to Configure
        UART_FIFO = (UART_FIFO & (~(0x15 << 4))) | (0x1 << 4);// SINCE 1'S COMPLEMENT SHOULD BE DONE BY 1111 IS CHANGES TO 0000
        NVIC_ISER1 = (NVIC_ISER1 & (~(0x1<<4))) | (0x1<<4); // it should be calculated ISER0 has 31 bit from 0 and UART0_INT start from 36 bit so we Will be Counting it goes to NVIC_ISER1
        NVIC_ISER1 = (NVIC_ISER1 & (~(0x1<<0))) | (0x1<<0); // it should be calculated ISER0 has 31 bit from 0 and UART0_INT start from 36 bit so we Will be Counting it goes to NVIC_ISER1
        NVIC_ISER0 =(NVIC_ISER0 & (~(0x1<<0))) | (0x1<<0);//The NVIC_ISER0-NVIC_ISER3 registers enable interrupts, and show which interrupts are enabled
}

void uart_write(unsigned char* data, unsigned char length)
{
    int i;
    available_data = (MAX - write_tx_buffer)  + read_tx_buffer;
    if (available_data >= length)
    {
        for (i = 0; i < length; i++)
        {
            tx_buffer[write_tx_buffer] = data[i];
            write_tx_buffer++;
            if (write_tx_buffer >= MAX)
            {
                write_tx_buffer = 0;
            }
        }
    }
}
void Rx_buffer(unsigned char data)
{
    printf("Received data: %c\n", data);
    rx_buffer[write_rx_buffer] = data;
    write_rx_buffer++;
    if (write_rx_buffer >= MAX)
    {
        write_rx_buffer = 0;
    }
}

void UART0_IRQHandler()
{
	if (UART0->INTSTS & UART_INTSTS_RDAIF_Msk)
	    {
	        char c = UART0->DAT;

	        // Process the received character 'c' here
	        printf("%c\n", c);
	    }

}
void read_buffer()
{

    printf("Stored Value: %d\n", rx_buffer[read_rx_buffer]);
    read_rx_buffer++;
    if (read_rx_buffer >= MAX)
    {
        read_rx_buffer = 0;
    }
}
void TMR0_IRQHandler()
             // timer 1 handler
{

        //modeselect
        counter++;
        T1ms=0xFF;
        if(counter%10 ==1 )
        {
                T10ms=0xFF;
        }
        if(counter%100 ==1 )
        {
                T100ms=0xFF;
        }
        if(counter == 10000)
        {
                counter =0;
        }
     TIMER0_INTSTS |= 1;
            // interrupt clear
}
void timer0()
{

        TIMER0_CMP=6009;                                                         // COMPARATOR VALUE FOR 100 MS
                    TIMER0_CTL = (TIMER0_CTL &(~(7<<0))) | (7<<0);                //PRESCALAR VALUE
                    TIMER0_CTL= (TIMER0_CTL &(~(0x3<<27))) | (0x1<<27);               //PERIODIC MODE
                                    TIMER0_CTL= (TIMER0_CTL &(~(0x1<<29))) | (0x1<<29);         //timer interrupt
                                        TIMER0_CTL= (TIMER0_CTL &(~(0x1<<30))) | (0x1<<30);             //start counting
                         NVIC_ISER0 |= 1<<8;                             // TIMER 1 HANDLER CALL
}
int main()
{
	SYS_Init();
	UART0_Init();
	TMR0_IRQHandler();
	uart_write("Jawadhur",8);
    int a,c;
    // Assign a value to 'a' here
    while (1)
    {

    	 if (T1ms==0xFF)
    	     {

    	       //printf("hai");
    	       T1ms=0x44;

    	        if(write_tx_buffer != read_tx_buffer)
    	            	{
    	            		UART0->DAT= tx_buffer[read_tx_buffer];
    	            		read_tx_buffer++;
    	            	}
    	       PAn_PDIO = ~PAn_PDIO;

    	     }

    	     if (T10ms==0xFF)
    	     {
    	          T10ms=0x44;

    	        PBn_PDIO = ~PBn_PDIO;


    	    //  ADC_ADCR |= 1<<11;                    // Triggering interrupt
    	     }
    	     if (T100ms==0xFF)
    	     {
    	        T100ms=0x44;
    	    //mode select
    	         PCn_PDIO = ~PCn_PDIO;
    	     }

}
}
