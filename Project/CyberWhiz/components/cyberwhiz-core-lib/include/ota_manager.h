/**
 * @file ota_manager.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2025-01-05
 * 
 * @copyright, Copyright (C) 2025 by CyberWhiz
 * 
 *    All Rights Reserved.
 *    Permission to use, reproduce, copy, prepare derivative works,
 *    modify, distribute, perform, display or sell this software and/or
 *    its documentation for any purpose is prohibited without the express
 *    written consent of CyberWhiz
 * 
 */

#ifndef OTA_MANAGER_H_
#define OTA_MANAGER_H_

#include "esp_err.h"
#include <stdbool.h>

#define SKIP_VERSION_CHECK

#define MB_FLASH_PAGE_SIZE 256 ///< TODO: 256
#define MB_METADATA_OFFSET (sizeof(tsMBOTAUpdateInfo))

#define MB_OTA_TIMEOUT_IN_SECS 10
#define MB_OTA_WAIT_CHECKSUM_TIMEOUT_IN_SECS 60

typedef struct {
    uint8_t hardwareVersion;
    uint8_t bootloaderVersion;
    uint8_t softwareVersion;
    bool isMainboardOTAActive;
    uint32_t imageSize;
    uint32_t crc32checksum;
} tsMBOTAUpdateInfo;

extern tsMBOTAUpdateInfo MBOTAUpdateInfo;

void start_nb_ota_update(const char *image_name);
esp_err_t download_mainboard_image(const char *url);
esp_err_t readMBImageChunkFromPartitionTable(uint8_t *buffer, size_t offset, size_t chunkLength);

#endif /* OTA_MANAGER_H_ */
