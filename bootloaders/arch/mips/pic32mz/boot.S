
/* This file implements the basic SoC initialization using XC32 compiler */

#include <xc.h>
#include <cp0defs.h>

#if (__XC32_VERSION > 1000) && !defined(CPP_INIT)
#define CPP_INIT
#endif

#if !defined(PIC32_SRS_SET_COUNT)
# warning PIC32_SRS_SET_COUNT not defined on build line
# define PIC32_SRS_SET_COUNT 2
#endif


/* This file contains 32-bit assembly code */
       .set nomips16

        ##################################################################
        # Entry point of the entire application
        ##################################################################
        .section .reset,code,keep
        .align 2
        .set noreorder
        .ent _reset

#if defined(__PIC32_HAS_MICROMIPS) || defined(__mips_micromips)
#if defined (__mips_micromips)
        .set micromips
#endif
_reset:
__pic32_software_reset:
        .word 0x10000003     /* MIPS32:    branch to 0xBFC00010 from here  */
                             /* MicroMIPS: ADDI32 $0, $0, 0x0007 (nop)     */
                             
        .word 0x00000000     /* NOP */
__reset_micromips_isa:
        .set    micromips
        jal     _startup
        nop
        
        .align 2
#if defined(__mips_micromips)
        /* Device not in proper ISA mode */
        .set nomicromips
__reset_switch_isa:
        jal _startup
        nop
#endif

#else

_reset:

#endif  /* __PIC32_HAS_MICROMIPS */

        .align 2
        .end _reset
        .globl _reset
        .globl __pic32_software_reset

        .size _reset, .-_reset

#if defined (__mips_micromips)
        .set micromips
#else 
        .set nomicromips
#endif

        ##################################################################
        # Startup code
        ##################################################################
        .align 2
        .set noreorder
        .ent _startup
_startup:
        ##################################################################
        # If entered because of an NMI, jump to the NMI handler.
        ##################################################################
        mfc0    k0,_CP0_STATUS


#if 0 //      Interrupts are not enabled for the bootloader

        ext     k0,k0,19,1              # Extract NMI bit
        beqz    k0,_no_nmi
        nop
        la      k0,_nmi_handler
        jr      k0
        nop
_no_nmi:
#endif  // NMI not supported

        ##################################################################
        # Initialize Stack Pointer
        #   _stack is initialized by the linker script to point to the
        #    starting location of the stack in DRM
        ##################################################################
        la      sp,_stack

        ##################################################################
        # Initialize Global Pointer
        #   _gp is initialized by the linker script to point to "middle"
        #   of the small variables region
        ##################################################################
        la      gp,_gp

#if (PIC32_SRS_SET_COUNT > 1) 
        ##################################################################
        # Initialize Global Pointer in Shadow Set(s)
        #   The SRSCtl PSS field must be set to the shadow set in which
        #   to initialize the global pointer.  We will initialize
        #   SRSCtl<PSS> to the number of reg sets and work down to set zero.
        #   We write the global pointer to the previous shadow set to 
        #   ensure that on interrupt, the global pointer has been 
        #   initialized.
        ##################################################################
        mfc0    t1,_CP0_SRSCTL          # Read SRSCtl register
        add     t3,t1,zero              # Save off current SRSCtl

#if (PIC32_SRS_SET_COUNT > 2)
        li      t2,(PIC32_SRS_SET_COUNT-1)
#endif

1:      ins     t1,t2,6,4               # Put next shadow set field
        mtc0    t1,_CP0_SRSCTL          # into SRSCtl<PSS>
        ehb                             # Clear hazard before using new SRSCTL
        wrpgpr  gp,gp                   # Set global pointer in PSS

#if (PIC32_SRS_SET_COUNT > 2)
        addiu   t2,t2,-1                # Next lower shadow set
                                        # Loop for all sets
        bne     t2,$0,1b                # Down to zero (normal GPR set)
        nop
#endif

        mtc0    t3,_CP0_SRSCTL          # Restore SRSCtl

#endif /* (PIC32_SRS_SET_COUNT > 1) */

#if 0 // only a call to a stub
        ##################################################################
        # Call the "on reset" procedure
        ##################################################################
        la      t0,_on_reset
        jalr    t0
        nop
#endif // end on reset

        ##################################################################
        # Initialize TLB for fixed mapping to EBI and SQI
        ##################################################################
        .extern __pic32_tlb_init_ebi_sqi
        la      t0,__pic32_tlb_init_ebi_sqi
        jalr    t0
        nop

        ##################################################################
        # Initialize L1 cache register
        ##################################################################
        .extern __pic32_init_cache
        la      t0,__pic32_init_cache
        jalr    t0
        nop

        ##################################################################
        # Clear uninitialized data sections
        ##################################################################
        la      t0,_bss_begin
        la      t1,_bss_end
        b       _bss_check
        nop

_bss_init:
        sw      zero,0x0(t0)
        sw      zero,0x4(t0)
        sw      zero,0x8(t0)
        sw      zero,0xc(t0)
        addu    t0,16
_bss_check:
        bltu    t0,t1,_bss_init
        nop

        ##################################################################
        # Initialize data using the linker-generated .dinit table
        ##################################################################
        .equiv FMT_CLEAR,0
        .equiv FMT_COPY,1
_dinit_init:
        la      t0,_dinit_addr

#define SRC t0
#define DST t1
#define LEN t2
#define FMT t3

0:      lw      DST,0(SRC)
        beqz    DST,9f
        addu    SRC,4
        lw      LEN,0(SRC)
        addu    SRC,4
        lw      FMT,0(SRC)
        beq     FMT,$0,_dinit_clear
        addu    SRC,4

_dinit_copy:
        lbu     t4,0(SRC)
        subu    LEN,1
        addu    SRC,1
        sb      t4,0(DST)
        bne     LEN,$0,_dinit_copy
        addu    DST,1

        b       _dinit_end
        nop

_dinit_clear:
        sb      $0,(DST)
        subu    LEN,1
        bne     LEN,$0,_dinit_clear
        addu    DST,1

_dinit_end:
        addu    SRC,3
        addiu   LEN,$0,0xFFFFFFFC
        and     SRC,LEN,SRC
        lw      DST,0(SRC)
        bne     DST,$0,0b
        nop
9:
 

        ##################################################################
        # Initialize CP0 registers
        ##################################################################
        # Initialize Count register
        ##################################################################
        mtc0    zero,_CP0_COUNT

        ##################################################################
        # Initialize Compare register
        ##################################################################
        li      t2,-1
        mtc0    t2,_CP0_COMPARE

        ##################################################################
        # Initialize EBase register
        ##################################################################
        la      t1,_ebase_address
        mtc0    t1,_CP0_EBASE

        ##################################################################
        # Initialize IntCtl register
        ##################################################################
        la      t1,_vector_spacing
        li      t2,0                    # Clear t2 and
        ins     t2,t1,5,5               # shift value to VS field
        mtc0    t2,_CP0_INTCTL

        ##################################################################
        # Initialize CAUSE registers
        # - Enable counting of Count register <DC = 0>
        # - Use special exception vector <IV = 1>
        # - Clear pending software interrupts <IP1:IP0 = 0>
        ##################################################################
        li      t1,0x00800000
        mtc0    t1,_CP0_CAUSE

        ##################################################################
        # Initialize STATUS register
        # - Access to Coprocessor 0 not allowed in user mode <CU0 = 0>
        # - User mode uses configured endianness <RE = 0>
        # - Preserve Bootstrap Exception vectors <BEV>
        # - Preserve soft reset <SR> and non-maskable interrupt <NMI>
        # - CorExtend enabled based on whether CorExtend User Defined
        #   Instructions have been implemented <CEE = Config<UDI>>
        # - Disable any pending interrups <IM7..IM2 = 0, IM1..IM0 = 0>
        # - Disable hardware interrupts <IPL7:IPL2 = 0>
        # - Base mode is Kernel mode <UM = 0>
        # - Error level is normal <ERL = 0>
        # - Exception level is normal <EXL = 0>
        # - Interrupts are disabled <IE = 0>
        # - DSPr2 ASE is enabled for devices that support it <MX = 1>
        ##################################################################
        mfc0    t0,_CP0_CONFIG
        ext     t1,t0,22,1              # Extract UDI from Config register
        sll     t1,t1,17                # Move UDI to Status.CEE location
        mfc0    t0,_CP0_STATUS
        and     t0,t0,0x00580000        # Preserve SR, NMI, and BEV
        li      t2, 0x01000000          # Set the Status.MX bit to enable DSP
        or      t0,t2,t0
        or      t0,t1,t0                # Include Status.CEE (from UDI)
        mtc0    t0,_CP0_STATUS

        ##################################################################
        # Initialize Status<BEV> for normal exception vectors
        ##################################################################
        mfc0    t0,_CP0_STATUS
        and     t0,t0,0xffbfffff        # Clear BEV
        mtc0    t0,_CP0_STATUS

        ##################################################################
        # Call main. We do this via a thunk in the text section so that
        # a normal jump and link can be used, enabling the startup code
        # to work properly whether main is written in MIPS16 or MIPS32
        # code. I.e., the linker will correctly adjust the JAL to JALX if
        # necessary
        ##################################################################
        and     a0,a0,0
        and     a1,a1,0
        la      t0,_main_entry
        jr      t0
        nop

        .end _startup

#define CFG1_MMUSSHIFT  25

        .weak __pic32_tlb_init_values_count
        .weak __pic32_tlb_init_values_begin
        .weak __pic32_tlb_init_values_end

        .section .tlb.__pic32_tlb_init_ebi_sqi, code, keep
        .set nomips16
        .set noreorder

        .global __pic32_tlb_init_ebi_sqi
        .ent __pic32_tlb_init_ebi_sqi
        
__pic32_tlb_init_ebi_sqi:
        /* Determine if we have a TLB */
        /* Read C0 Config0 */
        mfc0 v1, _CP0_CONFIG
        /* check MT field */
        ext v1, v1, 7, 3
        li a3, 0x1
        bne v1, a3, __done_init_tlb

        /* read C0 Config1 */
        mfc0 v0, _CP0_CONFIG1
        .end __pic32_tlb_init_ebi_sqi

__start_init_tlb:
        /* Extract MMU Size */
        ext v1, v0, CFG1_MMUSSHIFT, 6

        mtc0 zero, _CP0_ENTRYLO0
        mtc0 zero, _CP0_ENTRYLO1
        mtc0 zero, _CP0_PAGEMASK
        mtc0 zero, _CP0_WIRED

        move a0, $0
        lui a0, 0x8000

__next_tlb_entry_pair:
        mtc0 v1, _CP0_INDEX
        mtc0 a0, _CP0_ENTRYHI

        ehb
        tlbwi

        add   a0, (2<<13)
        add   v1, -1
        bne   v1, zero, __next_tlb_entry_pair
        nop

        /* Get the count of the entries in the table from a linker-script symbol */
__count_tlb_init_values:
        lui   a0, %hi(__pic32_tlb_init_values_count)
        ori   a0, %lo(__pic32_tlb_init_values_count)
        
__device_tlb_entries:
        /* Set lowest count (a0) TLB entries as fixed (entry 0 and entry 1) */
        mtc0  a0, _CP0_WIRED
        
        /* Point a1 to the beginning of the linker-script generated table */
        la   a1, __pic32_tlb_init_values_begin
        la   a2, __pic32_tlb_init_values_end
        move a3, zero                               /* index */

        /* Loop through the tables until we have done the correct number of iterations. */
        /* When the current pointer (a1) points to the end of the table (a2) we are done. */
1:
        beq  a1, a2, __done_init_tlb    /* done */

        /* Create one 64MB page table entry consisting of two 32MB physical pages */
        /* 64MB page mask for Entry 0 */
        lui t1,0x07FF                            /* in branch delay slot */
        ori t1,t1,0xE000

        mtc0 t1, _CP0_PAGEMASK

        /* TLB entry index */
        mtc0 a3, _CP0_INDEX
        
        /* Each entry consists of three words */
        lw   t0, 0(a1)                               /* ENTRYHI  */
        lw   t1, 4(a1)                               /* ENTRYLO0 */
        lw   t2, 8(a1)                               /* ENTRYLO1 */

        /* physical base address <31:12>, cacheable (write back), dirty, global set */
        /* First 32MB page in Entrylo0 */
        mtc0 t1, _CP0_ENTRYLO0

        /* Second 32MB page in EntryLo1 */
        mtc0 t2, _CP0_ENTRYLO1

        /*  virtual base address (ASID 0) */
        mtc0 t0, _CP0_ENTRYHI
        ehb
        tlbwi

        /* Next entry */
        add a1, 12         /* Point to next entry (WORD_SIZE * 3) */
        add a3, 1          /* Increment index */
        b   1b
        nop

__done_init_tlb:
        jr ra
        nop


        /* Symbols defined in linker script */
        .weak __pic32_init_cache_program_base_addr
        .weak __pic32_init_cache_data_base_addr

/* Cache Coherency Attributes */
#define _CACHE_WRITEBACK_WRITEALLOCATE      3
#define _CACHE_WRITETHROUGH_WRITEALLOCATE   1
#define _CACHE_WRITETHROUGH_NOWRITEALLOCATE 0
#define _CACHE_DISABLE                      2

#ifndef _CP0_ERRCTL
#define _CP0_ERRCTL                         $26, 0
#endif

#ifndef _CP0_TAGLO
#define _CP0_TAGLO                          $28, 0
#endif

/* Set __PIC32_CACHE_MODE to the desired coherency attribute */
#define __PIC32_CACHE_MODE _CACHE_WRITEBACK_WRITEALLOCATE

/* ==================================== */
#define Index_Store_Tag_I 0x08
#define Index_Store_Tag_D 0x09

#define tmp t0
#define cfg t1
#define icachesize t2
#define ilinesize t3
#define iways t4
#define dcachesize t5
#define dlinesize t6
#define dways t7
#define save_ra v1

.sdata; .globl __pic32_icache_size; .type __pic32_icache_size,@object; .size __pic32_icache_size,4; __pic32_icache_size:; .word -1
.sdata; .globl __pic32_icache_linesize; .type __pic32_icache_linesize,@object; .size __pic32_icache_linesize,4; __pic32_icache_linesize:; .word -1
.sdata; .globl __pic32_icache_ways; .type __pic32_icache_ways,@object; .size __pic32_icache_ways,4; __pic32_icache_ways:; .word 1

.sdata; .globl __pic32_dcache_size; .type __pic32_dcache_size,@object; .size __pic32_dcache_size,4; __pic32_dcache_size:; .word -1
.sdata; .globl __pic32_dcache_linesize; .type __pic32_dcache_linesize,@object; .size __pic32_dcache_linesize,4; __pic32_dcache_linesize:; .word -1
.sdata; .globl __pic32_dcache_ways; .type __pic32_dcache_ways,@object; .size __pic32_dcache_ways,4; __pic32_dcache_ways:; .word 1

.sdata; .globl __pic32_scache_size; .type __pic32_scache_size,@object; .size __pic32_scache_size,4; __pic32_scache_size:; .word -1
.sdata; .globl __pic32_scache_linesize; .type __pic32_scache_linesize,@object; .size __pic32_scache_linesize,4; __pic32_scache_linesize:; .word -1
.sdata; .globl __pic32_scache_ways; .type __pic32_scache_ways,@object; .size __pic32_scache_ways,4; __pic32_scache_ways:; .word 1

.section .cache_init.cache, code
        .set nomips16
        .ent __size_cache; __size_cache:
        mfc0 cfg,_CP0_CONFIG

        li icachesize,0
        li ilinesize,0
        li dcachesize,0
        li dlinesize,0

        /* Check that we have Config1 */
        and tmp,cfg,_CP0_CONFIG_M_MASK
        mfc0 cfg,_CP0_CONFIG1
        beqz tmp,9f
        nop

        /* Get icache line size (log2) */
        and tmp,cfg,_CP0_CONFIG1_IL_MASK
        srl tmp,_CP0_CONFIG1_IL_POSITION
        beqz tmp,8f # no i-cache
        addu tmp,1

        /* Get number of icache ways */
        and iways,cfg,_CP0_CONFIG1_IA_MASK
        srl iways,_CP0_CONFIG1_IA_POSITION
        addu iways,1
        move icachesize,iways

        /* total icache size = lines/way * linesize *ways */
        li ilinesize,1
        sll ilinesize,tmp
        sll icachesize,tmp

        /* get icache lines per way */
        and tmp,cfg,_CP0_CONFIG1_IS_MASK
        srl tmp,_CP0_CONFIG1_IS_POSITION
        addu tmp,6
        sll icachesize,tmp

        /* Get dcache line size (log2) */
8:      and tmp,cfg,_CP0_CONFIG1_DL_MASK
        srl tmp,_CP0_CONFIG1_DL_POSITION
        beqz tmp,8f # no d-cache
        addu tmp,1

        /* Get number of dcache ways */
        and dways,cfg,_CP0_CONFIG1_DA_MASK
        srl dways,_CP0_CONFIG1_DA_POSITION
        addu dways,1
        move dcachesize,dways

        /* Total dcache size = lines/way * linesize * ways */
        li dlinesize,1
        sll dlinesize,tmp
        sll dcachesize,tmp

        and tmp,cfg,_CP0_CONFIG1_DS_MASK
        srl tmp,_CP0_CONFIG1_DS_POSITION
        addu tmp,6
        sll dcachesize,tmp

#undef cfg
#undef tmp

8:
9:   j ra
     nop
    .size __size_cache,.-__size_cache; .end __size_cache;

/*
 * void __pic32_size_cache()
 *
 * Work out size of I & D caches (assume already initialized)
 */
        .section .cache_init.pic32_size_cache, code
        .set nomips16
        .globl __pic32_size_cache; .ent __pic32_size_cache;

__pic32_size_cache:
        lw t0,__pic32_icache_size
        move a3,ra
        bgtz t0,8f # already known?

        bal __size_cache
        move ra,a3

..savesize:
        sw icachesize,__pic32_icache_size
        sw dcachesize,__pic32_dcache_size
        sw ilinesize,__pic32_icache_linesize
        sw dlinesize,__pic32_dcache_linesize
        sw iways,__pic32_icache_ways
        sw dways,__pic32_dcache_ways

8:      j ra
        nop
        .size __pic32_size_cache,.-__pic32_size_cache; .end __pic32_size_cache

/*
 * void __pic32_init_cache()
 *
 * Work out size and initialize I & D caches.
 */
        .section .cache_init.pic32_init_cache, code
        .set nomips16
        .globl __pic32_init_cache; .ent __pic32_init_cache;
__pic32_init_cache:

        move save_ra,ra
        bal __size_cache

        /* Run uncached */
        .set noreorder
        .set nomacro

        /*
         * The caches may be in an indeterminate state, so we force an
         * invalidate, load/fill, and invalidate for each line.
         */

        /* Disable all i/u and cache exceptions */
        .set macro
        .set noreorder
        # Disable interrupts and set UM=1
        # Save current status in tmp
        mfc0 t0,_CP0_STATUS
        li t1,~_CP0_STATUS_IE_MASK
        and t1,t0
        or t1,_CP0_STATUS_ERL_MASK
        mtc0 t1,_CP0_STATUS
        ehb

        mtc0 zero,_CP0_ERRCTL
        mtc0 zero,_CP0_TAGLO # 4K taglo / 2*K itaglo
        ehb

        /* Initialize primary instruction cache */
        .set noreorder
4:      la a0,__pic32_init_cache_program_base_addr
        bne a0,zero,0f
        /* Use a default if the symbol is not defined */
        li a0,0x9D000000 /* KSEG0_PROGRAM_BASE */
0:      beqz icachesize,8f    # icachesize
        addu a1,a0,icachesize # limit = base + icachesize
1:      addu a0,ilinesize  # line size
        bne a0,a1,1b
        cache Index_Store_Tag_I,-4(a0) # BDSLOT: clear tag


        /* Initialize primary data cache */
        .set noreorder
8:      la a0,__pic32_init_cache_data_base_addr
        bne a0,zero,0f
        /* Use a default if the symbol is not defined */
        li a0,0x80000000  /* KSEG_DATA_BASE */

0:      beqz dcachesize,8f
        addu a1,a0,dcachesize # limit = base + dcachesize
1:      addu a0,dlinesize
        bne a0,a1,1b
        cache Index_Store_Tag_D,-4(a0) # BDSLOT: clear tag

        .set reorder

8:      sync

        /* Store the sizes only after the caches are initialized */
4:      sw icachesize,__pic32_icache_size
        sw dcachesize,__pic32_dcache_size
        sw ilinesize,__pic32_icache_linesize
        sw dlinesize,__pic32_dcache_linesize
        sw iways,__pic32_icache_ways
        sw dways,__pic32_dcache_ways

        .set noreorder

        mfc0 t0, _CP0_CONFIG
        ori t0, _CP0_CONFIG_K0_MASK
        xori t0, _CP0_CONFIG_K0_MASK
        ori t0, __PIC32_CACHE_MODE
        mtc0 t0, _CP0_CONFIG

        # restore status
        mtc0 t0,_CP0_STATUS
        ehb
         .set reorder

        move ra, save_ra
        jr ra
        nop
        .size __pic32_init_cache,.-__pic32_init_cache; .end __pic32_init_cache;

        .section .text.main_entry,code,keep
        .ent _main_entry
_main_entry:

#if defined(CPP_INIT)
        .weak _init
        # call .init section to run constructors etc
        lui	a0,%hi(_init)
        addiu	sp,sp,-24
        addiu	a0,a0,%lo(_init)
        beq	a0,$0,2f
        sw	$31,20(sp)	 #,
        jalr	a0
        nop
2:
#endif
        and     a0,a0,0
        and     a1,a1,0

        ##################################################################

        # Call main
        ##################################################################
        jal main
        nop

#if defined(CALL_EXIT)
        ##################################################################
        # Call exit()
        ##################################################################
        jal exit
        nop
#endif
   
        ##################################################################
        # Just in case, go into infinite loop
        # Call a software breakpoint only with -mdebugger compiler option
        ##################################################################
        .weak __exception_handler_break
__crt0_exit:
1:

#if 0 // no interrupts in bootloader
        la      v0,__exception_handler_break
        beq     v0,0,0f
        nop
        jalr    v0
        nop
#endif // endo of exception handler break

0:      b       1b
        nop

        .globl __crt0_exit
        .end _main_entry
