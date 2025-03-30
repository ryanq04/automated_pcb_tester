#ifndef __CAMERACODE_H__
#define __CAMERACODE_H__

#include "main.h"
#include "config.h"
#include <stdint.h>
#include "ov7670.h"

// External data buffers
extern uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];
extern uint8_t send_ptr[FRAMESIZE * 2];
extern uint8_t dma_flag;

// Printing / debugging helpers
void print_buf(void);
void print_bb(uint8_t *byte_ptr);
void print_sb(uint8_t *send_ptr);
void print_ss(void);

// Frame/image senders
void send_frame(void);
void send_img(void);

// Processing snapshot (single or continuous)
void p3(void);  // One-shot snapshot
void p4(void);  // Continuous capture loop

#endif /* __CAMERACODE_H__ */
