#!/bin/bash
#
# 快速构建、烧录和调试脚本
# PX4 + STM32F427 + OpenOCD + GDB
#
# 用法:
#   ./build_and_debug.sh build       # 仅构建
#   ./build_and_debug.sh flash       # 构建和烧录
#   ./build_and_debug.sh debug       # 构建、烧录并启动GDB调试
#   ./build_and_debug.sh serial      # 打开串口监控
#

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/kite_f427_default"
ELF_FILE="${BUILD_DIR}/kite_f427_default.elf"
BIN_FILE="${BUILD_DIR}/kite_f427_default.bin"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 函数：打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# 函数：检查工具是否可用
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        print_error "Tool not found: $1"
        return 1
    fi
    return 0
}

# 函数：检查OpenOCD是否运行
check_openocd() {
    if ! nc -z localhost 3333 > /dev/null 2>&1; then
        print_warning "OpenOCD is not running on port 3333"
        print_info "Please start OpenOCD in another terminal:"
        print_info "  openocd -f openocd_stm32f427.cfg"
        return 1
    fi
    return 0
}

# 函数：构建固件
build_firmware() {
    print_info "Building PX4 firmware for kite_f427..."
    cd "${PROJECT_ROOT}"
    
    if make kite_f427_default -j4; then
        print_success "Build completed successfully"
        print_info "ELF file: ${ELF_FILE}"
        print_info "BIN file: ${BIN_FILE}"
        return 0
    else
        print_error "Build failed"
        return 1
    fi
}

# 函数：烧录固件
flash_firmware() {
    print_info "Flashing firmware via OpenOCD..."
    
    if ! check_openocd; then
        return 1
    fi
    
    if [ ! -f "${ELF_FILE}" ]; then
        print_error "ELF file not found: ${ELF_FILE}"
        return 1
    fi
    
    print_info "Starting GDB flash process..."
    arm-none-eabi-gdb -batch \
        -ex "target extended-remote :3333" \
        -ex "monitor reset halt" \
        -ex "load" \
        -ex "monitor reset init" \
        -ex "quit" \
        "${ELF_FILE}" 2>&1 | while IFS= read -r line; do
            if [[ "$line" == *"Error"* ]] || [[ "$line" == *"error"* ]]; then
                print_error "$line"
            else
                print_info "$line"
            fi
        done
    
    print_success "Flashing completed"
    return 0
}

# 函数：启动GDB调试
start_gdb_debug() {
    print_info "Starting GDB debug session..."
    
    if ! check_openocd; then
        return 1
    fi
    
    if [ ! -f "${ELF_FILE}" ]; then
        print_error "ELF file not found: ${ELF_FILE}"
        return 1
    fi
    
    print_info "Opening GDB with debug script..."
    cd "${BUILD_DIR}"
    arm-none-eabi-gdb -x "${PROJECT_ROOT}/debug.gdb" "${ELF_FILE}"
    
    return $?
}

# 函数：打开串口监控
open_serial_monitor() {
    local port=""
    
    # 查找USB串口
    if [ -e /dev/ttyACM0 ]; then
        port="/dev/ttyACM0"
    elif [ -e /dev/ttyUSB0 ]; then
        port="/dev/ttyUSB0"
    else
        print_error "No USB serial port found"
        print_info "Available devices:"
        ls /dev/tty* | grep -E "(ACM|USB)" || print_warning "No TTY devices"
        return 1
    fi
    
    print_info "Opening serial monitor on ${port}..."
    print_warning "Press Ctrl+A, Ctrl+X to exit picocom"
    
    if command -v picocom &> /dev/null; then
        sudo picocom -b 115200 "${port}"
    elif command -v minicom &> /dev/null; then
        sudo minicom -D "${port}"
    else
        print_error "No serial terminal tool found (picocom or minicom)"
        print_info "Install with: sudo apt-get install picocom"
        return 1
    fi
}

# 函数：显示菜单
show_menu() {
    echo ""
    echo "=========================================="
    echo "PX4 Build & Debug Utility"
    echo "=========================================="
    echo "Usage: $0 <command>"
    echo ""
    echo "Commands:"
    echo "  build    - Build firmware only"
    echo "  flash    - Build and flash firmware"
    echo "  debug    - Build, flash and debug with GDB"
    echo "  serial   - Open serial monitor"
    echo "  help     - Show this menu"
    echo ""
    echo "Prerequisites:"
    echo "  - OpenOCD running: openocd -f openocd_stm32f427.cfg"
    echo "  - ST-Link connected via USB"
    echo "=========================================="
    echo ""
}

# 主逻辑
main() {
    local command="${1:-help}"
    
    case "$command" in
        build)
            build_firmware
            ;;
        flash)
            build_firmware && flash_firmware
            ;;
        debug)
            build_firmware && flash_firmware && start_gdb_debug
            ;;
        serial)
            open_serial_monitor
            ;;
        help|--help|-h)
            show_menu
            ;;
        *)
            print_error "Unknown command: $command"
            show_menu
            exit 1
            ;;
    esac
}

# 检查依赖工具
check_tool "arm-none-eabi-gcc" || exit 1
check_tool "arm-none-eabi-gdb" || exit 1
check_tool "make" || exit 1
check_tool "cmake" || exit 1

# 运行主函数
main "$@"
