#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

bool serial_open();
bool serial_isOpen();
void serial_close();
size_t serial_txBufferFreeSize();
void serial_write(char* data, size_t length);
bool serial_readAvailable();
char serial_readByte();
void serial_init();

#endif