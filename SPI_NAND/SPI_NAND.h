#include <mbed.h>

#define PAGE_SIZE                 2048
#define SPARE_SIZE                64
#define MAX_PAGES                 65536
#define MAX_BLOCKS                1024

#define CMD_DEVICE_RESET          0xFF
#define CMD_READ_JEDEC_ID         0x9F
#define CMD_READ_STATUS_REG       0x05
#define CMD_WRITE_STATUS_REG      0x01
#define CMD_WRITE_ENABLE          0x06
#define CMD_WRITE_DISABLE         0x04
#define CMD_BB_MANAGEMENT         0xA1  // not implemented yet
#define CMD_READ_BBM_LUT          0xA5  // not implemented yet
#define CMD_LAST_ECC_FPA          0xA9  // not implemented yet
#define CMD_BLOCK_ERASE           0xD8
#define CMD_PGM_DATA_LOAD         0x02
#define CMD_RPGM_DATA_LOAD        0x84  // not implemented yet
#define CMD_QUAD_DATA_LOAD        0x32  // not implemented yet
#define CMD_RQUAD_DATA_LOAD       0x34  // not implemented yet
#define CMD_PGM_EXECUTE           0x10
#define CMD_PAGE_DATA_READ        0x13
#define CMD_READ_DATA             0x03
#define CMD_FAST_READ             0x0B
#define CMD_FAST_READ_4B          0x0C  // not implemented yet
#define CMD_FAST_READ_DUAL        0x3B  // not implemented yet
#define CMD_FAST_READ_DUAL_4B     0x3C  // not implemented yet
#define CMD_FAST_READ_QUAD        0x6B  // not implemented yet
#define CMD_FAST_READ_QUAD_4B     0x6C  // not implemented yet
#define CMD_FAST_READ_DUAL_IO     0xBB  // not implemented yet
#define CMD_FAST_READ_DUAL_IO_4B  0xBC  // not implemented yet
#define CMD_FAST_READ_QUAD_IO     0xEB  // not implemented yet
#define CMD_FAST_READ_QUAD_IO_4B  0xEC  // not implemented yet
#define ADR_STATUS_REG_1          0xAF
#define ADR_STATUS_REG_2          0xBF
#define ADR_STATUS_REG_3          0xCF
#define SR3_BUSY_MASK             0x01

class SPI_NAND
{
public:
    // Create an SPI_NAND instance
    SPI_NAND(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName hold);

    // Destructor of SPI_NAND
    virtual ~SPI_NAND();

    void SPI_NAND_Begin();
    void SPI_NAND_Select();
    void SPI_NAND_Deselect();
    void SPI_NAND_Hold();
    void SPI_NAND_Unhold();
    void SPI_NAND_WriteEnable();
    void SPI_NAND_WriteDisable();
    uint8_t SPI_NAND_ReadStatusReg1();
    uint8_t SPI_NAND_ReadStatusReg2();
    uint8_t SPI_NAND_ReadStatusReg3();
    void SPI_NAND_WriteStatusReg1(uint8_t rc);
    void SPI_NAND_WriteStatusReg2(uint8_t rc);
    bool SPI_NAND_IsBusy();
    void SPI_NAND_ReadID(uint8_t* d);
    bool SPI_NAND_PageDataRead(uint16_t addr);
    uint16_t SPI_NAND_ReadData(uint16_t addr, uint8_t *buf);
    void SPI_NAND_ReadData2(uint16_t addr, uint8_t *buf);
    void SPI_NAND_BlockErase(uint16_t addr);
    uint16_t SPI_NAND_ProgramDataLoad(uint16_t addr, uint8_t *buf);
    bool SPI_NAND_ProgramExecute(uint16_t addr);

private:
    SPI _spi;
    DigitalOut _cs;
    DigitalOut _hold;
};
