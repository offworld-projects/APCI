#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/types.h>
#include <unistd.h>
#include <curses.h>
#include "apcilib.h"
#include "kbhit.inc"

int apci;
int terminated = 0;

/*
 * XXX(jwd)
 *
 * follow the steps in the top-level APCI README to compile/run the executable built by this source file;
 * remaining comments are just for context:
 *
 * there is an example for the mpcie-iiro-8 device which, based upon a comparison of the datasheets, looks
 * very similar to the mpcie-idio-8.  Therefore, I just copied that example and made some modifications to reflect the
 * the fact that the mpcie-idio-8 uses FET (not EM relays).  After we test that this code interacts with the kernel
 * driver as expected, we can write some python bindings for the API entrypoints that are used
 *
 * MPCIe-IIRO-8 manual URL: https://accesio.com/MANUALS/mPCIe-IIRO-8.pdf
 * MPCIe-IDIO-8 manual URL: https://accesio.com/MANUALS/mPCIe-IDIO-8.pdf
 */

int main (int argc, char **argv)
{ 
    int status = 0;  
    __u8 fetData;
    __u8 inputData;

#define deviceFileName "/dev/apci/mpcie_idio_4_0"  // XXX(jwd) this should be what the apci driver resolves the device name to be

    apci = open(deviceFileName, O_RDONLY);

    if (apci < 0)
    {
        printf("Device file %s could not be opened. Please ensure device filename and permissions ('sudo?') and that apci.ko is insmod.\n", deviceFileName);
        exit(0);
    }

    apci_read8(apci, 1, 2, 0, &fetData);
    // print out relay data (bits 4-7 should be 0?)
    for (int N = 0; N < 8; ++N)
      printf("Relay bit %d is in state %d\n", N, (int)(fetData & (1 << N)));
    printf("(in hex) read of fet bits 0-7 (bits 4-7 are 0?) = %02hhX\n", fetData);

    apci_read8(apci, 1, 2, 1, &inputData);
    // print out input data (bits 4-7 should be 0?)
    for (int N = 0; N < 8; ++N)
      printf("Input bit %d is in state %d\n", N, (int)(inputData & (1 << N)));
    printf("(in hex) read of input bits 0-7 (bits 4-7 are 0?) = %02hhX\n", inputData);
    
    printf("\nPress any number 0 through 3 to toggle that relay. SPACE to READ.  Press ESC to exit.\n");

    do
    {
        char key = 0;
        if (kbhit())
             key = getchar();
        
        if (('0' <= key)&& (key <='3'))
        {
            printf("\nToggling fet #%c\n", key);
            fetData ^= 1<< (key-'0');
            apci_write8(apci, 1, 2, 0, fetData);
            key=' ';
        }
        
        if (key == 27) terminated = 1;

        if (key==' ')
        {
            apci_read8(apci, 1, 2, 1, &inputData);
            apci_read8(apci, 1, 2, 0, &fetData);
            printf(" read of fet bits = %02hhX;  inputs = %02hhX\n", fetData, inputData);
        }

    }while (!terminated);

    terminated = 1;

err_out:
  close(apci);

  return 0;
}
