# 桌面宠物 && ToDoList 😀

> 灵感来源：网上的很多 todo-list 软件不好用，不够便捷。  
> 一个 Qt6 桌面宠物 + 待办清单小工具，支持托盘隐藏、任务管理、版本检查等功能。

---

## 功能

- 🐱 桌面宠物小窗口，支持拖拽移动。
- ✅ ToDoList 任务展示、添加、删除。
- ☑️ 勾选完成任务后隐藏窗口自动清理。
- 🔔 系统托盘图标，右键快速添加/隐藏/检查更新/退出。
- 🌐 异步版本检查与更新提示。

---

## 项目结构

```
DesktopPet
├── core/               # 领域模型与数据持久化
│   ├── event.h              # Event 领域模型
│   ├── settings_interface.h # 配置存储抽象
│   ├── qsettings_adapter.h/.cpp  # QSettings 适配器
│   ├── config_store.h/.cpp       # 事件序列化/反序列化
│   └── event_repository.h/.cpp   # 事件仓储
├── services/           # 公共服务
│   └── update_service.h/.cpp     # 异步版本检查
├── ui/                 # 窗口与 Qt .ui 文件
│   ├── main_window.h/.cpp/.ui    # 主窗口/宠物/托盘
│   ├── task_list_dialog.h/.cpp/.ui   # 任务列表窗口（原 Form）
│   └── add_dialog.h/.cpp/.ui     # 添加/编辑任务窗口（原 AddDialog）
├── main.cpp            # 依赖装配入口
├── config/
│   └── eventlist.ini   # 任务配置文件
└── image/              # 图片与 GIF 资源
```

---

## 构建

### 环境要求

- Qt 6（或 Qt 5，CMakeLists 做了兼容）
- CMake 3.5+
- MinGW / MSVC / 其他 Qt 支持的编译器

### 使用 MinGW

```bash
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

运行：

```bash
./build/desktop_todo.exe
```

程序使用 `../config/eventlist.ini` 保存任务数据，请从项目根目录或 build 目录运行。

---

## 使用方式

- **添加事件**：在系统托盘图标上右键，选择 `Add`。
- **显示任务列表**：在桌面宠物窗口上点击右键。
- **隐藏任务列表**：在桌面宠物窗口上点击左键并拖动，或再次点击左键。
- **完成任务**：在任务列表中勾选，隐藏窗口后自动删除已勾选任务。
- **检查更新**：托盘菜单选择 `Update`。
- **退出**：托盘菜单选择 `Quit`。

---

## 代码规范

本项目遵循 **Google C++ Style**：

- 文件名：`lower_with_underscore.cpp/.h/.ui`
- 类名：`UpperCamelCase`
- 成员变量：以尾部下划线结尾，例如 `event_repository_`
- 函数名：`UpperCamelCase`
- 枚举/常量：`kUpperCamelCase`
- 头文件包含守卫：`DESKTOP_TODO__<PATH>_<FILE>_H_`

具体约束见 `AGENTS.md`。

---

## 待办/未来计划

### ToDoList

- [ ] 根据任务重要性显示不同颜色
- [ ] 任务提醒
- [ ] 番茄时钟

### Desktop Pet

- [ ] 自定义图标和宠物样式
- [ ] 整点报时
- [ ] 微信消息提醒
- [ ] 提醒喝水、偶尔弹出消息等互动

---

## 技术栈

- Qt 6 Widgets
- Qt Network（版本检查）
- CMake
- QSettings / INI 持久化

---

## License

见 `LICENSE` 文件。
