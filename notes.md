# 项目笔记

## 代码整体架构

项目按关注点拆分为四个目录：

* `core`：领域模型与数据持久化。
  * `Event`：任务领域模型（ID、文本、完成状态、优先级）。
  * `SettingsInterface` / `QSettingsAdapter`：配置存储抽象与 `.ini` 实现。
  * `ConfigStore`：负责 `QList<Event>` 与 `.ini` 之间的序列化/反序列化。
  * `EventRepository`：事件列表的内存状态与持久化入口。
* `services`：跨模块公共服务。
  * `UpdateService`：异步版本检查服务。
* `ui`：窗口、对话框与 `.ui` 文件。
  * `MainWindow`：宠物主窗口、托盘菜单、拖拽交互。
  * `TaskListDialog`（原 `Form`）：任务列表展示与完成删除。
  * `AddDialog`：添加/编辑任务窗口。
* `app`：装配入口。
  * `main.cpp`：创建 `QSettingsAdapter`、`ConfigStore`、`EventRepository`、`UpdateService`，并注入给 UI。

## 命名规范

统一采用 Google C++ Style：

* 文件名：`lower_with_underscore.cpp/h/ui`
* 类名：`UpperCamelCase`
* 成员变量：以尾部下划线结尾，例如 `event_repository_`
* 函数名：`UpperCamelCase`
* 枚举/常量：`kUpperCamelCase`

## 已修复的问题

1. 消除了 `Struct.h` 中的全局静态变量。
2. UI 不再直接操作 `QSettings`，所有数据变更通过 `EventRepository` 完成。
3. 修复了任务列表删除时的索引漂移 bug。
4. 添加/编辑对话框的删除操作只在点击“确定”后才持久化。
5. 版本检查改为异步，不再阻塞主线程。

## 构建

```bash
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

程序运行时会读取/写入 `../config/eventlist.ini`。
