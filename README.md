# port_xf_for_nearlink

## 已支持

-   [x] ws63
-   [x] bs21

## 使用方法

此仓库无法独立使用，需结合 XFusion 与 对应的 SDK 一起使用。

如需增删源码，需修改 `bs21/target_build_lists.py` 或 `ws63/target_build_lists.py` srcs、incs 即可。

注意，`target_build_lists.py` 的路径都是相对 target_build_lists.py 的路径。

## 详情

XFusion 通过执行 `ports/nearlink/ws63/xf_collect.py` 或 `ports/nearlink/bs21/xf_collect.py`，调用位于本仓库的 `port_xf_collect.py`，并根据 bs21 或 ws63 目标获取 `bs21/target_build_lists.py` 或 `ws63/target_build_lists.py`，从而获得 srcs、incs、reqs、cflags。

由于 xf_collect.py 不支持绝对路径，且要求 srcs、incs 是相对 xf_collect.py 的路径，目前仅在简单地 `bs21/target_build_lists.py` 或 `ws63/target_build_lists.py` 内的 srcs、incs 每个条目前拼接路径来满足此需求。
