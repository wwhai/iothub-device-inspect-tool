# Iot设备调试工具
原计划用Qt5来做个专门调试设备的工具，包含MQTT、HTTP、TCP、UDP、CoAP等多种协议支持。

## 环境搭建
### Qt5
首先安装Qt5.10版本，其他版本没测。

### LIB编译
```sh
git clone https://github.com/wwhai/iothub-device-inspect-tool
```

上面完了以后把qmqtt的依赖下载下来
```
git clone https://github.com/emqx/qmqtt.git
cd qmqtt
qmake
make
make install
```

最后把编译后的lib加入到Qt5IDE配置中
