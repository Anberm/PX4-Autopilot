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
sudo chmod 777 -R $PWD
git submodule update --recursive
make distclean

```

2.build_git_version.h --validate --git_tag 'v0.0.0'
```
subprocess.CalledProcessError: Command '['git', 'rev-parse', '--verify', 'HEAD']' returned non-zero exit status 128.
```
解决方式：
Git 2.35.2+ 版本引入了安全特性，会检测到这种"可疑的所有权"（dubious ownership）并拒绝执行 git 命令,
解决方式依旧是
```
sudo chmod 777 -R $PWD
git submodule update --recursive
make distclean
```
