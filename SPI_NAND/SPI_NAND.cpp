#include "SPI_NAND.h"

SPI_NAND::SPI_NAND(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName hold)
    :
    _spi(mosi, miso, sclk),
    _cs(cs),
    _hold(hold)
{
    SPI_NAND_Begin();
}

SPI_NAND::~SPI_NAND() {
}

void SPI_NAND::SPI_NAND_Begin() {
    _spi.format(8, 0);
    _spi.frequency(20000000);  //SPI Clock = 20MHz
    SPI_NAND_Deselect();
}

void SPI_NAND::SPI_NAND_Select() {
    _cs = 0;
}

void SPI_NAND::SPI_NAND_Deselect() {
    _cs = 1;
}

void SPI_NAND::SPI_NAND_Hold() {
    _hold = 0;
}

void SPI_NAND::SPI_NAND_Unhold() {
    _hold = 1;
}

void SPI_NAND::SPI_NAND_WriteEnable() {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_WRITE_ENABLE);
    SPI_NAND_Deselect();
}

void SPI_NAND::SPI_NAND_WriteDisable() {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_WRITE_DISABLE);
    SPI_NAND_Deselect();
}

uint8_t SPI_NAND::SPI_NAND_ReadStatusReg1() {
    uint8_t rc;
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_1);
    rc = _spi.write(0xFF);
    SPI_NAND_Deselect();
    return rc;
}

uint8_t SPI_NAND::SPI_NAND_ReadStatusReg2() {
    uint8_t rc;
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_2);
    rc = _spi.write(0xFF);
    SPI_NAND_Deselect();
    return rc;
}

uint8_t SPI_NAND::SPI_NAND_ReadStatusReg3() {
    uint8_t rc;
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_3);
    rc = _spi.write(0xFF);
    SPI_NAND_Deselect();
    return rc;
}

void SPI_NAND::SPI_NAND_WriteStatusReg1(uint8_t rc) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_WRITE_STATUS_REG);
    _spi.write(ADR_STATUS_REG_1);
    _spi.write(rc);
    SPI_NAND_Deselect();
}

void SPI_NAND::SPI_NAND_WriteStatusReg2(uint8_t rc) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_WRITE_STATUS_REG);
    _spi.write(ADR_STATUS_REG_2);
    _spi.write(rc);
    SPI_NAND_Deselect();
}

bool SPI_NAND::SPI_NAND_IsBusy() {
    uint8_t r1;
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_STATUS_REG);
    _spi.write(ADR_STATUS_REG_3);
    r1 = _spi.write(0xFF);
    SPI_NAND_Deselect();
    if(r1 & SR3_BUSY_MASK)
        return true;
    return false;
}

void SPI_NAND::SPI_NAND_ReadID(uint8_t* d) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_JEDEC_ID);
    _spi.write(0x00);
    for (uint8_t i=0; i<3; i++) {
        d[i] = _spi.write(0x00);
    }
    SPI_NAND_Deselect();
}

bool SPI_NAND::SPI_NAND_PageDataRead(uint16_t addr) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_PAGE_DATA_READ);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_NAND_Deselect();
    wait(0.1);
    return true;
}

uint16_t SPI_NAND::SPI_NAND_ReadData(uint16_t addr, uint8_t *buf) { 
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_DATA);
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    _spi.write(0x00);           // Dummy
    uint16_t i;
    for(i=0; i<(PAGE_SIZE+SPARE_SIZE); i++) {
        buf[i] = _spi.write(0x00);
    }
    SPI_NAND_Deselect();
    wait(0.1);
    return i;
}

void SPI_NAND::SPI_NAND_ReadData2(uint16_t addr, uint8_t *buf) { 
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_READ_DATA);
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    _spi.write(0x00);           // Dummy
    buf[0] = _spi.write(0x00);
    SPI_NAND_Deselect();
    wait(0.1);
}

void SPI_NAND::SPI_NAND_BlockErase(uint16_t addr) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_BLOCK_ERASE);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_NAND_Deselect();
    wait(2);
}

uint16_t SPI_NAND::SPI_NAND_ProgramDataLoad(uint16_t addr, uint8_t *buf) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_PGM_DATA_LOAD);
    _spi.write(addr>>8);        // CA15-PA08
    _spi.write(addr & 0xFF);    // CA07-PA00
    uint16_t i;
    for(i=0; i<(PAGE_SIZE+SPARE_SIZE); i++) {
        _spi.write(buf[i]);
    }
    SPI_NAND_Deselect();
    wait(0.1);
    return i;
}

bool SPI_NAND::SPI_NAND_ProgramExecute(uint16_t addr) {
    SPI_NAND_Select();
    SPI_NAND_Unhold();
    _spi.write(CMD_PGM_EXECUTE);
    _spi.write(0x00);           // Dummy
    _spi.write(addr>>8);        // PA15-PA08
    _spi.write(addr & 0xFF);    // PA07-PA00
    SPI_NAND_Deselect();
    wait(1);
    return true;
}
