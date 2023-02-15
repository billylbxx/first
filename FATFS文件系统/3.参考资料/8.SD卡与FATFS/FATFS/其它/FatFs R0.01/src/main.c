#include "diskio.h"
#include "ff.h"
//#include "tff.h"

//for FatFs R0.01

void main(void)
{
    FATFS fs;            // FatFs work area
    FIL fsrc, fdst;      // file structures
    BYTE fbuff[512*2];   // file r/w buffers (not required for Tiny-FatFs)
    BYTE buffer[4096];   // file copy buffer
    FRESULT res;         // FatFs function common result code
    WORD br, bw;         // File R/W count


    // Activate FatFs module
    memset(&fs, 0, sizeof(FATFS));
    FatFs = &fs;

    // Open source file
    fsrc.buffer = fbuff+0;	// (not required for Tiny-FatFs)
    res = f_open(&fsrc, "/srcfile.dat", FA_OPEN_EXISTING | FA_READ);
    if (res) die(res);

    // Create destination file
    fdst.buffer = fbuff+512;	// (not required for Tiny-FatFs)
    res = f_open(&fdst, "/dstfile.dat", FA_CREATE_ALWAYS | FA_WRITE);
    if (res) die(res);

    // Copy source to destination
    for (;;) {
        res = f_read(&fsrc, buffer, sizeof(buffer), &br);
        if (res) die(res);
        if (br == 0) break;
        res = f_write(&fdst, buffer, br, &bw);
        if (res) die(res);
        if (bw < br) break;
    }

    // Close all files
    f_close(&fsrc);
    f_close(&fdst);

    // Deactivate FatFs module
    FatFs = NULL;
}
