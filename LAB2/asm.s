.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss

/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss
	 * not necessary for rom only code
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	/* zero bss */
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:
   ldr r0, = (0x40021000 + 0x34)
   ldr r2, [r0]
   movs r1, #0x3 //enable GPIOB and GPIOA clock
   orrs r2, r2, r1
   str r2, [r0]

   ldr r0, =(0x50000400 + 0x00) //GPIOB MODER Register offset
   ldr r1, =0xfFF //setting PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7 for output
   mvns r1, r1
   ands r2, r2, r1
   ldr r1, =0x5555 //setting bits 0, 2, 4, 6, 8, 10, 12, 14.
   orrs r2, r2, r1
   str r2, [r0]

   ldr r0, =(0x50000000 + 0x00) //GPIOA MODER Register offset
   ldr r2, [r0]
   ldr r1, =0x3 //setting PA0 for input
   mvns r1, r1
   ands r2, r2, r1
   str r2, [r0]

T0:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40 //208d55
	bl delay_func

T1:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x8 //Turning on LED4
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T2:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x1C //Turning on LED3, LED4 and LED5
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T3:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x3E //Turning on LED2, LED3, LED4, LED5 and LED6
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T4:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x7F //Turning on LED1, LED2, LED3, LED4, LED5, LED6 and LED7
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T5:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x3E //Turning on LED2, LED3, LED4, LED5 and LED6
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T6:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x1C //Turning on LED3, LED4 and LED5
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func

T7:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x0 //Turning off the leds
    ands r2, r2, r1
    ldr r1, =0x8 //Turning on LED4
    orrs r2, r2, r1
    str r2, [r3]

    ldr r4, =0x4C4B40
	bl delay_func
	b T0

delay_func:
	push {lr}
    ldr r0, =(0x50000000 + 0x10)
    ldr r2, [r0]
    ldr r1, =0x00000001
    mvns r3, r1
    orrs r2, r2, r3
    cmp r3, r2
    bEQ eq

    push {lr}
    bl reset_state
    pop {r5}
    mov lr, r5

    eq:
    subs r4, #0x1
    bNE delay_func
	pop {pc}

reset_state:
    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x80 //Turning on LED8
    orrs r2, r2, r1
    str r2, [r3]

    ldr r0, =(0x50000000 + 0x10) //check if button is pressed
    ldr r2, [r0]
    ldr r1, =0x00000001
    mvns r3, r1
    orrs r2, r2, r3
    cmp r3, r2
    bEQ reset_state //if button is not pressed loop STOCK: BEQ

    ldr r3, =(0x50000400 + 0x14)
    ldr r2, [r3]
    ldr r1, =0x7F //Turning off LED8
    ands r2, r2, r1
    str r2, [r3]

	bx lr //if button is pressed return to last adress
