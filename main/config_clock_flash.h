#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_CLOCK_FLASH_H
#define CONFIG_CLOCK_FLASH_H

#include <stdint.h>

extern void CLOCK_FLASH_config();
extern void erase_flash_sector(uint32_t FLASH_Sector);
extern void write_1byte_to_flash(uint32_t address, uint8_t data);
extern void write_2bytes_to_flash(uint32_t address, uint16_t data);
extern void write_4bytes_to_flash(uint32_t address, uint32_t data);

#endif

#ifdef __cplusplus
}
#endif
