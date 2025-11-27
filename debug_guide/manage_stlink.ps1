# PowerShell Script to manage ST-Link USB device for WSL
# Windows端USB设备管理脚本
#
# 使用方法:
#   1. 以管理员身份运行 PowerShell
#   2. 允许脚本执行: Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
#   3. 运行: .\manage_stlink.ps1 connect
#
# 命令:
#   connect    - 连接ST-Link到WSL
#   disconnect - 从WSL断开ST-Link
#   list       - 列出USB设备
#   help       - 显示帮助信息

param(
    [string]$command = "help",
    [string]$busId = "6-1"
)

# 颜色定义
function Write-Info {
    param([string]$message)
    Write-Host "[INFO] $message" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$message)
    Write-Host "[SUCCESS] $message" -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$message)
    Write-Host "[ERROR] $message" -ForegroundColor Red
}

function Write-Warning-Custom {
    param([string]$message)
    Write-Host "[WARNING] $message" -ForegroundColor Yellow
}

# 检查管理员权限
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# 列出USB设备
function List-Devices {
    Write-Info "Listing all USB devices..."
    Write-Info "Looking for ST-Link (VID: 0483)..."
    Write-Host ""
    
    usbipd list
    
    Write-Host ""
    Write-Info "To find your ST-Link, look for one of these patterns:"
    Write-Info "  - 0483:3748 (ST-Link/V2)"
    Write-Info "  - 0483:374b (ST-Link/V2-1)"
    Write-Info "  - 0483:374d (ST-Link/V2-1 Mass Storage)"
}

# 连接设备
function Connect-Device {
    param([string]$id)
    
    Write-Info "Checking if device $id exists..."
    $device = usbipd list | Select-String $id
    
    if (-not $device) {
        Write-Error-Custom "Device $id not found"
        Write-Host ""
        List-Devices
        exit 1
    }
    
    Write-Info "Found device: $device"
    
    # 检查是否已绑定
    if ($device -like "*bound*") {
        Write-Info "Device is already bound"
    } else {
        Write-Info "Binding device $id..."
        usbipd bind --busid $id
        Write-Success "Device bound"
    }
    
    # 检查连接状态
    $status = usbipd list | Select-String $id
    if ($status -like "*attached*") {
        Write-Warning-Custom "Device is already attached to WSL"
    } else {
        Write-Info "Attaching device $id to WSL..."
        # 新版本USBIPD-WIN (v4.0+) 使用attach替代connect
        usbipd attach --wsl --busid $id
        Write-Success "Device attached to WSL"
    }
    
    Write-Host ""
    Write-Info "You can now use OpenOCD in WSL:"
    Write-Info "  openocd -f openocd_stm32f427.cfg"
}

# 断开设备
function Disconnect-Device {
    param([string]$id)
    
    Write-Info "Detaching device $id from WSL..."
    # 新版本USBIPD-WIN (v4.0+) 使用detach替代disconnect
    usbipd detach --busid $id
    Write-Success "Device detached"
    
    Write-Host ""
    Write-Info "To remove binding later, run:"
    Write-Info "  usbipd unbind --busid $id"
}

# 显示帮助
function Show-Help {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "ST-Link USB Management for WSL" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage: .\manage_stlink.ps1 <command> [options]"
    Write-Host ""
    Write-Host "Commands:"
    Write-Host "  connect [busid]      Attach ST-Link to WSL (default: 6-1)"
    Write-Host "  disconnect [busid]   Detach ST-Link from WSL (default: 6-1)"
    Write-Host "  list                 List all USB devices"
    Write-Host "  help                 Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\manage_stlink.ps1 connect"
    Write-Host "  .\manage_stlink.ps1 connect 5-2"
    Write-Host "  .\manage_stlink.ps1 disconnect"
    Write-Host "  .\manage_stlink.ps1 list"
    Write-Host ""
    Write-Host "Requirements:"
    Write-Host "  - Run as Administrator"
    Write-Host "  - Windows 11 or later"
    Write-Host "  - USBIPD-WIN v4.0+ installed"
    Write-Host "  - WSL2 installed with Ubuntu"
    Write-Host ""
    Write-Host "Installation of USBIPD-WIN:"
    Write-Host "  winget install usbipd"
    Write-Host ""
    Write-Host "Check version: usbipd --version"
    Write-Host ""
    Write-Host "Version notes:"
    Write-Host "  - v4.0+: Use 'attach --wsl' and 'detach' commands"
    Write-Host "  - v3.x: Use 'connect' and 'disconnect' commands"
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
}

# 主程序
function Main {
    # 检查管理员权限
    if (-not (Test-Administrator)) {
        Write-Error-Custom "This script must be run as Administrator"
        Write-Info "Please run PowerShell as Administrator and try again"
        exit 1
    }
    
    # 检查usbipd
    try {
        $null = usbipd --version
    } catch {
        Write-Error-Custom "USBIPD-WIN is not installed or not in PATH"
        Write-Info "Install it using: winget install usbipd"
        exit 1
    }
    
    # 处理命令
    switch ($command) {
        "connect" {
            Connect-Device -id $busId
        }
        "disconnect" {
            Disconnect-Device -id $busId
        }
        "list" {
            List-Devices
        }
        "help" {
            Show-Help
        }
        default {
            Write-Error-Custom "Unknown command: $command"
            Show-Help
            exit 1
        }
    }
}

# 执行主程序
Main
