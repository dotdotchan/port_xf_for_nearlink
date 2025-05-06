/* ==================== [Includes] ========================================== */

#include "xfconfig.h"

#if (CONFIG_XF_SLE_ENABLE)

#include "xf_utils.h"
#include "xf_init.h"
#include "string.h"

#include "common_def.h"
#include "soc_osal.h"

#include "xf_sle_connection_manager.h"
#include "sle_connection_manager.h"

#include "key_id.h"
#include "nv.h"
#include "common.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_sle_ssaps"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

// 更新连接参数
xf_err_t xf_sle_update_conn_params(
    xf_sle_conn_param_update_t *params)
{
    sle_connection_param_update_t conn_param = {
        .conn_id = params->conn_id,
        .interval_min = params->interval_min,
        .interval_max = params->interval_max,
        .max_latency = params->max_latency,
        .supervision_timeout = params->supervision_timeout,
    };
    errcode_t ret = sle_update_connect_param(&conn_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_update_connect_param failed!:%#X", ret);
    return XF_OK;

}

// 与指定地址设备进行连接
xf_err_t xf_sle_connect(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_connect_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_connect_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备断开连接
xf_err_t xf_sle_disconnect(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_disconnect_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_disconnect_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与所有设备断开连接
xf_err_t xf_sle_disconnect_all(void)
{
    errcode_t ret = sle_disconnect_all_remote_device();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_disconnect_all_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备进行配对
xf_err_t xf_sle_add_pair(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_pair_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_pair_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备取消配对
xf_err_t xf_sle_del_pair(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_remove_paired_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_remove_paired_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与所有设备取消配对
xf_err_t xf_sle_del_pair_all(void)
{
    errcode_t ret = sle_remove_all_pairs();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_remove_all_pairs failed!:%#X", ret);
    return XF_OK;
}

// 获取已配对的设备
xf_err_t xf_sle_get_pair_list(
    uint16_t *max_num, xf_sle_addr_t *dev_list)
{
    sle_addr_t *ws63_dev_list = (sle_addr_t *)xf_malloc(sizeof(sle_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");
    errcode_t ret = sle_get_paired_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "sle_get_paired_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_SLE_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 获取已绑定的设备
xf_err_t xf_sle_get_bond_list(
    int *max_num, xf_sle_addr_t *dev_list)
{
    sle_addr_t *ws63_dev_list = (sle_addr_t *)xf_malloc(sizeof(sle_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");
    errcode_t ret = sle_get_bonded_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "sle_get_bonded_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_SLE_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 设置 PHY 参数
xf_err_t xf_sle_set_phy_params(uint16_t conn_id, xf_sle_set_phy_t *sle_phy)
{
    sle_set_phy_t _sle_phy = {
        .tx_phy = sle_phy->tx_phy,
        .rx_phy = sle_phy->rx_phy,
        .tx_format = sle_phy->tx_format,
        .rx_format = sle_phy->rx_format,
        .tx_pilot_density = sle_phy->tx_pilot_density,
        .rx_pilot_density = sle_phy->rx_pilot_density,
        .t_feedback = sle_phy->t_feedback,
        .g_feedback = sle_phy->g_feedback,
    };
    errcode_t ret = sle_set_phy_param(conn_id, &_sle_phy);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_phy_param failed!:%#X", ret);
    return XF_OK;
}

// 获取对端设备 RSSI
xf_err_t xf_sle_get_peer_rssi(uint16_t conn_id, int8_t *rssi);

// 设置 调制与编码策略（  Modulation and Coding Scheme ）
xf_err_t xf_sle_set_mcs(uint16_t conn_id, uint8_t mcs)
{
    errcode_t ret = sle_set_mcs(conn_id, mcs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_mcs failed!:%#X", ret);
    return XF_OK;
}

// 设置连接链路上所偏好的最大传输 payload 字节数
xf_err_t xf_sle_set_data_len(uint16_t conn_id, uint16_t tx_octets)
{
    errcode_t ret = sle_set_data_len(conn_id, tx_octets);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_data_len failed!:%#X", ret);
    return XF_OK;
}

// 设置 sle 默认连接参数
xf_err_t xf_sle_set_default_conn_params(
    xf_sle_conn_param_def_t *conn_param_def)
{
    sle_default_connect_param_t _conn_param_def = {
        .enable_filter_policy   = conn_param_def->en_filter,
        .gt_negotiate           = conn_param_def->en_gt_negotiate,
        .initiate_phys          = conn_param_def->scan_phy,
        .max_interval           = conn_param_def->interval_max,
        .min_interval           = conn_param_def->interval_min,
        .scan_interval          = conn_param_def->scan_interval,
        .scan_window            = conn_param_def->scan_window,
        .timeout                = conn_param_def->timeout,
    };
    errcode_t ret = sle_default_connection_param_set(&_conn_param_def);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_default_connection_param_set failed!:%#X", ret);
    return XF_OK;
}

// 配置定制化信息
xf_err_t xf_sle_set_max_pwr(int8_t ble_pwr, int8_t sle_pwr)
{
    errcode_t ret = sle_customize_max_pwr(ble_pwr, sle_pwr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_customize_max_pwr failed!:%#X", ret);
    return XF_OK;
}


#define PORT_NV_ID_BTH_BLE_NV_RESERVERD     (0xE)

#define GLE_PWR_LEVEL_GFSK_MAX_PWR          (8)
#define GLE_PWR_LEVEL_GFSK_MIN_PWR          (-12)

#define SLE_PWR_LEVEL_PSK_MAX_PWR           (2)
#define SLE_PWR_LEVEL_PSK_MIN_PWR           (-16)

// typedef union _port_bth_ble_nv_reserved_t port_bth_ble_nv_reserved_t;

typedef struct
{
    uint8_t _reserve    :6;
    uint8_t sle_psk     :1;
    uint8_t gle_gfsk    :1;
} gle_customize_pwr_en_t;

typedef union _port_bth_ble_nv_reserved_t
{
    bth_ble_nv_reserved_struct_t _bth_ble_nv_reserved;
    struct 
    {
        gle_customize_pwr_en_t gle_customize_pwr_en;
        uint8_t gle_gfsk_max_pwr;
        uint8_t sle_psk_max_pwr;
        // uint8_t _reserve[BTH_BLE_RESERVE_LEN - xf_offsetof(union _port_bth_ble_nv_reserved_t, _reserve)];
        uint8_t _reserve[BTH_BLE_RESERVE_LEN - 4];
    };
    
} port_bth_ble_nv_reserved_t;

static port_bth_ble_nv_reserved_t s_gle_nv_cfg = {
    .gle_customize_pwr_en = 
    {
        .gle_gfsk = true,
        .sle_psk = true,
    },
    .gle_gfsk_max_pwr = GLE_PWR_LEVEL_GFSK_MAX_PWR,
    .sle_psk_max_pwr = SLE_PWR_LEVEL_PSK_MAX_PWR,
};

// 配置发射功率档位（根据指定的发射功率）
xf_err_t xf_sle_set_max_pwr_level_by_pwr(int8_t target_max_pwr)
{
    /* 如果预期功率 > 最大功率档位功率 (gle_gfsk pwr max) -> 返回错误 */
    XF_ASSERT(target_max_pwr <= GLE_PWR_LEVEL_GFSK_MAX_PWR, XF_ERR_INVALID_ARG,
        TAG, "target_max_pwr(%d) > MAX(%d)", target_max_pwr, GLE_PWR_LEVEL_GFSK_MAX_PWR);

    /* 如果预期功率 < 最小功率档位功率 (gle_psk pwr min) -> 返回错误码 */
    XF_ASSERT(target_max_pwr >= SLE_PWR_LEVEL_PSK_MIN_PWR, XF_ERR_INVALID_ARG,
        TAG, "target_max_pwr(%d) < MIN(%d)", target_max_pwr, SLE_PWR_LEVEL_PSK_MIN_PWR);


    /* 如果预期功率 >= sle psk 最大档位 -> sle_psk 功率档位设置为最大， gle_gfsk 设置为预期值 */
    if(target_max_pwr >= SLE_PWR_LEVEL_PSK_MAX_PWR)
    {
        s_gle_nv_cfg.sle_psk_max_pwr = SLE_PWR_LEVEL_PSK_MAX_PWR;
        s_gle_nv_cfg.gle_gfsk_max_pwr = target_max_pwr;
    }

    /* 如果预期功率 <= sle gfsk 最小档位 -> gle_gfsk 功率档位设置为最大， sle_psk 设置为预期值 */
    else if(target_max_pwr <= GLE_PWR_LEVEL_GFSK_MIN_PWR)
    {
        s_gle_nv_cfg.gle_gfsk_max_pwr = GLE_PWR_LEVEL_GFSK_MIN_PWR;
        s_gle_nv_cfg.sle_psk_max_pwr = target_max_pwr;
    }
    
    errcode_t nv_ret = uapi_nv_write(PORT_NV_ID_BTH_BLE_NV_RESERVERD, 
        (uint8_t *)&s_gle_nv_cfg, sizeof(s_gle_nv_cfg));
    XF_CHECK(nv_ret != ERRCODE_SUCC, XF_ERR_INVALID_ARG, TAG, "WRITE NV_ID_BTC_TXPOWER_CFG failed");
}

/* ==================== [Static Functions] ================================== */

#endif  // CONFIG_XF_SLE_ENABLE