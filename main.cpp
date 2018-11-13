#include "mbed.h"
#include "SPI_NAND.h"

Serial pc(USBTX, USBRX);
SPI_NAND spinand(D11, D12, D13, D10, D8);

void dump(uint8_t *dt) {
    uint16_t ca = 0;

    for(uint16_t i=0; i<((PAGE_SIZE+SPARE_SIZE)/16); i++) {
        pc.printf("%04X: ", ca);
        for(uint16_t j=0; j<16; j++) {
            pc.printf("%02X ", dt[i*16+j]);
        }
        pc.printf("\n");
        ca = ca + 16;
    }
}

// Block Erase
void block_erase(uint32_t addr) {
    pc.printf("==================Block Erase Start==================\n");
    spinand.SPI_NAND_WriteEnable();
    spinand.SPI_NAND_BlockErase(addr);
    pc.printf("Block Erase (D8h) : done\n");
}

// Read data
void read_data(uint32_t addr) {
    uint8_t rdata[PAGE_SIZE+SPARE_SIZE]={};    // Read data buffer
    uint16_t num;                   // Number of read data

    pc.printf("===================Page Read Start===================\n");

    num = spinand.SPI_NAND_PageDataRead(addr);
    if(num) {
        pc.printf("Page Data Read (13h) : done\n");
    }

    num = spinand.SPI_NAND_ReadData(0x00, rdata);
    pc.printf("Read Data (03h) : num = %d\n", num);
    dump(rdata);
}

// Program data
void program_data(uint32_t addr, uint8_t mode) {
    uint8_t pdata[PAGE_SIZE+SPARE_SIZE]={};    // Program data buffer
    uint16_t num;                   // Number of read data

    pc.printf("=================Page Program Start==================\n");

    if (mode==0) {    // Increment program mode
        for(uint16_t i=0; i<PAGE_SIZE+SPARE_SIZE; i++) {
            pdata[i] = pdata[i] + i;
        }
    } else if(mode==1) {    // AAh 55h program mode
        for(uint16_t i=0; i<PAGE_SIZE+SPARE_SIZE; i++) {
            if(i%2==0) {
                pdata[i] = 0xAA;
            } else {
                pdata[i] = 0x55;
            }
        }
    } else if(mode==2) {    // FFh 00h program mode
        for(uint16_t i=0; i<PAGE_SIZE+SPARE_SIZE; i++) {
            if(i%2==0) {
                pdata[i] = 0xFF;
            } else {
                pdata[i] = 0x00;
            }
        }
    } else if(mode==3) {    // 00h program mode
        for(uint16_t i=0; i<PAGE_SIZE+SPARE_SIZE; i++) {
            pdata[i] = 0x00;
        }
    }

    spinand.SPI_NAND_WriteEnable();
    num = spinand.SPI_NAND_ProgramDataLoad(0x00, pdata);
    if(num) {
        pc.printf("Program Data Load (02h) : num = %d\n", num);
    }

    spinand.SPI_NAND_WriteEnable();
    num = spinand.SPI_NAND_ProgramExecute(addr);
    if(num) {
        pc.printf("Program Execute (10h) : done\n");
    }
}

// Detect Initial Bad Block Marker
void detect_ibbm(void) {
    uint32_t addr=0;
    uint8_t rdata[PAGE_SIZE+SPARE_SIZE]={};    // Read data buffer
    uint32_t bb_num=0;

    for(uint16_t i=0; i<MAX_BLOCKS; i++) {
        spinand.SPI_NAND_PageDataRead(addr);
        spinand.SPI_NAND_ReadData2(0, rdata);
        if(rdata[0]!=0xFF) {
            pc.printf("Block %d = Initial Bad Block\n", i);
            bb_num = bb_num + 1;
        }
        addr = addr + 64;
    }
    pc.printf("Total Number of Initial Bad Blocks = %d\n", bb_num);
}

int main() {
    uint8_t temp[32];    // Temporary data buffer
    uint8_t menu;
    uint32_t address;

    // Set Block Protect Bits to 0
    spinand.SPI_NAND_WriteStatusReg1(0x00);
    pc.printf("\n");

    while(1) {
        pc.printf("=====================================================\n");
        pc.printf("=         mbed Serial NAND Sample Program           =\n");
        pc.printf("=         Winbond Electronics Corporation           =\n");
        pc.printf("=====================================================\n");
        // Get Manufacture and Device ID
        spinand.SPI_NAND_ReadID(temp);
        pc.printf("MFR ID : ");
        for(uint8_t i=0; i<1; i++) {
            pc.printf("%X", temp[i]);
            pc.printf(" ");
        }
        pc.printf("\n");
        pc.printf("DEV ID : ");
        for(uint8_t i=1; i<3; i++) {
            pc.printf("%X", temp[i]);
            pc.printf(" ");
        }
        pc.printf("\n");

        // Get ECC-E and BUF of Status Register-2
        temp[0] = spinand.SPI_NAND_ReadStatusReg2();
        if(temp[0] & 0x10) {
            pc.printf("ECC-E  : 1\n");
        } else {
            pc.printf("ECC-E  : 0\n");
        }
        if(temp[0] & 0x08) {
            pc.printf("BUF    : 1\n");
        } else {
            pc.printf("BUF    : 0\n");
        }

        // Get P-FAIL, E-FAIL and WEL of Status Register-3
        temp[0] = spinand.SPI_NAND_ReadStatusReg3();
        if(temp[0] & 0x08) {
            pc.printf("P-FAIL : 1\n");
        } else {
            pc.printf("P-FAIL : 0\n");
        }
        if(temp[0] & 0x04) {
            pc.printf("E-FAIL : 1\n");
        } else {
            pc.printf("E-FAIL : 0\n");
        }

        pc.printf("=====================================================\n");
        pc.printf("Menu :\n");
        pc.printf("  1. Read\n");
        pc.printf("  2. Block Erase\n");
        pc.printf("  3. Program Increment Data\n");
        pc.printf("  4. Program AAh 55h\n");
        pc.printf("  5. Program FFh 00h\n");
        pc.printf("  6. Program 00h\n");
        pc.printf("  7. Detect Initial Bad Block Marker\n");
        pc.printf("       -> Please execute \"7\" before the first Erase/Program.\n");
        pc.printf("Please input menu number: ");
        pc.scanf("%d", &menu);

        switch(menu) {
            case 1:
                pc.printf(">Read\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                read_data(address);
                break;
            case 2:
                pc.printf(">Block Erase\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                block_erase(address);
                break;
            case 3:
                pc.printf(">Program Increment Data\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                program_data(address, 0);
                break;
            case 4:
                pc.printf(">Program AAh 55h\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                program_data(address, 1);
                break;
            case 5:
                pc.printf(">Program FFh 00h\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                program_data(address, 2);
                break;
            case 6:
                pc.printf(">Program 00h\n");
                pc.printf(">Please input address: ");
                pc.scanf("%x", &address);
                program_data(address, 3);
                break;
            case 7:
                pc.printf(">Detect Bad Block Marker\n");
                detect_ibbm();
                break;
            default:
                pc.printf("Invalid menu number\n");
                break;
            }
    }
}
