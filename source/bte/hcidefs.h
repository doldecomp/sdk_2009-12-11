#ifndef BTE_HCIDEFS_H
#define BTE_HCIDEFS_H

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/include/hcidefs.h
 */

/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include "bt_types.h"
#include "data_types.h"

/*******************************************************************************
 * macros
 */

#define HCI_SUCCESS                                     0x00
#define HCI_ERR_AUTH_FAILURE                            0x05
#define HCI_ERR_KEY_MISSING                             0x06
#define HCI_ERR_HOST_REJECT_SECURITY                    0x0E
#define HCI_ERR_PAIRING_NOT_ALLOWED                     0x18
#define HCI_ERR_UNIT_KEY_USED                           0x26
#define HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED     0x29
#define HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE               0x25
#define HCI_ERR_REPEATED_ATTEMPTS                       0x17

#define HCI_MAX_INQUIRY_LENGTH					0x30

#define HCI_PER_INQ_MIN_MIN_PERIOD				0x0002
#define HCI_PER_INQ_MAX_MIN_PERIOD				0xfffe
#define HCI_PER_INQ_MIN_MAX_PERIOD				0x0003
#define HCI_PER_INQ_MAX_MAX_PERIOD				0xffff

#define LMP_COMPID_BROADCOM						15

#define HCIC_PREAMBLE_SIZE						3

/* HCI mode defenitions */
#define HCI_MODE_ACTIVE                 0x00
#define HCI_MODE_HOLD                   0x01
#define HCI_MODE_SNIFF                  0x02
#define HCI_MODE_PARK                   0x03

#define HCI_FILTER_COND_NEW_DEVICE				0x00
#define HCI_FILTER_COND_DEVICE_CLASS			0x01
#define HCI_FILTER_COND_BD_ADDR					0x02

#define HCI_GROUP_LINK_CONTROL_CMDS				(1 << 10)
#define HCI_GROUP_LINK_POLICY_CMDS				(2 << 10)
#define HCI_GROUP_HOST_CONT_BASEBAND_CMDS		(3 << 10)
#define HCI_GROUP_INFORMATIONAL_PARAMS			(4 << 10)
#define HCI_GROUP_STATUS_PARAMS					(5 << 10)
#define HCI_GROUP_TESTING_CMDS					(6 << 10)
#define HCI_GROUP_VENDOR_SPECIFIC				(0x3F << 10)

#define HCI_COMMAND_NONE						0
#define HCI_INQUIRY								(HCI_GROUP_LINK_CONTROL_CMDS | 0x0001)
#define HCI_INQUIRY_CANCEL						(HCI_GROUP_LINK_CONTROL_CMDS | 0x0002)
#define HCI_PERIODIC_INQUIRY_MODE				(HCI_GROUP_LINK_CONTROL_CMDS | 0x0003)
#define HCI_EXIT_PERIODIC_INQUIRY_MODE			(HCI_GROUP_LINK_CONTROL_CMDS | 0x0004)
#define HCI_CREATE_CONNECTION					(HCI_GROUP_LINK_CONTROL_CMDS | 0x0005)
#define HCI_DISCONNECT							(HCI_GROUP_LINK_CONTROL_CMDS | 0x0006)
#define HCI_ADD_SCO_CONNECTION					(HCI_GROUP_LINK_CONTROL_CMDS | 0x0007)
#define HCI_CREATE_CONNECTION_CANCEL			(HCI_GROUP_LINK_CONTROL_CMDS | 0x0008)
#define HCI_ACCEPT_CONNECTION_REQUEST			(HCI_GROUP_LINK_CONTROL_CMDS | 0x0009)
#define HCI_REJECT_CONNECTION_REQUEST			(HCI_GROUP_LINK_CONTROL_CMDS | 0x000a)
#define HCI_LINK_KEY_REQUEST_REPLY				(HCI_GROUP_LINK_CONTROL_CMDS | 0x000b)
#define HCI_LINK_KEY_REQUEST_NEG_REPLY			(HCI_GROUP_LINK_CONTROL_CMDS | 0x000c)
#define HCI_PIN_CODE_REQUEST_REPLY				(HCI_GROUP_LINK_CONTROL_CMDS | 0x000d)
#define HCI_PIN_CODE_REQUEST_NEG_REPLY			(HCI_GROUP_LINK_CONTROL_CMDS | 0x000e)
#define HCI_CHANGE_CONN_PACKET_TYPE				(HCI_GROUP_LINK_CONTROL_CMDS | 0x000f)
#define HCI_AUTHENTICATION_REQUESTED			(HCI_GROUP_LINK_CONTROL_CMDS | 0x0011)
#define HCI_SET_CONN_ENCRYPTION					(HCI_GROUP_LINK_CONTROL_CMDS | 0x0013)
#define HCI_CHANGE_CONN_LINK_KEY				(HCI_GROUP_LINK_CONTROL_CMDS | 0x0015)
#define HCI_MASTER_LINK_KEY						(HCI_GROUP_LINK_CONTROL_CMDS | 0x0017)
#define HCI_RMT_NAME_REQUEST					(HCI_GROUP_LINK_CONTROL_CMDS | 0x0019)
#define HCI_RMT_NAME_REQUEST_CANCEL				(HCI_GROUP_LINK_CONTROL_CMDS | 0x001a)
#define HCI_READ_RMT_FEATURES					(HCI_GROUP_LINK_CONTROL_CMDS | 0x001b)
#define HCI_READ_RMT_EXT_FEATURES				(HCI_GROUP_LINK_CONTROL_CMDS | 0x001c)
#define HCI_READ_RMT_VERSION_INFO				(HCI_GROUP_LINK_CONTROL_CMDS | 0x001d)
#define HCI_READ_RMT_CLOCK_OFFSET				(HCI_GROUP_LINK_CONTROL_CMDS | 0x001f)
#define HCI_READ_LMP_HANDLE						(HCI_GROUP_LINK_CONTROL_CMDS | 0x0020)
#define HCI_SETUP_ESCO_CONNECTION				(HCI_GROUP_LINK_CONTROL_CMDS | 0x0028)
#define HCI_ACCEPT_ESCO_CONNECTION				(HCI_GROUP_LINK_CONTROL_CMDS | 0x0029)
#define HCI_REJECT_ESCO_CONNECTION				(HCI_GROUP_LINK_CONTROL_CMDS | 0x002a)

#define HCI_HOLD_MODE							(HCI_GROUP_LINK_POLICY_CMDS | 0x0001)
#define HCI_SNIFF_MODE							(HCI_GROUP_LINK_POLICY_CMDS | 0x0003)
#define HCI_EXIT_SNIFF_MODE						(HCI_GROUP_LINK_POLICY_CMDS | 0x0004)
#define HCI_PARK_MODE							(HCI_GROUP_LINK_POLICY_CMDS | 0x0005)
#define HCI_EXIT_PARK_MODE						(HCI_GROUP_LINK_POLICY_CMDS | 0x0006)
#define HCI_QOS_SETUP							(HCI_GROUP_LINK_POLICY_CMDS | 0x0007)
#define HCI_ROLE_DISCOVERY						(HCI_GROUP_LINK_POLICY_CMDS | 0x0009)
#define HCI_SWITCH_ROLE							(HCI_GROUP_LINK_POLICY_CMDS | 0x000b)
#define HCI_READ_POLICY_SETTINGS				(HCI_GROUP_LINK_POLICY_CMDS | 0x000c)
#define HCI_WRITE_POLICY_SETTINGS				(HCI_GROUP_LINK_POLICY_CMDS | 0x000d)
#define HCI_READ_DEF_POLICY_SETTINGS			(HCI_GROUP_LINK_POLICY_CMDS | 0x000e)
#define HCI_WRITE_DEF_POLICY_SETTINGS			(HCI_GROUP_LINK_POLICY_CMDS | 0x000f)
#define HCI_FLOW_SPECIFICATION					(HCI_GROUP_LINK_POLICY_CMDS | 0x0010)
#define HCI_SNIFF_SUB_RATE						(HCI_GROUP_LINK_POLICY_CMDS | 0x0011)

#define HCI_SET_EVENT_MASK						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0001)
#define HCI_RESET								(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0003)
#define HCI_SET_EVENT_FILTER					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0005)
#define HCI_FLUSH								(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0008)
#define HCI_READ_PIN_TYPE						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0009)
#define HCI_WRITE_PIN_TYPE						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x000a)
#define HCI_CREATE_NEW_UNIT_KEY					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x000b)
#define HCI_READ_STORED_LINK_KEY				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x000d)
#define HCI_WRITE_STORED_LINK_KEY				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0011)
#define HCI_DELETE_STORED_LINK_KEY				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0012)
#define HCI_CHANGE_LOCAL_NAME					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0013)
#define HCI_READ_LOCAL_NAME						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0014)
#define HCI_READ_CONN_ACCEPT_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0015)
#define HCI_WRITE_CONN_ACCEPT_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0016)
#define HCI_READ_PAGE_TOUT						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0017)
#define HCI_WRITE_PAGE_TOUT						(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0018)
#define HCI_READ_SCAN_ENABLE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0019)
#define HCI_WRITE_SCAN_ENABLE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001a)
#define HCI_READ_PAGESCAN_CFG					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001b)
#define HCI_WRITE_PAGESCAN_CFG					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001c)
#define HCI_READ_INQUIRYSCAN_CFG				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001d)
#define HCI_WRITE_INQUIRYSCAN_CFG				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001e)
#define HCI_READ_AUTHENTICATION_ENABLE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x001f)
#define HCI_WRITE_AUTHENTICATION_ENABLE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0020)
#define HCI_READ_ENCRYPTION_MODE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0021)
#define HCI_WRITE_ENCRYPTION_MODE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0022)
#define HCI_READ_CLASS_OF_DEVICE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0023)
#define HCI_WRITE_CLASS_OF_DEVICE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0024)
#define HCI_READ_VOICE_SETTINGS					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0025)
#define HCI_WRITE_VOICE_SETTINGS				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0026)
#define HCI_READ_AUTO_FLUSH_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0027)
#define HCI_WRITE_AUTO_FLUSH_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0028)
#define HCI_READ_NUM_BCAST_REXMITS				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0029)
#define HCI_WRITE_NUM_BCAST_REXMITS				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002a)
#define HCI_READ_HOLD_MODE_ACTIVITY				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002b)
#define HCI_WRITE_HOLD_MODE_ACTIVITY			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002c)
#define HCI_READ_TRANSMIT_POWER_LEVEL			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002d)
#define HCI_READ_SCO_FLOW_CTRL_ENABLE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002e)
#define HCI_WRITE_SCO_FLOW_CTRL_ENABLE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x002f)
#define HCI_SET_HC_TO_HOST_FLOW_CTRL			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0031)
#define HCI_HOST_BUFFER_SIZE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0033)
#define HCI_HOST_NUM_PACKETS_DONE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0035)
#define HCI_READ_LINK_SUPER_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0036)
#define HCI_WRITE_LINK_SUPER_TOUT				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0037)
#define HCI_READ_NUM_SUPPORTED_IAC				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0038)
#define HCI_READ_CURRENT_IAC_LAP				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0039)
#define HCI_WRITE_CURRENT_IAC_LAP				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003a)
#define HCI_READ_PAGESCAN_PERIOD_MODE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003b)
#define HCI_WRITE_PAGESCAN_PERIOD_MODE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003c)
#define HCI_READ_PAGESCAN_MODE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003d)
#define HCI_WRITE_PAGESCAN_MODE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003e)
#define HCI_SET_AFH_CHANNELS					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x003f)
#define HCI_READ_INQSCAN_TYPE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0042)
#define HCI_WRITE_INQSCAN_TYPE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0043)
#define HCI_READ_INQUIRY_MODE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0044)
#define HCI_WRITE_INQUIRY_MODE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0045)
#define HCI_READ_PAGESCAN_TYPE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0046)
#define HCI_WRITE_PAGESCAN_TYPE					(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0047)
#define HCI_READ_AFH_ASSESSMENT_MODE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0048)
#define HCI_WRITE_AFH_ASSESSMENT_MODE			(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0049)
#define HCI_READ_EXT_INQ_RESPONSE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0051)
#define HCI_WRITE_EXT_INQ_RESPONSE				(HCI_GROUP_HOST_CONT_BASEBAND_CMDS | 0x0052)

#define HCI_READ_LOCAL_VERSION_INFO				(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0001)
#define HCI_READ_LOCAL_SUPPORTED_CMDS			(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0002)
#define HCI_READ_LOCAL_FEATURES					(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0003)
#define HCI_READ_LOCAL_EXT_FEATURES				(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0004)
#define HCI_READ_BUFFER_SIZE					(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0005)
#define HCI_READ_COUNTRY_CODE					(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0007)
#define HCI_READ_BD_ADDR						(HCI_GROUP_INFORMATIONAL_PARAMS | 0x0009)

#define HCI_READ_FAILED_CONN_COUNT				(HCI_GROUP_STATUS_PARAMS | 0x0001)
#define HCI_RESET_FAILED_CONN_COUNT				(HCI_GROUP_STATUS_PARAMS | 0x0002)
#define HCI_GET_LINK_QUALITY					(HCI_GROUP_STATUS_PARAMS | 0x0003)
#define HCI_READ_RSSI							(HCI_GROUP_STATUS_PARAMS | 0x0005)
#define HCI_READ_AFH_CH_MAP						(HCI_GROUP_STATUS_PARAMS | 0x0006)
#define HCI_READ_CLOCK							(HCI_GROUP_STATUS_PARAMS | 0x0007)

#define HCI_READ_LOOPBACK_MODE					(HCI_GROUP_TESTING_CMDS | 0x0001)
#define HCI_WRITE_LOOPBACK_MODE					(HCI_GROUP_TESTING_CMDS | 0x0002)
#define HCI_ENABLE_DEV_UNDER_TEST_MODE			(HCI_GROUP_TESTING_CMDS | 0x0003)

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

BOOLEAN btsnd_hcic_inquiry(LAP const inq_lap, UINT8 duration,
                           UINT8 response_cnt);
BOOLEAN btsnd_hcic_inq_cancel(void);
BOOLEAN btsnd_hcic_per_inq_mode(UINT16 max_period, UINT16 min_period,
                                LAP const inq_lap, UINT8 duration,
                                UINT8 response_cnt);
BOOLEAN btsnd_hcic_exit_per_inq(void);
BOOLEAN btsnd_hcic_create_conn(BD_ADDR dest, UINT16 packet_types,
                               UINT8 page_scan_rep_mode, UINT8 page_scan_mode,
                               UINT16 clock_offset, UINT8 allow_switch);
BOOLEAN btsnd_hcic_disconnect(UINT16 handle, UINT8 reason);
BOOLEAN btsnd_hcic_add_SCO_conn(UINT16 handle, UINT16 packet_types);
BOOLEAN btsnd_hcic_create_conn_cancel(BD_ADDR dest);
void btsnd_hcic_accept_conn(void *buffer, BD_ADDR dest, UINT8 role);
void btsnd_hcic_reject_conn(void *buffer, BD_ADDR dest, UINT8 reason);
BOOLEAN btsnd_hcic_link_key_req_reply(BD_ADDR bd_addr, LINK_KEY link_key);
BOOLEAN btsnd_hcic_link_key_neg_reply(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_pin_code_req_reply(BD_ADDR bd_addr, UINT8 pin_code_len,
                                      PIN_CODE pin_code);
BOOLEAN btsnd_hcic_pin_code_neg_reply(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_change_conn_type(UINT16 handle, UINT16 packet_types);
BOOLEAN btsnd_hcic_auth_request(UINT16 handle);
BOOLEAN btsnd_hcic_set_conn_encrypt(UINT16 handle, BOOLEAN enable);
BOOLEAN btsnd_hcic_change_link_key(UINT16 handle);
BOOLEAN btsnd_hcic_master_link_key(BOOLEAN key_flag);
BOOLEAN btsnd_hcic_rmt_name_req(BD_ADDR bd_addr, UINT8 page_scan_rep_mode,
                                UINT8 page_scan_mode, UINT16 clock_offset);
BOOLEAN btsnd_hcic_rmt_name_req_cancel(BD_ADDR bd_addr);
BOOLEAN btsnd_hcic_rmt_features_req(UINT16 handle);
void btsnd_hcic_rmt_ext_features(void *buffer, UINT16 handle, UINT8 page_num);
BOOLEAN btsnd_hcic_rmt_ver_req(UINT16 handle);
BOOLEAN btsnd_hcic_read_rmt_clk_offset(UINT16 handle);
void btsnd_hcic_read_lmp_handle(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_setup_esco_conn(UINT16 handle, UINT32 tx_bw, UINT32 rx_bw,
                                   UINT16 max_latency, UINT16 voice,
                                   UINT8 retrans_effort, UINT16 packet_types);
void btsnd_hcic_accept_esco_conn(void *buffer, BD_ADDR bd_addr, UINT32 tx_bw,
                                 UINT32 rx_bw, UINT16 max_latency,
                                 UINT16 content_fmt, UINT8 retrans_effort,
                                 UINT16 packet_types);
void btsnd_hcic_reject_esco_conn(void *buffer, BD_ADDR bd_addr, UINT8 reason);
BOOLEAN btsnd_hcic_hold_mode(void *buffer, UINT16 handle,
                             UINT16 max_hold_period, UINT16 min_hold_period);
BOOLEAN btsnd_hcic_sniff_mode(void *buffer, UINT16 handle,
                              UINT16 max_sniff_period, UINT16 min_sniff_period,
                              UINT16 sniff_attempt, UINT16 sniff_timeout);
BOOLEAN btsnd_hcic_exit_sniff_mode(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_park_mode(void *buffer, UINT16 handle,
                             UINT16 beacon_max_interval,
                             UINT16 beacon_min_interval);
BOOLEAN btsnd_hcic_exit_park_mode(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_qos_setup(UINT16 handle, UINT8 flags, UINT8 service_type,
                             UINT32 token_rate, UINT32 peak, UINT32 latency,
                             UINT32 delay_var);
void btsnd_hcic_role_discovery(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_switch_role(BD_ADDR bd_addr, UINT8 role);
BOOLEAN btsnd_hcic_read_policy_set(UINT16 handle);
BOOLEAN btsnd_hcic_write_policy_set(UINT16 handle, UINT16 settings);
void btsnd_hcic_read_def_policy_set(void *buffer);
void btsnd_hcic_write_def_policy_set(void *buffer, UINT16 settings);
void btsnd_hcic_flow_specification(void *buffer, UINT16 handle, UINT8 flags,
                                   UINT8 flow_direct, UINT8 service_type,
                                   UINT32 token_rate, UINT32 token_bucket_size,
                                   UINT32 peak, UINT32 latency);
void btsnd_hcic_sniff_sub_rate(void *buffer, UINT16 handle, UINT16 max_lat,
                               UINT16 min_remote_lat, UINT16 min_local_lat);
void btsnd_hcic_set_event_mask(void *buffer, BT_EVENT_MASK event_mask);
BOOLEAN btsnd_hcic_reset(void);
void btsnd_hcic_set_event_filter(void *buffer, UINT8 filt_type,
                                 UINT8 filt_cond_type, UINT8 *filt_cond,
                                 UINT8 filt_cond_len);
void btsnd_hcic_flush(void *buffer, UINT16 handle);
void btsnd_hcic_read_pin_type(void *buffer);
BOOLEAN btsnd_hcic_write_pin_type(UINT8 type);
void btsnd_hcic_new_unit_key(void *buffer);
void btsnd_hcic_read_stored_key(void *buffer, BD_ADDR bd_addr,
                                BOOLEAN read_all_flag);
void btsnd_hcic_write_stored_key(void *buffer, UINT8 num_keys, BD_ADDR *bd_addr,
                                 LINK_KEY *link_key);
BOOLEAN btsnd_hcic_delete_stored_key(BD_ADDR bd_addr, BOOLEAN delete_all_flag);
void btsnd_hcic_change_name(void *buffer, BD_NAME name);
void btsnd_hcic_read_name(void *buffer);
void btsnd_hcic_read_conn_acc_tout(void *buffer);
void btsnd_hcic_write_conn_acc_tout(void *buffer, UINT16 timeout);
void btsnd_hcic_read_page_tout(void *buffer);
void btsnd_hcic_write_page_tout(void *buffer, UINT16 timeout);
void btsnd_hcic_read_scan_enable(void *buffer);
void btsnd_hcic_write_scan_enable(void *buffer, UINT8 flag);
void btsnd_hcic_read_pagescan_cfg(void *buffer);
void btsnd_hcic_write_pagescan_cfg(void *buffer, UINT16 interval,
                                   UINT16 window);
void btsnd_hcic_read_inqscan_cfg(void *buffer);
void btsnd_hcic_write_inqscan_cfg(void *buffer, UINT16 interval, UINT16 window);
void btsnd_hcic_read_auth_enable(void *buffer);
BOOLEAN btsnd_hcic_write_auth_enable(UINT8 flag);
void btsnd_hcic_read_encr_mode(void *buffer);
BOOLEAN btsnd_hcic_write_encr_mode(UINT8 mode);
void btsnd_hcic_read_dev_class(void *buffer);
void btsnd_hcic_write_dev_class(void *buffer, DEV_CLASS dev_class);
void btsnd_hcic_read_voice_settings(void *buffer);
void btsnd_hcic_write_voice_settings(void *buffer, UINT16 flags);
void btsnd_hcic_read_auto_flush_tout(void *buffer, UINT16 handle);
void btsnd_hcic_write_auto_flush_tout(void *buffer, UINT16 handle, UINT16 tout);
void btsnd_hcic_read_num_bcast_xmit(void *buffer);
void btsnd_hcic_write_num_bcast_xmit(void *buffer, UINT8 num);
void btsnd_hcic_read_hold_mode_act(void *buffer);
void btsnd_hcic_write_hold_mode_act(void *buffer, UINT8 flags);
void btsnd_hcic_read_tx_power(void *buffer, UINT16 handle, UINT8 type);
void btsnd_hcic_read_sco_flow_enable(void *buffer);
void btsnd_hcic_write_sco_flow_enable(void *buffer, UINT8 flag);
BOOLEAN btsnd_hcic_set_host_flow_ctrl(UINT8 value);
BOOLEAN btsnd_hcic_set_host_buf_size(UINT16 acl_len, UINT8 sco_len,
                                     UINT16 acl_num, UINT16 sco_num);
void btsnd_hcic_host_num_xmitted_pkts(void *buffer, UINT8 num_handles,
                                      UINT16 *handle, UINT16 *num_pkts);
void btsnd_hcic_read_link_super_tout(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_write_link_super_tout(UINT16 handle, UINT16 timeout);
void btsnd_hcic_read_max_iac(void *buffer);
void btsnd_hcic_read_cur_iac_lap(void *buffer);
void btsnd_hcic_write_cur_iac_lap(void *buffer, UINT8 num_cur_iac,
                                  LAP * const iac_lap);
void btsnd_hcic_read_page_scan_per(void *buffer);
void btsnd_hcic_write_page_scan_per(void *buffer, UINT8 mode);
void btsnd_hcic_read_page_scan_mode(void *buffer);
void btsnd_hcic_write_page_scan_mode(void *buffer, UINT8 mode);
void btsnd_hcic_read_ext_inquiry_response(void *buffer);
void btsnd_hcic_write_ext_inquiry_response(void *buffer, UINT8 fec_req,
                                           UINT8 *p_data);
BOOLEAN btsnd_hcic_read_local_ver(void);
void btsnd_hcic_read_local_supported_cmds(void *buffer);
BOOLEAN btsnd_hcic_read_local_features(void);
void btsnd_hcic_read_local_ext_features(void *buffer, UINT8 page_num);
void btsnd_hcic_read_buffer_size(void *buffer);
void btsnd_hcic_read_country_code(void *buffer);
BOOLEAN btsnd_hcic_read_bd_addr(void);
void btsnd_hcic_read_fail_conn_count(void *buffer, UINT16 handle);
void btsnd_hcic_reset_fail_conn_count(void *buffer, UINT16 handle);
BOOLEAN btsnd_hcic_get_link_quality(UINT16 handle);
BOOLEAN btsnd_hcic_read_rssi(UINT16 handle);
void btsnd_hcic_read_loopback_mode(void *buffer);
void btsnd_hcic_write_loopback_mode(void *buffer, UINT8 mode);
void btsnd_hcic_enable_test_mode(void *buffer);
BOOLEAN btsnd_hcic_write_afh_channel_assessment_mode(UINT8 mode);
void btsnd_hcic_read_afh_channel_assessment_mode(void *buffer);
BOOLEAN btsnd_hcic_set_afh_channels(UINT8 first, UINT8 last);
void btsnd_hcic_set_afh_host_channel_class(void *buffer,
                                           UINT8 *p_afhchannelmap);
void btsnd_hcic_read_afh_channel_map(void *buffer, UINT16 handle);
void btsnd_hcic_read_clock(void *buffer, UINT16 handle, UINT8 which_clock);
void btsnd_hcic_read_inqscan_type(void *buffer);
void btsnd_hcic_write_inqscan_type(void *buffer, UINT8 type);
void btsnd_hcic_read_inquiry_mode(void *buffer);
void btsnd_hcic_write_inquiry_mode(void *buffer, UINT8 mode);
void btsnd_hcic_read_pagescan_type(void *buffer);
void btsnd_hcic_write_pagescan_type(void *buffer, UINT8 type);
void btsnd_hcic_vendor_spec_cmd(void *buffer, UINT16 opcode, UINT8 len,
                                UINT8 *p_data);
void btsnd_hcic_data(BT_HDR *p_buf, UINT16 len, UINT16 handle, UINT8 boundary,
                     UINT8 broadcast);
void btsnd_hcic_nop(void *buffer);

#ifdef __cplusplus
	}
#endif

#endif // BTE_HCIDEFS_H
