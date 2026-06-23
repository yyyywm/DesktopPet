# DesktopPet QWidget → QML 重构设计文档

## 1. 背景与目标

当前 DesktopPet 使用 Qt Widgets（`QMainWindow`、`QDialog`、`QTableWidget` 等）实现桌面宠物、任务列表和添加对话框。为了提升 UI 的现代感、动画能力以及后续扩展性（自定义皮肤、番茄钟等），计划将 UI 层整体重构为 QML，同时保留现有 `core/` 和 `services/` 的业务逻辑。

**目标：**
- 完全移除 `ui/` 中的 QWidget 实现和 `.ui` 文件。
- 使用 Qt 6 QML + Qt Quick Controls 2 重新实现所有界面。
- 保持核心流程不变：宠物窗口、任务列表、添加/编辑任务、系统托盘、版本检查、INI 持久化。
- 遵循 `AGENTS.md` 的架构约束（依赖注入、禁止 UI 直接读写 `QSettings`、禁止全局状态）。

## 2. 关键决策

| 决策项 | 选择 | 原因 |
|--------|------|------|
| 范围 | 全部换成 QML | 用户要求完整重构，且现有 `core/` 已拆分清楚，适合整体替换。 |
| QML 引擎 | `QQmlApplicationEngine` | Qt 6 推荐做法，主入口由 QML 驱动，C++ 提供模型和服务。 |
| Qt 版本 | 仅 Qt 6 | 可以使用 `qt_add_qml_module`、SystemTrayIcon 等现代特性，简化构建。 |
| 视觉方向 | 现代轻量风格 | 圆角、阴影、卡片、微动画；借重构机会提升视觉体验。 |
| C++ / QML 协作 | 后端 Model 驱动 | `TaskListModel` 继承 `QAbstractListModel`，保持类型安全和可测试性。 |

## 3. 总体架构

```
main.cpp
  ├── QGuiApplication
  ├── QQmlApplicationEngine
  │     └── qml/main.qml
  │           ├── PetWindow.qml
  │           ├── TaskListDialog.qml
  │           └── AddDialog.qml
  ├── ApplicationController  (QObject, 注入为 context property "App")
  ├── TaskListModel          (QAbstractListModel, 注入为 context property "TaskModel")
  ├── EventRepository        (现有，数据源)
  ├── ConfigStore            (现有，INI 持久化)
  └── UpdateService          (现有，异步版本检查)
```

### 3.1 C++ 新增/调整

#### `ApplicationController`
- 职责：系统托盘、更新服务桥接、窗口生命周期控制、全局动作分发。
- 注入为 QML context property `App`。
- 关键信号：
  - `void OpenAddDialogRequested()` —— 托盘 “Add” 点击后通知 QML 打开添加对话框。
  - `void ShowPetWindowRequested()` —— 托盘图标激活后通知 QML 显示宠物窗口。
  - `void UpdateMessageRequested(QString title, QString body, QString url)` —— 版本检查结果通知 QML 显示弹窗或打开浏览器。
- 关键槽：
  - `void CheckForUpdates()` —— 触发 `UpdateService`。
  - `void Quit()` —— 退出应用。
  - `void HidePetWindow()` —— 隐藏宠物窗口。

#### `TaskListModel`（新）
- 继承 `QAbstractListModel`。
- 包装 `EventRepository*`，监听其数据变化并发出 `dataChanged` / `rowsInserted` / `rowsRemoved`。
- 角色：
  - `IdRole`（`id`）
  - `TextRole`（`text`）
  - `DoneRole`（`done`）
  - `PriorityRole`（`priority`）
- 提供 QML 可调用的方法：
  - `void addEvent(QString text)`
  - `void removeDoneEvents()`
  - `void setDone(int id, bool done)`
  - `void setText(int id, QString text)`
  - `void clear()`
  - `void save()`

#### `EventRepository`（调整）
- 新增数据变更通知信号，供 `TaskListModel` 连接：
  - `void EventAdded(int id)`
  - `void EventRemoved(int id)`
  - `void EventChanged(int id)`
  - `void EventsCleared()`
- 现有 `Load()` / `Save()` / `AddEvent()` / `RemoveEvent()` / `Clear()` / `FindEvent()` 保持兼容。

#### `UpdateService`
- 保持不变。`ApplicationController` 连接其信号并转发给 QML。

## 4. QML 结构

新增 `qml/` 目录，文件命名使用 `UpperCamelCase.qml`（QML 惯例）。

### 4.1 文件清单

| 文件 | 职责 |
|------|------|
| `main.qml` | 应用入口：创建 `PetWindow`、`TaskListDialog`、`AddDialog` 实例，处理全局信号。 |
| `PetWindow.qml` | 桌面宠物主窗口：无边框、置顶、透明背景、GIF 动画、拖拽移动、鼠标事件分发。 |
| `TaskListDialog.qml` | 任务列表弹窗：基于 `Dialog`，使用 `ListView` 展示任务，支持勾选和编辑。 |
| `AddDialog.qml` | 添加/编辑任务弹窗：基于 `Dialog`，可添加、删除、确认、取消。 |
| `Theme.qml` | 现代风格常量：颜色、圆角、阴影、间距、字体大小；作为 `pragma Singleton` 单例供全局使用。 |
| `TaskListItem.qml` | 任务列表单行组件：复选框 + 文本输入框。 |
| `IconButton.qml` | 通用图标/文字按钮组件。 |

### 4.2 关键 QML 实现细节

#### `PetWindow.qml`
```qml
Window {
    id: petWindow
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    width: 168
    height: 163

    AnimatedImage {
        id: petImage
        source: "../image/bear.gif"
        anchors.fill: parent
        playing: true
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            if (mouse.button === Qt.LeftButton) {
                taskListDialog.hide()
                // 开始拖拽逻辑
            } else if (mouse.button === Qt.RightButton) {
                taskListDialog.showAt(petWindow.x + petWindow.width, petWindow.y)
            }
        }
    }
}
```

#### `TaskListDialog.qml`
- 使用独立 `Window`（非 `Dialog`），以便精确定位在宠物窗口右侧。
- `ListView` 绑定 `TaskModel`。
- Delegate 使用 `TaskListItem`。
- 关闭 / 隐藏时调用 `TaskModel.removeDoneEvents()` 和 `TaskModel.save()`。
- “添加”按钮调用 `TaskModel.addEvent("")`，并进入编辑状态。

#### `AddDialog.qml`
- 本地维护一个临时任务列表（从 `TaskModel` 复制）。
- 确认时：清空 `TaskModel`，按临时列表重新添加，调用 `TaskModel.save()`。
- 取消时：直接关闭，不保存。

## 5. 系统托盘

保留 C++ `QSystemTrayIcon`，原因：
- QML 的 `SystemTrayIcon` 在 Qt 6.8 才引入，为了兼容性和功能完整，继续用 C++。
- 托盘菜单项：`Hide`、`Add`、`Update`、`Quit`。

`ApplicationController` 负责创建托盘图标和菜单，点击后通过信号通知 QML。

## 6. 构建系统变更

### 6.1 `CMakeLists.txt` 调整

- 替换 `find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)` 为：
  ```cmake
  find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick QuickControls2 Network)
  ```
- 链接库替换为：
  ```cmake
  target_link_libraries(desktop_todo PRIVATE
      Qt6::Core
      Qt6::Qml
      Qt6::Quick
      Qt6::QuickControls2
      Qt6::Network
  )
  ```
- 使用 `qt_add_qml_module` 将 `qml/` 目录注册为 QML 模块：
  ```cmake
  qt_add_qml_module(desktop_todo
      URI DesktopPet
      VERSION 1.0
      QML_FILES
          qml/main.qml
          qml/PetWindow.qml
          qml/TaskListDialog.qml
          qml/AddDialog.qml
          qml/Theme.qml
          qml/TaskListItem.qml
          qml/IconButton.qml
      RESOURCE_PREFIX "/"
  )
  ```
- 删除 `ui/main_window.*`、`ui/task_list_dialog.*`、`ui/add_dialog.*` 及 `set(CMAKE_AUTOUIC ON)`。
- `ioc.o` 是当前 `Desktop.rc` 编译出的 Windows 图标资源对象文件（含绝对路径），应在 CMake 中改用 `target_sources(desktop_todo PRIVATE Desktop.rc)` 或重新编译为相对路径的 `.o`，避免使用硬编码绝对路径。

### 6.2 资源路径

- GIF 和图标仍通过相对路径 `../image/` 加载，保持与现有运行方式一致。
- 配置文件仍使用 `../config/eventlist.ini`。

## 7. 迁移步骤概要

1. **创建 C++ 桥接类**：`ApplicationController`、`TaskListModel`。
2. **扩展 `EventRepository`**：增加变更通知信号。
3. **创建 QML 模块与基础组件**：`main.qml`、`PetWindow.qml`、`Theme.qml`。
4. **实现 `TaskListDialog.qml` 和 `AddDialog.qml`**，并对接 `TaskModel`。
5. **改造 `main.cpp`**：使用 `QGuiApplication` + `QQmlApplicationEngine`。
6. **更新 `CMakeLists.txt`**：切换 Qt 模块、注册 QML 文件、删除 Widgets 依赖。
7. **删除旧 `ui/` 文件**。
8. **构建并手动验证**核心流程（见 `AGENTS.md` 提交前检查）。

## 8. 风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 无边框窗口拖拽 / 透明背景在 QML 下行为差异 | 中 | 使用 `Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool` + `color: "transparent"` 充分测试。 |
| GIF 动画支持 | 低 | `AnimatedImage` 原生支持 GIF，替换 `QMovie`。 |
| 任务列表关闭时清理已完成任务 | 中 | 在 `TaskListDialog` 关闭 / 隐藏时显式调用 `TaskModel.removeDoneEvents()` 和 `save()`。 |
| 系统托盘与 QML 窗口状态同步 | 低 | 通过 `ApplicationController` 信号桥接。 |
| `Desktop.rc` / `ioc.o` 硬编码绝对路径 | 中 | 改用 CMake `target_sources(desktop_todo PRIVATE Desktop.rc)` 或重新编译相对路径资源对象。 |

## 9. 验收标准

重构完成后，必须满足：
1. `cmake --build build` 成功，无新增警告/错误。
2. 启动后宠物窗口正常显示、可拖拽、置顶。
3. 右键宠物显示任务列表；勾选任务后隐藏 / 关闭任务列表能正确删除已完成任务。
4. 托盘 “Add” 打开添加对话框；能添加、删除、确认、取消。
5. 托盘 “Update” 触发版本检查并正确提示。
6. 重启程序后任务状态与上次一致。

## 10. 参考

- `AGENTS.md` 项目规范
- Qt 6 文档：`QQmlApplicationEngine`、`QAbstractListModel`、`qt_add_qml_module`、`AnimatedImage`
- 现有核心类：`core/event.h`、`core/event_repository.h`、`services/update_service.h`
