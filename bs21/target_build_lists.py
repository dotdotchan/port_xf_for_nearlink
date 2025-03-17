#
# note:
#   - target_build_lists 内的 srcs 和 incs 路径都是相对 target_build_lists.py 的路径。
#   - target_build_lists 不使用大写，有可能需要根据宏配置情况改变。
#

target_build_lists = {
    # 源码文件列表
    "srcs": [
        "platform_def/*.c",
        "port_utils/*.c",
        "port_xf/*.c",
        "port_xf/port_xf_ble/*.c",
        "port_xf/port_xf_hal/*.c",
        "port_xf/port_xf_log/*.c",
        # "port_xf/port_xf_ota/*.c",
        "port_xf/port_xf_ringbuf/*.c",
        "port_xf/port_xf_sle/*.c",
        "port_xf/port_xf_sys/*.c",
        # "port_xf/port_xf_wifi/*.c", # 不支持 wifi
    ],
    # 包含路径列表
    "incs": [
        "platform_def",
        "port_utils",
        "port_xf",
        "port_xf/port_xf_ble",
        "port_xf/port_xf_hal",
        "port_xf/port_xf_log",
        # "port_xf/port_xf_ota",
        "port_xf/port_xf_ringbuf",
        "port_xf/port_xf_sle",
        "port_xf/port_xf_sys",
        # "port_xf/port_xf_wifi", # 不支持 wifi
    ],
    # 需求组件列表
    "reqs": [
        "xf_utils",
    ],
    # 构建标志列表
    "cflags": [
    ],
}
