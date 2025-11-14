#!/usr/bin/env bash
set -euo pipefail

# 用法：
#   scripts/gen_compile_commands.sh <board> [config]
# 示例：
#   scripts/gen_compile_commands.sh kite_h743-v2 default
#   scripts/gen_compile_commands.sh px4_sitl
#
# 说明：在工程根执行本脚本，将在 build/<board>_<config> 生成编译数据库并软链到 compile_commands.json

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BOARD="${1:-px4_sitl}"
CONFIG="${2:-default}"

GEN_DB_ARGS=(-G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

pushd "${ROOT}" >/dev/null

if [[ "${BOARD}" == "px4_sitl" ]]; then
  BUILD_DIR="${ROOT}/build/px4_sitl"
  mkdir -p "${BUILD_DIR}"
  cmake -S . -B "${BUILD_DIR}" "${GEN_DB_ARGS[@]}" -DPX4_BOARD=px4_sitl
  ninja -C "${BUILD_DIR}" px4_sitl
else
  FULL_CONFIG="${BOARD}_${CONFIG}"
  BUILD_DIR="${ROOT}/build/${FULL_CONFIG}"
  mkdir -p "${BUILD_DIR}"
  cmake -S . -B "${BUILD_DIR}" "${GEN_DB_ARGS[@]}" -DCONFIG="${FULL_CONFIG}"
  ninja -C "${BUILD_DIR}" px4
fi

ln -snf "${BUILD_DIR}/compile_commands.json" "${ROOT}/compile_commands.json"
echo "compile_commands.json -> ${BUILD_DIR}/compile_commands.json"
popd >/dev/null

echo "Done. Please reload VS Code to re-index."


