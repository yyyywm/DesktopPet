# Agent 工作规范

本文件为指导 AI 编码助手在 DesktopPet 项目中工作的规范。  
当你（AI 助手）处理本项目的代码、构建、文档或相关任务时，请遵循以下约定。

---

## 项目概述

- **项目类型**：Qt6 桌面应用（C++）。
- **功能**：桌面宠物 + ToDoList，支持系统托盘、任务管理、版本检查。
- **构建工具**：CMake。
- **代码规范**：Google C++ Style。

---

## 目录结构约定

```
DesktopPet
├── core/          # 领域模型与数据持久化（不依赖 Qt Widgets）
├── services/      # 跨模块公共服务
├── ui/            # 窗口、对话框、.ui 文件
├── main.cpp       # 依赖装配入口
├── config/        # .ini 配置文件
├── image/         # 图片与 GIF 资源
├── README.md      # 用户/贡献者文档
└── AGENTS.md      # 本文件
```

### 各目录职责

- `core/`：纯业务逻辑。禁止在此处引入 `QWidget` 相关头文件。
- `services/`：网络、版本检查、通知等公共服务。
- `ui/`：只处理展示和用户输入。禁止直接读写 `QSettings` 或全局变量。
- `app`（`main.cpp`）：创建 `core`/`services` 实例并注入到 `ui`。

---

## 编码规范

统一遵循 [Google C++ Style](https://google.github.io/styleguide/cppguide.html)：

- **文件命名**：`lower_with_underscore.cpp/.h/.ui`
- **类/结构体命名**：`UpperCamelCase`
- **成员变量**：尾部下划线，例如 `event_repository_`
- **函数/方法**：`UpperCamelCase`
- **局部变量**：`lower_with_underscore`
- **常量/枚举**：`kUpperCamelCase`
- **枚举类**：`EnumClassName::kValueName`
- **头文件包含守卫**：
  ```cpp
  #ifndef DESKTOP_TODO__CORE__EVENT_H_
  #define DESKTOP_TODO__CORE__EVENT_H_
  // ...
  #endif  // DESKTOP_TODO__CORE__EVENT_H_
  ```

---

## 架构约束

1. **禁止全局可变状态**。所有共享状态通过依赖注入传递（如 `EventRepository*`）。
2. **禁止 UI 直接操作 `QSettings`**。配置读写统一通过 `core::ConfigStore`。
3. **禁止 UI 层直接持有裸资源生命周期**。尽量使用 `std::unique_ptr` 或 Qt 父子对象机制。
4. **网络请求必须异步**。避免在主线程使用 `QEventLoop` 阻塞 UI。
5. **新增功能前先找对应模块**：
   - 数据模型/持久化 → `core/`
   - 网络/跨模块服务 → `services/`
   - 窗口/交互 → `ui/`
   - 对象装配 → `main.cpp`

---

## 构建与验证

### 构建命令（MinGW）

```bash
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

### 提交前检查

- [ ] `cmake --build build` 成功，无新增编译警告/错误。
- [ ] 未提交构建产物（`build/`、`cmake-build-*` 已在 `.gitignore` 中）。
- [ ] 未提交 Qt Creator 用户设置（`CMakeLists.txt.user`）。
- [ ] 提交信息语义清晰，使用中文或英文均可，推荐前缀：
  - `feat:` 新功能
  - `fix:` 修复
  - `refactor:` 重构
  - `docs:` 文档
  - `chore:` 杂项/配置

---

## 扩展指南

### 新增一个任务字段

1. 修改 `core/event.h` 中的 `Event` 类。
2. 在 `core/config_store.cpp` 中处理该字段的序列化/反序列化（保持 `.ini` 兼容）。
3. 在 `ui/` 中需要展示的窗口更新界面逻辑。

### 新增一个公共服务

1. 在 `services/` 下新增类，继承 `QObject` 并使用信号返回结果。
2. 在 `main.cpp` 中创建实例并注入给需要的 UI 类。
3. UI 类通过信号槽与服务交互。

### 新增一个窗口

1. 在 `ui/` 下新增 `.h/.cpp/.ui` 文件，文件名使用 `lower_with_underscore`。
2. 类名使用 `UpperCamelCase`。
3. 窗口所需数据通过构造函数注入，禁止直接访问全局状态。
4. 在 `CMakeLists.txt` 的 `PROJECT_SOURCES` 中添加文件。

---

## 注意事项

- 本项目没有自动化测试。每次修改后请手动跑通核心流程：
  1. 启动后宠物窗口正常显示、可拖拽。
  2. 右键宠物显示任务列表，勾选任务后隐藏能正确删除。
  3. 托盘 Add 打开编辑窗口，能添加、删除、确定、取消。
  4. 重启程序后任务状态与上次一致。
- 程序运行时依赖相对路径 `../config/eventlist.ini` 和 `../image/`。
- 修改 `AGENTS.md` 或 `README.md` 后，应同步更新另一份文档中的相关内容。

---

## 外部资源

- Qt 官方文档：https://doc.qt.io/
- Google C++ Style Guide：https://google.github.io/styleguide/cppguide.html
