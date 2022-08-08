# Gnome-shell Chinese Lunar Calendar extension (gnome-shell 中国农历扩展)

网上目前的扩展缺少文件，本扩展包含了所有需要的源文件，请从源码构建。


## install 安装方法

### 1. install dependency 安装依赖库

```
	cd glunar
	meson build
	cd build
	sudo meson install
	#如果扩展不能正常运行，执行如下命令
	sudo cp /usr/local/lib64/liblunar.so /usr/lib64/
	sudo cp /usr/local/lib64/girepository-1.0/GILunar-1.0.typelib /usr/lib64/girepository-1.0/
```

### 2. install gnome-shell-extension 安装Gnome-shell扩展

```
	cd ../../lunarcal@jmchxy
	make install
```
