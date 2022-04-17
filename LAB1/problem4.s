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
    ldr r0, =(0x40021000 + 0x34)
    ldr r2, [r0]
    movs r1, #0x3 //enable GPIOB and GPIOA clock
    orrs r2, r2, r1
    str r2, [r0]

    ldr r0, =(0x50000400 + 0x00) //GPIOB MODER Register offset
    ldr r2, [r0]
    ldr r1, =0x3000 //setting PB6 input, bit 13 and 12 to 0
    mvns r1, r1
    ands r2, r2, r1
    str r2, [r0]

    ldr r0, =(0x50000000 + 0x00) //GPIOA MODER Register offset
    ldr r2, [r0]
    ldr r1, =0x30000 //setting PA8 for output, bit 17 and 16 to 0
    mvns r1, r1
    ands r2, r2, r1
    ldr r1, =0x10000 //setting bit 16 to 1
    orrs r2, r2, r1
    str r2, [r0]

LED_OFF:
    LDR r0, =(0x50000000 + 0x14)
    LDR r2, [r0]
    LDR r1, =0xfFFFEFF
    ANDS r2, r2, r1 //seting bit 8 low
    STR r2, [r0]

BUTTON:
    LDR r0, =(0x50000400 + 0x10)
    LDR r2, [r0]
    LDR r1, =0x00000040
    MVNS r3, r1
    ORRS r2, r2, r3
    CMP r3, r2
    BEQ LED_OFF

LED_ON:
    LDR r0, =(0x50000000 + 0x14)
    LDR r2, [r0]
    LDR r1, =0x100
    ORRS r2, r2, r1 //seting the 8th bit high
    STR r2, [r0]
    B BUTTON
