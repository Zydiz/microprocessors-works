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
    ldr r1, =0x3C00 //setting PB5 and PB6 for output, bits 10, 11, 12 and 13 to 0
    mvns r1, r1
    ands r2, r2, r1
    ldr r1, =0x1400 //setting bits 10 and 12 to 1
    orrs r2, r2, r1
    str r2, [r0]

    ldr r0, =(0x50000000 + 0x00) //GPIOA MODER Register offset
    ldr r2, [r0]
    ldr r1, =0x3C00000 //setting PA11 and PA12 for output, bits 22, 23, 24 and 25 to 0
    mvns r1, r1
    ands r2, r2, r1
    ldr r1, =0x1400000 //setting bits 22 and 24 to 1
    orrs r2, r2, r1
    str r2, [r0]

    LDR r0, =(0x50000400 + 0x14) //GPIOB with ODR offset
    LDR r2, [r0]
    LDR r1, =0x60 //bits 5 and 6 are set to "1"
    ORRS r2, r2, r1
    STR r2, [r0]

    LDR r0, =(0x50000000 + 0x14) //GPIOA with ODR offset
    LDR r2, [r0]
    LDR r1, =0x1800 //bits 11 and 12 are set to "1"
    ORRS r2, r2, r1
    STR r2, [r0]