import os
import sys

def get_target_build_lists(target, port_rel_path):
    # 路径转换
    def make_relative_path(entries):
        return [os.path.join(port_rel_path, target, p) for p in entries]

    # 加载目标配置
    try:
        port_root = os.path.dirname(os.path.abspath(__file__))
        target_dir = os.path.join(port_root, target)
        sys.path.insert(0, target_dir)
        from target_build_lists import target_build_lists
    except ImportError:
        raise ImportError(f"目标 {target} 缺少 target_build_lists.py")

    return {
        "srcs": make_relative_path(target_build_lists.get("srcs", [])),
        "incs": make_relative_path(target_build_lists.get("incs", [])),
        "reqs": target_build_lists.get("reqs", []),
        "cflags": target_build_lists.get("cflags", [])
    }
