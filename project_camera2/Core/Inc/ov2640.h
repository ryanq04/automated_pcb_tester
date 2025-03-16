/*
 * ov2640.h
 *
 *  Created on: Mar 15, 2025
 *      Author: root
 */

#ifndef INC_OV2640_H_
#define INC_OV2640_H_

#include "config.h"

// function declarations
uint8_t ov2640_read(uint8_t reg);
uint8_t ov2640_init(void);
void i2c_scan(void);
void ov2640_config();
void read_all_ov2640_registers();

#endif /* INC_OV2640_H_ */
