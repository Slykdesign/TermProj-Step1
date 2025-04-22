#include "vdi.h"
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

// Function prototypes
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

int main() {
    VDIFile *vdi = vdiOpen("./good-fixed-1k.vdi");
    if (!vdi) return 1;

    printf("Dump of VDI header:\n");
    displayVDIHeader(vdi);

    printf("\nTranslation map:\n");
    displayVDITranslationMap(vdi);

    printf("\nPartition table from Master Boot Record:\n");
    displayMBR(vdi);

    char buffer[512];
    vdiSeek(vdi, 0, SEEK_SET);
    vdiRead(vdi, buffer, 512);
    printf("%.*s\n", 512, buffer);

    vdiClose(vdi);
    return 0;
}

// Function Definitions
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset) {
    count = (count > 256) ? 256 : count;  // Limit to 256 bytes
    printf("Offset: 0x%lx\n", offset);

    // Header line
    printf("00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 0...4...8...c...\n");
    printf("+-----------------------------------------------+ +----------------+\n");

    for (int i = 0; i < 16; i++) {  // 16 lines of 16 bytes
        printf("%02x|", i*16);

        // Hexadecimal display
        for (int j = 0; j < 16; j++) {
            size_t pos = i*16 + j;
            if (pos >= skip && pos < skip+count && pos < 256) {
                printf("%02x ", buf[pos]);
            } else {
                printf("   ");
            }
        }

        printf("|%02x|", i*16);

        // Character display
        for (int j = 0; j < 16; j++) {
            size_t pos = i*16 + j;
            if (pos >= skip && pos < skip+count && pos < 256) {
                printf("%c", isprint(buf[pos]) ? buf[pos] : '.');
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("+-----------------------------------------------+ +----------------+\n\n");
}

void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset) {
    for (uint32_t i = 0; i < count; i += 256) {
        uint32_t chunk_size = (count - i > 256) ? 256 : count - i;
        displayBufferPage(buf + i, chunk_size, 0, offset + i);
    }
}
