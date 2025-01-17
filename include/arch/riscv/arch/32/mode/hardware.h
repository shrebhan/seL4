/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <util.h>
#include <sel4/config.h>

/*
 *         2^32 +-------------------+
 *              |  Kernel Devices   |
 *  2^32 - 2^22 +-------------------+ KDEV_BASE
 *              |    Kernel ELF     |
 *  2^32 - 2^23 +-------------------+ PPTR_TOP / KERNEL_ELF_BASE
 *              |                   |
 *              |  Physical Memory  |
 *              |       Window      |
 *              |                   |
 *              +-------------------+ USER_TOP / PPTR_BASE
 *              |                   |
 *              |       User        |
 *              |                   |
 *          0x0 +-------------------+
 */

/* last accessible virtual address in user space */
#define USER_TOP seL4_UserTop

/* The first physical address to map into the kernel's physical memory
 * window */
#define PADDR_BASE physBase

/* The base address in virtual memory to use for the 1:1 physical memory
 * mapping */
#define PPTR_BASE seL4_UserTop

/* Top of the physical memory window */
#ifdef CONFIG_KERNEL_LOG_BUFFER
#define PPTR_TOP UL_CONST(0xFF400000)
/* Place the kernel log buffer after the PPTR region */
#define KS_LOG_PPTR PPTR_TOP
#else
#define PPTR_TOP UL_CONST(0xFF800000)
#endif

/* The physical memory address to use for mapping the kernel ELF
 *
 * This represents the physical address that the kernel image will be linked to. This needs to
 * be on a 1gb boundary as we currently require being able to creating a mapping to this address
 * as the largest frame size */
#define KERNEL_ELF_PADDR_BASE UL_CONST(0x84000000)

/* The base address in virtual memory to use for the kernel ELF mapping */
#define KERNEL_ELF_BASE UL_CONST(0xFF800000)

/* The base address in virtual memory to use for the kernel device
 * mapping region. These are mapped in the kernel page table. */
#define KDEV_BASE UL_CONST(0xFFC00000)

#define LOAD  lw
#define STORE sw

#ifndef __ASSEMBLER__

#include <stdint.h>

static inline uint64_t riscv_read_time(void)
{
    uint32_t nH1, nL, nH2;
    asm volatile(
        "rdtimeh %0\n"
        "rdtime  %1\n"
        "rdtimeh %2\n"
        : "=r"(nH1), "=r"(nL), "=r"(nH2));
    if (nH1 < nH2) {
        /* Ensure that the time is correct if there is a rollover in the
         * high bits between reading the low and high bits. */
        asm volatile(
            "rdtime  %0\n"
            : "=r"(nL));
        nH1 = nH2;
    }
    return ((uint64_t)((uint64_t) nH1 << 32)) | (nL);
}

static inline uint64_t riscv_read_cycle(void)
{
    uint32_t nH1, nL, nH2;
    asm volatile(
        "rdcycleh %0\n"
        "rdcycle  %1\n"
        "rdcycleh %2\n"
        : "=r"(nH1), "=r"(nL), "=r"(nH2));
    if (nH1 != nH2) {
        /* Ensure that the cycles are correct if there is a rollover in the
         * high bits between reading the low and high bits. */
        asm volatile(
            "rdcycle  %0\n"
            : "=r"(nL));
        nH1 = nH2;
    }
    return ((uint64_t)((uint64_t) nH1 << 32)) | (nL);
}

#endif /* __ASSEMBLER__ */

