#include <defs.h>
#include <x86.h>
#include <elf.h>

/* *********************************************************************
 * This a dirt simple boot loader, whose sole job is to boot
 * an ELF kernel image from the first IDE hard disk.
 *
 * DISK LAYOUT
 *  * This program(bootasm.S and bootmain.c) is the bootloader.
 *    It should be stored in the first sector of the disk.
 *
 *  * The 2nd sector onward holds the kernel image.
 *
 *  * The kernel image must be in ELF format.
 *
 * BOOT UP STEPS
 *  * when the CPU boots it loads the BIOS into memory and executes it
 *
 *  * the BIOS intializes devices, sets of the interrupt routines, and
 *    reads the first sector of the boot device(e.g., hard-drive)
 *    into memory and jumps to it.
 *
 *  * Assuming this boot loader is stored in the first sector of the
 *    hard-drive, this code takes over...
 *
 *  * control starts in bootasm.S -- which sets up protected mode,
 *    and a stack so C code then run, then calls bootmain()
 *
 *  * bootmain() in this file takes over, reads in the kernel and jumps to it.
 * */

#define SECTSIZE        512
#define ELFHDR          ((struct elfhdr *)0x10000)      // scratch space

/* waitdisk - wait for disk ready */
static void
waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void
readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void
readseg(uintptr_t va, uint32_t count, uint32_t offset) {
    uintptr_t end_va = va + count;

    // round down to sector boundary
    va -= offset % SECTSIZE;

    // translate from bytes to sectors; kernel starts at sector 1
    uint32_t secno = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for (; va < end_va; va += SECTSIZE, secno++) {
        readsect((void *) va, secno);
    }
}

//#define screen_width 80
//#define screen_height 25
//
//void print_char(int8_t x, int8_t y, char c) {
//    //wait x,y outcome ,then output str[i] !!!
//    //very important algorithm processs!!!
//    static int16_t *monitor_io_memory = (int16_t *) 0xb8000;
//    int32_t idx = y * screen_width + x;
//    //using '&' not '|' ,otherwise blink letters!!!
//    monitor_io_memory[idx] = (monitor_io_memory[idx] & 0xFF00) | c;
//}

//void clear_screen() {
//    //rows
//    for (uint8_t y = 0; y < screen_height; y++) {
//        //cols
//        for (uint8_t x = 0; x < screen_width; x++) {
//            print_char(x, y, '\0');
//        }
//    }
//}
//

//void printf(char *str) {
//    //y-->rows,x-->cols
//    static u8 x = 0, y = 0;
//
//    //写入字符串，取或0xff00的意思是我们需要把屏幕高四位拉低，
//    //否则就是黑色的字体，黑色的字体黑色的屏幕是啥也看不到的
//    for (int i = 0; str[i] != '\0'; ++i) {
//        //fistly check letter
//        switch (str[i]) {
//            case '\n':
//                y++;//checkout rows not output!!!
//                x = 0;
//                break;
//            default:
//                //firstly,check pos
//                if (x >= screen_width) {
//                    y++;
//                    x = 0;
//                }
//                if (y >= screen_height) {
//                    clear_screen();
//                    x = 0;
//                    y = 0;
//                }
//                //wait x,y outcome ,then output str[i] !!!
//                //very important algorithm processs!!!
//                print_char(x, y, str[i]);
//                x++;
//
//        }
//    }
//}

/* bootmain - the entry of bootloader */
void
bootmain(void) {
    static int16_t *monitor_io_memory = (int16_t *) 0xb8000;
    char *err;
    static const int start_pos = 640;
    int i = 0;
    // read the 1st page off disk
    readseg((uintptr_t) ELFHDR, SECTSIZE * 8, 0);

    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    // load each program segment (ignores ph flags)
    ph = (struct proghdr *) ((uintptr_t) ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph++) {
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }

    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void)) (ELFHDR->e_entry & 0xFFFFFF))();

    bad:
    // 下面注释打开可以打印字符
//    err = "err";
//    while (i < 3) {
//        monitor_io_memory[start_pos + i] =
//                (monitor_io_memory[start_pos + i] & 0xFC00) | err[i];
//        i++;
//    }
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);
}

