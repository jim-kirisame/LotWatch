/*
 * DFU 相关服务
 */

#ifndef _DFU_APP_
#define _DFU_APP_

#include "ble_dfu.h"
#include "dfu_app_handler.h"

#define DFU_REV_MAJOR                    0x00                                       /** DFU Major revision number to be exposed. */
#define DFU_REV_MINOR                    0x01                                       /** DFU Minor revision number to be exposed. */
#define DFU_REVISION                     ((DFU_REV_MAJOR << 8) | DFU_REV_MINOR)     /** DFU Revision number to be exposed. Combined of major and minor versions. */
#define APP_SERVICE_HANDLE_START         0x000C                                     /**< Handle of first application specific service when when service changed characteristic is present. */
#define BLE_HANDLE_MAX                   0xFFFF                                     /**< Max handle value in BLE. */

void dfu_serv_init(dm_application_instance_t * m_app_handle);
void dfu_ble_evt_dispatch(ble_evt_t * p_ble_evt);
void dfu_device_manager_event_handler(dm_handle_t const * p_handle, dm_event_t const * p_event);


#endif
