# Gnome-shell Chinese Lunar Calendar extension (gnome-shell 中国农历扩展)

网上目前的扩展缺少文件，本扩展包含了所有需要的源文件，请从源码构建。


## install 安装方法

### 1. install dependency 安装依赖库

```
	cd glunar
	meson build
	cd build
	sudo meson install
```

### 2. install gnome-shell-extension 安装Gnome-shell扩展

```
	cd ../../lunarcal@jmchxy
	make install
```
