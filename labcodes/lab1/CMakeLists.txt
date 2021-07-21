cmake_minimum_required(VERSION 3.19)
project(lab1 C)

set(CMAKE_C_STANDARD 11)

include_directories(boot)
include_directories(kern/debug)
include_directories(kern/driver)
include_directories(kern/mm)
include_directories(kern/trap)
include_directories(libs)
include_directories(tools)

add_executable(lab1
        boot/asm.h
        boot/bootmain.c
        kern/debug/assert.h
        kern/debug/kdebug.c
        kern/debug/kdebug.h
        kern/debug/kmonitor.c
        kern/debug/kmonitor.h
        kern/debug/panic.c
        kern/debug/stab.h
        kern/driver/clock.c
        kern/driver/clock.h
        kern/driver/console.c
        kern/driver/console.h
        kern/driver/intr.c
        kern/driver/intr.h
        kern/driver/kbdreg.h
        kern/driver/picirq.c
        kern/driver/picirq.h
        kern/init/init.c
        kern/libs/readline.c
        kern/libs/stdio.c
        kern/mm/memlayout.h
        kern/mm/mmu.h
        kern/mm/pmm.c
        kern/mm/pmm.h
        kern/trap/trap.c
        kern/trap/trap.h
        libs/defs.h
        libs/elf.h
        libs/error.h
        libs/printfmt.c
        libs/stdarg.h
        libs/stdio.h
        libs/string.c
        libs/string.h
        libs/x86.h
        tools/sign.c
        tools/vector.c
        )