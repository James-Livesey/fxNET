#include <gint/intc.h>

#include "serial.h"

#define BIT(x) (1 << (x))
#define BITMASK(end, start) (((BIT(end + 1) >> start) - 1) << start)

#define MMIO8(addr) (*(volatile uint8_t*)(addr))
#define MMIO16(addr) (*(volatile uint16_t*)(addr))

#define SCIF_BASE 0xA4410000
#define SCIF_SIZE 0x10000

#define SCSMR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x00) // Serial mode register
#define SCBRR(x)    MMIO8   (SCIF_BASE + SCIF_SIZE * (x) + 0x04) // Bit rate register
#define SCSCR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x08) // Serial control register
#define SCFTDR(x)   MMIO8   (SCIF_BASE + SCIF_SIZE * (x) + 0x0C) // Transmit FIFO data register
#define SCFSR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x10) // Serial status register
#define SCFRDR(x)   MMIO8   (SCIF_BASE + SCIF_SIZE * (x) + 0x14) // Receive FIFO data register
#define SCFCR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x18) // FIFO control register
#define SCFDR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x1C) // FIFO data count register
#define SCLSR(x)    MMIO16  (SCIF_BASE + SCIF_SIZE * (x) + 0x24) // Line status register

#define SCSMR0  SCSMR(0)
#define SCBRR0  SCBRR(0)
#define SCSCR0  SCSCR(0)
#define SCFTDR0 SCFTDR(0)
#define SCFSR0  SCFSR(0)
#define SCFRDR0 SCFRDR(0)
#define SCFCR0  SCFCR(0)
#define SCFDR0  SCFDR(0)
#define SCLSR0  SCLSR(0)

#define SCFSR_TEND  BIT(6)
#define SCFSR_TDFE  BIT(5)
#define SCFSR_RDF   BIT(1)
#define SCFSR_DR    BIT(0)

#define SCFDR_TFDC_MASK BITMASK(12, 8)
#define SCFDR_TFDC_SHIFT 8
#define SCFDR_TFDC ((SCFDR0 & SCFDR_TFDC_MASK) >> SCFDR_TFDC_SHIFT)

#define SCFDR_RFDC_MASK BITMASK(4, 0)
#define SCFDR_RFDC_SHIFT 0
#define SCFDR_RFDC ((SCFDR0 & SCFDR_RFDC_MASK) >> SCFDR_RFDC_SHIFT)

#define SCIF_SCIF0_INTEVT 0xC00

int __Serial_Open(char* mode);
int __Serial_IsOpen();
int __Serial_Close(int mode);
int __Serial_GetTxBufferFreeCapacity();

uint8_t rxRingBuffer[1024];
uint16_t rxRingRead = 0;
volatile uint16_t rxRingWrite = 0;

bool serial_open() {
	char mode[5] = {0, 9, 0, 0, 0};

	return __Serial_Open(mode) == 0;
}

bool serial_isOpen() {
	return __Serial_IsOpen() == 1;
}

void serial_close() {
	__Serial_Close(1);
}

size_t serial_txBufferFreeSize() {
	return __Serial_GetTxBufferFreeCapacity();
}

void serial_write(char* data, size_t length) {
    while (length--) {
        while (SCFDR_TFDC > 8) {}

        SCFTDR0 = *data++;
    }
}

bool serial_readAvailable() {
    return rxRingRead != rxRingWrite;
}

char serial_readByte() {
    if (serial_readAvailable()) {
        uint8_t byte = rxRingBuffer[rxRingRead++];

        rxRingRead %= sizeof(rxRingBuffer);

        return byte;
    }

    return 0x00;
}

static void interruptHandler() {
    while (SCFDR_RFDC > 0) {
        rxRingBuffer[rxRingWrite++] = SCFRDR0;
        rxRingWrite %= sizeof(rxRingBuffer);
    }

    SCFSR0 &= ~SCFSR_DR;
    SCFSR0 &= ~SCFSR_RDF;
}

void serial_init() {
    intc_handler_function(SCIF_SCIF0_INTEVT, GINT_CALL(interruptHandler));
    intc_priority(INTC_SCIF0, 1);
}