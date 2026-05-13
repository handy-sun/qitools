# qitools — Personal Common Tools Cross Platform Programs

Qt C++ 桌面工具集，插件化架构，跨平台（Windows / macOS / Linux）。

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

Qt 选择策略：
- CMake 优先查找 Qt6
- 未找到 Qt6 时回退到 Qt5.15

### qmake 构建

```bash
cd ~/work/qitools
qmake qitools.pro
make -j$(nproc)
```

## 依赖

- **Qt6 或 Qt5.15**: Core, Gui, Widgets, LinguistTools
- **C++17** 默认，可通过 `CMAKE_CXX_STANDARD` 覆盖

## 项目结构

```
CMakeLists.txt          ← 顶层，add_subdirectory(src)
qitools.pro / .pri      ← qmake 构建
CMake/                  ← CMake 模块（qitools_common, get_depends, pch_gcc4）
resource/               ← 图标(.ico/.icns/.svg), qrc, css, rc
bin/                    ← 构建输出 + translations/*.qm
scripts/                ← 辅助脚本（打包、CMake presets 生成、deployqt）
src/
  CMakeLists.txt        ← 版本 0.2.0，定义所有子模块
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
- 主窗口通过插件名构建左侧列表，通过插件 `QWidget` 构建右侧工具页

## 运行时约定

- 用户配置目录：`~/.config/qitools/`
- 主配置文件：`qitools.ini`
- 可选日志文件：`qitools.log`
- 样式表默认来自内置资源 `:/QiTools.css`
- 启动时会加载 `translations/*.qm`

## CI / 发布

- `master` 分支使用 Qt5 构建发布矩阵：Windows x64/x86、Linux x64/ARM64、macOS x86_64
- `qt6` 分支使用 Qt6 构建发布矩阵：Windows x64、Linux x64/ARM64、macOS x86_64
- 打 tag `v*` 时，工作流会汇总平台产物并创建 draft release

## 注意事项

- CMake 默认使用 C++17，可通过 `CMAKE_CXX_STANDARD` 覆盖
- CMake 的 Qt5 / Qt6 兼容逻辑集中在 `CMake/qt_compatibility.cmake`
- Windows 下 core 使用 `.rc` 文件设置图标
- macOS 下 core 使用 MACOSX_BUNDLE 打包
- GCC 下启用预编译头（stable.h）
- 版本号在 `src/CMakeLists.txt` 的 `project()` 中定义
