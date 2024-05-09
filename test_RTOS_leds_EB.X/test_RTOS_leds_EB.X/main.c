//SWITCHING SEQUENCE AND CLOCK SWITCH   


#pragma config FMIIEN    = OFF            // Ethernet RMII/MII Enable (RMII Enabled)
#pragma config FETHIO    = OFF            // Ethernet I/O Pin Select (Alternate Ethernet I/O)
#pragma config PGL1WAY   = OFF            // Permission Group Lock One Way Configuration (Allow multiple reconfigurations)
#pragma config PMDL1WAY  = OFF            // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY   = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO  = OFF            // USB USBID Selection (Controlled by Port Function)

//DEVCFG2
#pragma config FPLLIDIV  = DIV_2          // System PLL Input Divider (1x Divider)
#pragma config FPLLRNG   = RANGE_8_16_MHZ // System PLL Input Range (13-26 MHz Input)
#pragma config FPLLICLK  = PLL_POSC       // System PLL Input Clock Selection (FRC is input to the System PLL)
#pragma config FPLLMULT  = MUL_63         // System PLL Multiplier (PLL Multiply by 20)
#pragma config FPLLODIV  = DIV_2          // System PLL Output Clock Divider (2x Divider)
#pragma config UPLLFSEL  = FREQ_24MHZ     // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)

//DEVCFG1
#pragma config FNOSC     = SPLL           // Oscillator Selection Bits (System PLL)
#pragma config DMTINTV   = WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN   = OFF            // Secondary Oscillator Enable (Enable SOSC)
#pragma config IESO      = ON             // Internal/External Switch Over (Disabled)
#pragma config POSCMOD   = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC  = OFF            // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config WDTPS     = PS1048576      // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM   = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS    = NORMAL         // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN    = OFF            // Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ = WINSZ_25       // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT    = DMT31          // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN    = OFF            // Deadman Timer Enable (Deadman Timer is disabled)

//DEVCFG0
#pragma config DEBUG     = OFF            // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN    = OFF            // JTAG Enable (JTAG Disabled)
#pragma config ICESEL    = ICS_PGx2       // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)
#pragma config TRCEN     = ON             // Trace Enable (Trace features in the CPU are enabled)
#pragma config BOOTISA   = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON   = OFF_UNLOCKED   // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
#pragma config FSLEEP    = OFF            // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER    = PG_ALL         // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR     = MCLR_NORM      // Soft Master Clear Enable bit (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN  = GAIN_2X        // Secondary Oscillator Gain Control bits (2x gain setting)
#pragma config SOSCBOOST = ON             // Secondary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCGAIN  = GAIN_2X        // Primary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCBOOST = ON             // Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config EJTAGBEN  = NORMAL         // EJTAG Boot (Normal EJTAG functionality)

//DEVCP0
#pragma config CP = OFF                   // Code Protect (Protection Disabled)






#include <xc.h>
#include <cp0defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/attribs.h>
#include "main.h"
#include "EvalBoard1_Ports.h"
#include "Uart.h"

#include <os.h>              //
#include <os_cfg_app.h>      // Archivos necesarios para invocar el RTOS



//inicializacion del sistema operativo, pero no el multitask

void Clicker2_init_RTOS(void) {
    OS_ERR os_err;
    CPU_Init();
    OSInit(&os_err);
}

void Clicker2_set_performance_mode() {
    unsigned int cp0;

    // Unlock Sequence
    asm volatile("di"); // Disable all interrupts
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    PB1DIVbits.PBDIV = 2;
    PB2DIVbits.ON = 1; // Peripheral Bus 2 Output Clock Enable
    PB2DIVbits.PBDIV = 2;


    PB3DIVbits.ON = 1;
    PB3DIVbits.PBDIV = 2;


    // PB4DIV
    PB4DIVbits.ON = 1;
    
    while (!PB4DIVbits.PBDIVRDY); // Wait until it is ready to write to
    PB4DIVbits.PBDIV = 2;

    // PB5DIV
    PB5DIVbits.ON = 1;
    PB5DIVbits.PBDIV = 2;

    // PB7DIV
    PB7DIVbits.ON = 1;
    PB7DIVbits.PBDIV = 0;

    // PB8DIV
    PB8DIVbits.ON = 1;
    PB8DIVbits.PBDIV = 2;


    // Set up caching
    cp0 = _mfc0(16, 0);
    cp0 &= ~0x07;
    cp0 |= 0b011; // K0seg = Cacheable, non-coherent, write - back, write allocate
    _mtc0(16, 0, cp0);


    // PRECON - Set up prefetch
    PRECONbits.PFMSECEN = 0; // Flash SEC Interrupt Enable (Do not generate an interrupt when the PFMSEC bit is set)
    PRECONbits.PREFEN = 3; // Predictive Prefetch Enable (Enable predictive prefetch for any address)
    PRECONbits.PFMWS = 4; // PFM Access Time Defined in Terms ofSYSCLK Wait States(Four wait states are needed when SYSCLK = 252 MHz)


    IPC0bits.CTIP = 2;
    IPC0bits.CTIS = 0;
    _mtc0(11, 0, (252000000ul) / (2 * 1000));
    _mtc0(9, 0, 0);

    IFS0bits.CTIF = 0; // Clear core timer interrupt flag
    IEC0bits.CTIE = 1; // Enable core timerinterrupt


    // Lock Sequence
    SYSKEY = 0x33333333;
}

// Prototipos de funciones
void TASK_LED1(void);
void TASK_LED2(void);
void TASK_LED3(void);
void TASK_LED4(void);
void TASK_LED5(void);
void TASK_LED6(void);
void TASK_MONITOR(void);

// VARIABLES RELATIVAS AL RTOS. Task Control Block y Stacks
// TCB
OS_TCB task_led1_TCB;
// STK
CPU_STK task_led1_STK[1024]; // 1024*4 (32-bit system)


//------------------------------------------------------------------
//VARIABLES para las dos leds adicionales
OS_TCB task_led2_TCB;
// STK
CPU_STK task_led2_STK[1024];


OS_TCB task_led3_TCB;
// STK
CPU_STK task_led3_STK[1024];


OS_TCB task_led4_TCB;
// STK
CPU_STK task_led4_STK[1024];

OS_TCB task_led5_TCB;
CPU_STK task_led5_STK[1024];

OS_TCB task_led6_TCB;
CPU_STK task_led6_STK[1024];


OS_TCB task_monitor_TCB;
// STK
CPU_STK task_monitor_STK[1024];
OS_ERR os_err;



//--------------------------------------------------------------------

unsigned char txdata[100];
unsigned char txdata1[100];
unsigned char txdata2[100];

OS_MUTEX mutext4t5;
OS_SEM semt2t3_one;
OS_SEM semt2t3_two;

void main(void) {

    Clicker2_init_RTOS(); //asociada al RTOS

    Clicker2_set_performance_mode(); // COnfigurar PLL. 16MHZ a 252 MHZ

    EvalBoard1_GpioConfig(); //Define los puertos de la tarjeta

    Uart3_Config(Baud_9600); // ConfIG Uart 3 module (PC communication).


    LED_RED = 0;
    LED_ORANGE = 0;
    LED_GREEN = 0;
    LED_BLUE1 = 0;
    LED_BLUE2 = 0;
    LED_BLUE3 = 0;
    OSMutexCreate(
            (OS_MUTEX *) & mutext4t5,
            (CPU_CHAR *) "Mutex tareas 4 y 5",
            (OS_ERR*) & os_err);
    OSSemCreate(
            (OS_SEM *) & semt2t3_one,
            (CPU_CHAR) * "Semaforo tareas 2 y 3",
            (OS_SEM_CTR) 0,
            (OS_ERR *) & os_err);



    //asocia tarea a la cola o lista de tareas para el sistema operativo
    //asocia un hilo para que lo tenga en cuenta el sistema operativo
    OSTaskCreate(
            (OS_TCB *) & task_led1_TCB,
            (CPU_CHAR *) "Tarea 1. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED1,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led1_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);
    OSTaskCreate(
            (OS_TCB *) & task_led2_TCB,
            (CPU_CHAR *) "Tarea 2. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED2,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led2_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);

    OSTaskCreate(
            (OS_TCB *) & task_led3_TCB,
            (CPU_CHAR *) "Tarea 3. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED3,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led3_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);

    OSTaskCreate(
            (OS_TCB *) & task_led4_TCB,
            (CPU_CHAR *) "Tarea 4. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED4,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led4_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);

    OSTaskCreate(
            (OS_TCB *) & task_led5_TCB,
            (CPU_CHAR *) "Tarea 5. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED5,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led5_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);

    OSTaskCreate(
            (OS_TCB *) & task_led6_TCB,
            (CPU_CHAR *) "Tarea 6. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_LED6,
            (void *) 0,
            (OS_PRIO) 3,
            (CPU_STK *) & task_led6_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 0u, //10*System Tick period
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);


    OSTaskCreate(
            (OS_TCB *) & task_monitor_TCB,
            (CPU_CHAR *) "Tarea Monitor. Control led y mensaje uart",
            (OS_TASK_PTR) TASK_MONITOR,
            (void *) 0,
            (OS_PRIO) 1,
            (CPU_STK *) & task_monitor_STK[0],
            (CPU_STK_SIZE) 0u,
            (CPU_STK_SIZE) 1024u,
            (OS_MSG_QTY) 0u,
            (OS_TICK) 10u,
            (void *) 0,
            (OS_OPT) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            (OS_ERR *) & os_err);



    if (os_err != OS_ERR_NONE) {
        sprintf(txdata, " ERROR en la creaci n de las Tareas \r\n");
        EnviarString(txdata, Uart3);
        while (1);
    } else {
        OSStart(&os_err);

        while (1) {
            sprintf(txdata, " ERROR en la inicializaci n del RTOS\r\n");
            EnviarString(txdata, Uart3);
        }
    }

}

void TASK_LED1(void) {
    OS_ERR os_err;

    while (1) {
        LED_GREEN = !LED_GREEN_Read;
        OSTimeDly(1000, OS_OPT_TIME_DLY, &os_err); //100 ticks del RTOS 
    }
}

void TASK_LED2(void) {
    OS_ERR os_err;
    while (1) {
        sprintf(txdata, "TASK2 entrando en hilo  \r\n");

        OSSemPend(&semt2t3_one,
                0,
                OS_OPT_PEND_BLOCKING,
                0,
                &os_err
                );
        sprintf(txdata, "TASK3 Tras Pend  \r\n");


        LED_RED = !LED_RED_Read;

        OSSemPost(&semt2t3_two,
                OS_OPT_PEND_BLOCKING,
                &os_err
                );
        sprintf(txdata, "TASK3 Tras Primer Post  \r\n");

        OSSemPost(&semt2t3_one,
                OS_OPT_PEND_BLOCKING,
                &os_err
                );
        sprintf(txdata, "TASK3 Tras Segundo Post  \r\n");

        OSTimeDly(500, OS_OPT_TIME_DLY, &os_err);
    }
}

void TASK_LED3(void) {
    OS_ERR os_err;
    while (1) {
        sprintf(txdata, "TASK3 entrando en hilo  \r\n");
        OSSemPend(&semt2t3_two,
                0,
                OS_OPT_PEND_BLOCKING,
                0,
                &os_err
                );
        sprintf(txdata, "TASK3 Tras Pend  \r\n");


        LED_RED = !LED_RED_Read;

        OSSemPost(&semt2t3_one,
                OS_OPT_PEND_BLOCKING,
                &os_err
                );
        OSSemPost(&semt2t3_two,
                OS_OPT_PEND_BLOCKING,
                &os_err
                );
        OSTimeDly(500, OS_OPT_TIME_DLY, &os_err);
    }
}

void TASK_LED4(void) {
    OS_ERR os_err;

    while (1) {
        OSMutexPend(
                (OS_MUTEX *) & mutext4t5,
                (OS_TICK) 0,
                (OS_OPT) OS_OPT_PEND_BLOCKING,
                (CPU_TS *) 0,
                (OS_ERR *) & os_err
                );
        LED_BLUE2 = !LED_BLUE2_Read;

        OSTimeDly(2000, OS_OPT_TIME_DLY, &os_err);

        OSMutexPost(
                (OS_MUTEX *) & mutext4t5,
                (OS_OPT) OS_OPT_PEND_BLOCKING,
                (OS_ERR *) & os_err
                );
    }
}

void TASK_LED5(void) {
    OS_ERR os_err;

    while (1) {
        OSMutexPend(
                (OS_MUTEX *) & mutext4t5,
                (OS_TICK) 0,
                (OS_OPT) OS_OPT_PEND_BLOCKING,
                (CPU_TS *) 0,
                (OS_ERR *) & os_err
                );
        LED_BLUE2 = !LED_BLUE2_Read;

        OSTimeDly(5000, OS_OPT_TIME_DLY, &os_err);

        OSMutexPost(
                (OS_MUTEX *) & mutext4t5,
                (OS_OPT) OS_OPT_PEND_BLOCKING,
                (OS_ERR *) & os_err
                );
    }
}

void TASK_LED6(void) {
    OS_ERR os_err;

    while (1) {
        LED_BLUE2 = !LED_BLUE2_Read;
        OSTimeDly(8000, OS_OPT_TIME_DLY, &os_err);
    }
}

void TASK_MONITOR(void) {

}
