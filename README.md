<div align="center">
<h1>BetterQQNT-Launcher</h1>
<p><a href="https://github.com/BetterQQNT/BetterQQNT" target="_blank">BetterQQNT</a> 启动器</p>
</div>

## 使用

1. 前往 [Releases](https://github.com/BetterQQNT/BetterQQNT-Launcher/releases) 下载 `bqqenv.exe`
1. 关闭电脑上正在运行的 QQ
1. 双击运行 `bqqenv.exe`

## 编译

```sh
./build.sh
```

自定义使用的 BetterQQNT 版本：

```sh
export BETTERQQNT_VERSION=0.1.0

# 或
export BETTERQQNT_VERSION_X86=0.1.0
export BETTERQQNT_VERSION_X64=0.1.0

./build.sh
```

## 兼容性

已在 64 位系统上测试 32 位和 64 位 QQ
