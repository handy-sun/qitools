# qitools — Personal Common Tools Cross Platform Programs

Qt5 C++ 桌面工具集，插件化架构，跨平台（Windows / macOS / Linux）。

## 构建系统

**双构建系统**：CMake（主）+ qmake（备）

### CMake 构建

```bash
cd ~/work/qitools
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

输出目录：`bin/`，可执行文件命名 `qitools-<compiler>[-d]`

CMake 选项：
- `QITOOLS_MAC_BUNDLE` (ON) — macOS 下自动打包为 .app
- `QITOOLS_LINUX_RPATH` (OFF) — Linux 下启用 rpath

### qmake 构建

```bash
cd ~/work/qitools
qmake qitools.pro
make -j$(nproc)
```

## 依赖

- **Qt5**: Core, Gui, Widgets, LinguistTools
- **C++11** 起

## 项目结构

```
CMakeLists.txt          ← 顶层，add_subdirectory(src)
qitools.pro / .pri      ← qmake 构建
CMake/                  ← CMake 模块（qitools_common, get_depends, pch_gcc4）
resource/               ← 图标(.ico/.icns/.svg), qrc, css, rc
bin/                    ← 构建输出 + translations/*.qm
scripts/                ← 辅助脚本（打包、CMake presets 生成、deployqt）
src/
  CMakeLists.txt        ← 版本 0.15.0，定义所有子模块
  core/                 ← 主程序（main.cpp, qitoolswindow, pluginmanager, pluginterface）
  codecconvert/         ← 编码转换工具
  cppcodegen/           ← C++ 代码生成器（property generator）
  colortool/            ← 颜色工具（屏幕取色、颜色转换）
  simpledownload/       ← 简易下载器
  audio/                ← 音频工具（minimp3, wavfile, ID3v2）
  imageconvert/         ← 图片转换
  glsltool/             ← GLSL 代码转换
```

## 架构

- **插件化**：core 是主程序，各工具是动态插件（.dll/.dylib/.so）
- 插件接口：`pluginterface.h`
- 插件管理：`pluginmanager.h/cpp`
- 插件输出路径：`bin/plugins/`（macOS bundle 内为 `*.app/Contents/MacOS/plugins/`）
- 国际化：每个模块有 `*-zh_CN.ts` 翻译文件，编译后输出到 `bin/translations/`

## 注意事项

- C++ 标准默认 C++11，可通过 `CMAKE_CXX_STANDARD` 覆盖
- Windows 下 core 使用 `.rc` 文件设置图标
- macOS 下 core 使用 MACOSX_BUNDLE 打包
- GCC 下启用预编译头（stable.h）
- 版本号在 `src/CMakeLists.txt` 的 `project()` 中定义
