相关错误处理记录：
1. libOpticalFlow.so Permission denied
```
CMake Error at cmake_install.cmake:52 (file):
  file INSTALL cannot copy file
  "/home/ubuntu/sourcecode/PX4-Autopilot/build/px4_sitl_default/OpticalFlow/src/OpticalFlow-build/libOpticalFlow.so"
  to "/usr/local/lib/libOpticalFlow.so": Permission denied.
```
解决方式：
```
sudo git submodule update --recursive
sudo make distclean
```
