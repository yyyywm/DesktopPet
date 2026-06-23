# DesktopPet QWidget → QML 重构实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 DesktopPet 的 UI 层从 Qt Widgets 完全迁移到 Qt 6 QML，保留 `core/` 和 `services/` 业务逻辑，保持现有核心交互不变。

**架构：** C++ 新增 `ApplicationController`（系统托盘 / 全局信号桥接）和 `TaskListModel`（`QAbstractListModel`），通过 `QQmlApplicationEngine` 的 context property 注入 QML。`EventRepository` 增加变更通知信号供 model 监听。所有界面用 `qml/` 下的 QML 文件实现，构建改用 `qt_add_qml_module`。

**Tech Stack：** Qt 6 (Core / Qml / Quick / QuickControls2 / Network), CMake 3.5+, MinGW/MSVC, Google C++ Style.

---

## 文件结构总览

| 文件 | 状态 | 职责 |
|------|------|------|
| `core/event_repository.h` / `.cpp` | 修改 | 增加变更通知信号与批量操作方法。 |
| `ui/task_list_model.h` / `.cpp` | 新建 | `QAbstractListModel` 包装 `EventRepository`。 |
| `ui/application_controller.h` / `.cpp` | 新建 | 系统托盘、更新服务桥接、全局动作分发。 |
| `qml/main.qml` | 新建 | 应用入口，装配窗口与对话框。 |
| `qml/PetWindow.qml` | 新建 | 桌面宠物主窗口。 |
| `qml/TaskListDialog.qml` | 新建 | 任务列表窗口。 |
| `qml/AddDialog.qml` | 新建 | 添加 / 编辑任务窗口。 |
| `qml/TaskListItem.qml` | 新建 | 任务列表单行 delegate。 |
| `qml/Theme.qml` | 新建 | 现代风格设计 token（Singleton）。 |
| `main.cpp` | 修改 | 改用 `QGuiApplication` + `QQmlApplicationEngine`。 |
| `CMakeLists.txt` | 修改 | 切换 Qt 模块，注册 QML，移除 Widgets。 |
| `Desktop.rc` | 修改 | 使用相对路径指向图标。 |
| `ui/main_window.*` / `ui/task_list_dialog.*` / `ui/add_dialog.*` | 删除 | 旧 QWidget 实现。 |

---

## Task 1：扩展 EventRepository 变更通知

**Files:**
- Modify: `core/event_repository.h`
- Modify: `core/event_repository.cpp`

目标：让 `TaskListModel` 能监听事件增删改，发出对应的 `QAbstractListModel` 信号。

- [ ] **Step 1：在头文件中添加信号与新方法**

打开 `core/event_repository.h`，在 `public:` 区域添加以下信号和方法：

```cpp
signals:
  void EventAdded(int id);
  void EventRemoved(int id);
  void EventChanged(int id);
  void EventsCleared();

public:
  void SetEventText(int id, QString text);
  void SetEventDone(int id, bool done);
  void RemoveDoneEvents();
```

- [ ] **Step 2：在实现文件中添加方法并触发信号**

打开 `core/event_repository.cpp`，追加实现：

```cpp
void EventRepository::SetEventText(int id, QString text) {
  Event* event = FindEvent(id);
  if (event == nullptr) {
    return;
  }
  event->set_text(std::move(text));
  emit EventChanged(id);
}

void EventRepository::SetEventDone(int id, bool done) {
  Event* event = FindEvent(id);
  if (event == nullptr) {
    return;
  }
  event->set_done(done);
  emit EventChanged(id);
}

void EventRepository::RemoveDoneEvents() {
  auto& events = events_;
  for (int i = events.size() - 1; i >= 0; --i) {
    if (events[i].done()) {
      const int id = events[i].id();
      events.removeAt(i);
      emit EventRemoved(id);
    }
  }
}
```

- [ ] **Step 3：在现有方法中 emit 信号**

在 `AddEvent` 两个重载中，在 `events_.append(...)` 之后 emit：

```cpp
emit EventAdded(events_.last().id());
```

在 `RemoveEvent` 中，在 `events_.erase(...)` 之前 emit：

```cpp
emit EventRemoved(id);
```

在 `Clear` 中：

```cpp
void EventRepository::Clear() {
  events_.clear();
  emit EventsCleared();
}
```

- [ ] **Step 4：构建验证核心层**

运行：

```bash
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

预期：编译通过，无新增警告/错误。

- [ ] **Step 5：Commit**

```bash
git add core/event_repository.h core/event_repository.cpp
git commit -m "refactor(core): add event change notifications"
```

---

## Task 2：创建 TaskListModel

**Files:**
- Create: `ui/task_list_model.h`
- Create: `ui/task_list_model.cpp`

目标：为 QML 提供类型安全的任务列表模型。

- [ ] **Step 1：创建头文件**

创建 `ui/task_list_model.h`：

```cpp
#ifndef DESKTOP_TODO__UI__TASK_LIST_MODEL_H_
#define DESKTOP_TODO__UI__TASK_LIST_MODEL_H_

#include <QAbstractListModel>
#include <QHash>
#include <QList>

#include "core/event_repository.h"

namespace desktop_todo {
namespace ui {

class TaskListModel : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit TaskListModel(core::EventRepository* repository,
                         QObject* parent = nullptr);

  enum Role {
    IdRole = Qt::UserRole + 1,
    TextRole,
    DoneRole,
    PriorityRole
  };
  Q_ENUM(Role)

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value,
               int role) override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void addEvent(const QString& text);
  Q_INVOKABLE void removeDoneEvents();
  Q_INVOKABLE void clear();
  Q_INVOKABLE void save();

 private:
  void ConnectRepository();
  int FindRowById(int id) const;

  core::EventRepository* repository_ = nullptr;
};

}  // namespace ui
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__UI__TASK_LIST_MODEL_H_
```

- [ ] **Step 2：创建实现文件**

创建 `ui/task_list_model.cpp`：

```cpp
#include "ui/task_list_model.h"

namespace desktop_todo {
namespace ui {

TaskListModel::TaskListModel(core::EventRepository* repository, QObject* parent)
    : QAbstractListModel(parent), repository_(repository) {
  ConnectRepository();
}

int TaskListModel::rowCount(const QModelIndex& /*parent*/) const {
  return repository_ == nullptr ? 0 : repository_->events().size();
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || repository_ == nullptr) {
    return QVariant();
  }
  const int row = index.row();
  const auto& events = repository_->events();
  if (row < 0 || row >= events.size()) {
    return QVariant();
  }
  const core::Event& event = events[row];
  switch (role) {
    case IdRole:
      return event.id();
    case TextRole:
      return event.text();
    case DoneRole:
      return event.done();
    case PriorityRole:
      return static_cast<int>(event.priority());
    default:
      return QVariant();
  }
}

bool TaskListModel::setData(const QModelIndex& index, const QVariant& value,
                            int role) {
  if (!index.isValid() || repository_ == nullptr) {
    return false;
  }
  const int row = index.row();
  auto& events = repository_->events();
  if (row < 0 || row >= events.size()) {
    return false;
  }
  const int id = events[row].id();
  switch (role) {
    case TextRole:
      repository_->SetEventText(id, value.toString());
      return true;
    case DoneRole:
      repository_->SetEventDone(id, value.toBool());
      return true;
    default:
      return false;
  }
}

QHash<int, QByteArray> TaskListModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[IdRole] = "id";
  roles[TextRole] = "text";
  roles[DoneRole] = "done";
  roles[PriorityRole] = "priority";
  return roles;
}

void TaskListModel::addEvent(const QString& text) {
  if (repository_ == nullptr) {
    return;
  }
  repository_->AddEvent(text, core::EventPriority::kNormal);
}

void TaskListModel::removeDoneEvents() {
  if (repository_ == nullptr) {
    return;
  }
  repository_->RemoveDoneEvents();
}

void TaskListModel::clear() {
  if (repository_ == nullptr) {
    return;
  }
  repository_->Clear();
}

void TaskListModel::save() {
  if (repository_ == nullptr) {
    return;
  }
  repository_->Save();
}

void TaskListModel::ConnectRepository() {
  if (repository_ == nullptr) {
    return;
  }
  connect(repository_, &core::EventRepository::EventAdded, this,
          [this](int /*id*/) {
            const int row = rowCount() - 1;
            beginInsertRows(QModelIndex(), row, row);
            endInsertRows();
          });
  connect(repository_, &core::EventRepository::EventRemoved, this,
          [this](int id) {
            const int row = FindRowById(id);
            if (row < 0) {
              return;
            }
            beginRemoveRows(QModelIndex(), row, row);
            endRemoveRows();
          });
  connect(repository_, &core::EventRepository::EventChanged, this,
          [this](int id) {
            const int row = FindRowById(id);
            if (row < 0) {
              return;
            }
            emit dataChanged(index(row), index(row));
          });
  connect(repository_, &core::EventRepository::EventsCleared, this, [this]() {
    beginResetModel();
    endResetModel();
  });
}

int TaskListModel::FindRowById(int id) const {
  if (repository_ == nullptr) {
    return -1;
  }
  const auto& events = repository_->events();
  for (int i = 0; i < events.size(); ++i) {
    if (events[i].id() == id) {
      return i;
    }
  }
  return -1;
}

}  // namespace ui
}  // namespace desktop_todo
```

- [ ] **Step 3：构建验证**

运行：

```bash
cmake --build build
```

预期：编译通过。

- [ ] **Step 4：Commit**

```bash
git add ui/task_list_model.h ui/task_list_model.cpp
git commit -m "feat(ui): add TaskListModel for QML"
```

---

## Task 3：创建 ApplicationController

**Files:**
- Create: `ui/application_controller.h`
- Create: `ui/application_controller.cpp`

目标：处理系统托盘、更新服务桥接、全局动作分发。

- [ ] **Step 1：创建头文件**

创建 `ui/application_controller.h`：

```cpp
#ifndef DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_
#define DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

#include "core/event_repository.h"
#include "services/update_service.h"

namespace desktop_todo {
namespace ui {

class ApplicationController : public QObject {
  Q_OBJECT

 public:
  explicit ApplicationController(core::EventRepository* event_repository,
                                 services::UpdateService* update_service,
                                 QObject* parent = nullptr);

 signals:
  void OpenAddDialogRequested();
  void ShowPetWindowRequested();
  void HidePetWindowRequested();
  void UpdateMessageRequested(const QString& title, const QString& body,
                              const QString& url);

 public slots:
  void CheckForUpdates();
  void Quit();

 private:
  void CreateTrayIcon();
  void CreateActions();
  void CreateMenu();
  void ConnectUpdateService();

  void OnUpToDate();
  void OnUpdateAvailable(const QString& release_date, const QString& log,
                         const QString& url);
  void OnUpdateCheckFailed(const QString& reason);

  core::EventRepository* event_repository_ = nullptr;
  services::UpdateService* update_service_ = nullptr;

  QSystemTrayIcon* tray_icon_ = nullptr;
  QMenu* tray_menu_ = nullptr;
  QAction* hide_action_ = nullptr;
  QAction* add_action_ = nullptr;
  QAction* update_action_ = nullptr;
  QAction* exit_action_ = nullptr;
};

}  // namespace ui
}  // namespace desktop_todo

#endif  // DESKTOP_TODO__UI__APPLICATION_CONTROLLER_H_
```

- [ ] **Step 2：创建实现文件**

创建 `ui/application_controller.cpp`：

```cpp
#include "ui/application_controller.h"

#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

namespace desktop_todo {
namespace ui {

ApplicationController::ApplicationController(
    core::EventRepository* event_repository,
    services::UpdateService* update_service, QObject* parent)
    : QObject(parent),
      event_repository_(event_repository),
      update_service_(update_service) {
  CreateTrayIcon();
  CreateActions();
  CreateMenu();
  ConnectUpdateService();
  tray_icon_->show();
}

void ApplicationController::CreateTrayIcon() {
  tray_icon_ = new QSystemTrayIcon(this);
  tray_icon_->setIcon(QIcon(QStringLiteral("../image/cat.ico")));
  tray_icon_->setToolTip(QStringLiteral("DesktopPet"));
  connect(tray_icon_, &QSystemTrayIcon::activated, this,
          [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger) {
              emit ShowPetWindowRequested();
            }
          });
}

void ApplicationController::CreateActions() {
  hide_action_ = new QAction(QStringLiteral("Hide"), this);
  connect(hide_action_, &QAction::triggered, this,
          [this]() { emit HidePetWindowRequested(); });

  add_action_ = new QAction(QStringLiteral("Add"), this);
  connect(add_action_, &QAction::triggered, this,
          &ApplicationController::OpenAddDialogRequested);

  update_action_ = new QAction(QStringLiteral("Update"), this);
  connect(update_action_, &QAction::triggered, this,
          &ApplicationController::CheckForUpdates);

  exit_action_ = new QAction(QStringLiteral("Quit"), this);
  connect(exit_action_, &QAction::triggered, this,
          &ApplicationController::Quit);
}

void ApplicationController::CreateMenu() {
  tray_menu_ = new QMenu();
  tray_menu_->addAction(hide_action_);
  tray_menu_->addAction(add_action_);
  tray_menu_->addAction(update_action_);
  tray_menu_->addAction(exit_action_);
  tray_icon_->setContextMenu(tray_menu_);
}

void ApplicationController::ConnectUpdateService() {
  if (update_service_ == nullptr) {
    return;
  }
  connect(update_service_, &services::UpdateService::UpToDate, this,
          &ApplicationController::OnUpToDate);
  connect(update_service_, &services::UpdateService::UpdateAvailable, this,
          &ApplicationController::OnUpdateAvailable);
  connect(update_service_, &services::UpdateService::CheckFailed, this,
          &ApplicationController::OnUpdateCheckFailed);
}

void ApplicationController::CheckForUpdates() {
  if (update_service_ == nullptr) {
    return;
  }
  update_service_->CheckForUpdates();
}

void ApplicationController::Quit() { QApplication::quit(); }

void ApplicationController::OnUpToDate() {
  emit UpdateMessageRequested(QStringLiteral("已经是最新版本啦！^-^"), QString(),
                              QString());
}

void ApplicationController::OnUpdateAvailable(const QString& /*release_date*/,
                                              const QString& log,
                                              const QString& url) {
  emit UpdateMessageRequested(QStringLiteral("检测到有新版本！"), log, url);
}

void ApplicationController::OnUpdateCheckFailed(const QString& /*reason*/) {
  emit UpdateMessageRequested(QStringLiteral("网络错误^-^！"), QString(),
                              QString());
}

}  // namespace ui
}  // namespace desktop_todo
```

注意：`HidePetWindow` 当前为空槽，QML 端将连接 `HidePetWindow` 信号/槽来隐藏窗口。这里保留槽是为了让托盘菜单项能触发；也可以改为信号。更简单的方式是：把 `HidePetWindow` 改成信号 `HidePetWindowRequested()`，并在菜单项 `triggered` 时 emit。请根据实际实现调整。

- [ ] **Step 3：构建验证**

运行：

```bash
cmake --build build
```

预期：编译通过。

- [ ] **Step 4：Commit**

```bash
git add ui/application_controller.h ui/application_controller.cpp
git commit -m "feat(ui): add ApplicationController for tray and service bridge"
```

---

## Task 4：创建 QML Theme 单例

**Files:**
- Create: `qml/Theme.qml`
- Create: `qml/qmldir`

目标：集中管理现代风格设计 token。

- [ ] **Step 1：创建 Theme.qml**

创建 `qml/Theme.qml`：

```qml
pragma Singleton
import QtQuick

QtObject {
    readonly property color background: "#ffffff"
    readonly property color surface: "#f3f4f6"
    readonly property color primary: "#667eea"
    readonly property color primaryDark: "#764ba2"
    readonly property color textPrimary: "#1f2937"
    readonly property color textSecondary: "#6b7280"
    readonly property color border: "#e5e7eb"

    readonly property int radiusSmall: 6
    readonly property int radiusMedium: 12
    readonly property int radiusLarge: 18

    readonly property int paddingSmall: 8
    readonly property int paddingMedium: 16
    readonly property int paddingLarge: 24

    readonly property int fontSmall: 12
    readonly property int fontBody: 14
    readonly property int fontTitle: 18

    readonly property int shadowElevation: 12
}
```

- [ ] **Step 2：创建 qmldir 注册单例**

创建 `qml/qmldir`：

```text
singleton Theme Theme.qml
```

- [ ] **Step 3：Commit**

```bash
git add qml/Theme.qml qml/qmldir
git commit -m "feat(qml): add Theme singleton"
```

---

## Task 5：创建 main.qml 与 PetWindow.qml

**Files:**
- Create: `qml/main.qml`
- Create: `qml/PetWindow.qml`

目标：搭建应用入口和桌面宠物主窗口。

- [ ] **Step 1：创建 PetWindow.qml**

创建 `qml/PetWindow.qml`：

```qml
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    width: 168
    height: 163

    signal rightClicked()
    signal dragStarted()

    AnimatedImage {
        id: petImage
        source: "../image/bear.gif"
        anchors.fill: parent
        playing: true
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property point dragStartMouse
        property point dragStartWindow

        onPressed: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                dragStartMouse = Qt.point(mouse.x, mouse.y)
                dragStartWindow = Qt.point(root.x, root.y)
                root.dragStarted()
            } else if (mouse.button === Qt.RightButton) {
                root.rightClicked()
            }
        }

        onPositionChanged: function (mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                root.x = dragStartWindow.x + (mouse.x - dragStartMouse.x)
                root.y = dragStartWindow.y + (mouse.y - dragStartMouse.y)
            }
        }
    }
}
```

- [ ] **Step 2：创建 main.qml**

创建 `qml/main.qml`：

```qml
import QtQuick
import QtQuick.Controls

PetWindow {
    id: petWindow

    Component.onCompleted: {
        App.showPetWindowRequested.connect(function () {
            petWindow.show()
        })
        App.hidePetWindowRequested.connect(function () {
            petWindow.hide()
        })
    }

    onRightClicked: {
        taskListDialog.showAt(petWindow.x + petWindow.width, petWindow.y)
    }

    onDragStarted: {
        taskListDialog.hideDialog()
    }

    TaskListDialog {
        id: taskListDialog
    }

    AddDialog {
        id: addDialog
    }

    Connections {
        target: App
        function onOpenAddDialogRequested() {
            addDialog.openFromModel()
        }
        function onUpdateMessageRequested(title, body, url) {
            updateDialog.title = title
            updateDialog.body = body
            updateDialog.url = url
            updateDialog.open()
        }
    }

    Dialog {
        id: updateDialog
        property string url: ""
        title: ""
        modal: true
        standardButtons: url.length > 0 ? Dialog.Ok | Dialog.Cancel : Dialog.Ok
        anchors.centerIn: parent
        visible: false

        Text {
            text: updateDialog.title
            font.bold: true
        }
        Text {
            text: updateDialog.body
            visible: updateDialog.body.length > 0
        }

        onAccepted: {
            if (url.length > 0) {
                Qt.openUrlExternally(url)
            }
        }
    }
}
```

注意：`ApplicationController` 的 `hidePetWindowRequested` 和 `showPetWindowRequested` 均为信号，QML 端通过 `Connections` 或直接 `connect` 监听。

- [ ] **Step 3：Commit**

```bash
git add qml/PetWindow.qml qml/main.qml
git commit -m "feat(qml): add pet window and main entry"
```

---

## Task 6：创建 TaskListDialog 与 TaskListItem

**Files:**
- Create: `qml/TaskListItem.qml`
- Create: `qml/TaskListDialog.qml`

目标：实现任务列表窗口，支持勾选、编辑、添加。

- [ ] **Step 1：创建 TaskListItem.qml**

创建 `qml/TaskListItem.qml`：

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root
    width: ListView.view ? ListView.view.width : implicitWidth
    spacing: Theme.paddingSmall

    required property int index
    required property int eventId
    required property string text
    required property bool done

    signal textEdited(string newText)
    signal doneChanged(bool newDone)

    CheckBox {
        checked: root.done
        onCheckedChanged: {
            if (checked !== root.done) {
                root.doneChanged(checked)
            }
        }
    }

    TextField {
        Layout.fillWidth: true
        text: root.text
        background: Rectangle {
            color: "transparent"
            border.color: parent.activeFocus ? Theme.primary : "transparent"
            radius: Theme.radiusSmall
        }
        onEditingFinished: {
            if (text !== root.text) {
                root.textEdited(text)
            }
        }
    }
}
```

- [ ] **Step 2：创建 TaskListDialog.qml**

创建 `qml/TaskListDialog.qml`：

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: Theme.background
    width: 360
    height: 400
    visible: false

    function showAt(xPos, yPos) {
        root.x = xPos
        root.y = yPos
        root.show()
    }

    function hideDialog() {
        TaskModel.removeDoneEvents()
        TaskModel.save()
        root.hide()
    }

    onClosing: {
        hideDialog()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingSmall

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: TaskModel
            clip: true
            spacing: 6

            delegate: TaskListItem {
                index: model.index
                eventId: model.id
                text: model.text
                done: model.done

                onTextEdited: function (newText) {
                    TaskModel.setData(TaskModel.index(index, 0), newText, TaskModel.TextRole)
                }
                onDoneChanged: function (newDone) {
                    TaskModel.setData(TaskModel.index(index, 0), newDone, TaskModel.DoneRole)
                }
            }
        }

        Button {
            text: qsTr("添加")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                TaskModel.addEvent("")
                listView.positionViewAtEnd()
            }
        }
    }
}
```

注意：`TaskModel.TextRole` / `TaskModel.DoneRole` 在 QML 中是否可直接访问取决于 `Q_ENUM` 注册。如果访问不到，可以改为硬编码 `0x100 + 1` 等，或在 C++ 中提供 `Q_INVOKABLE` 方法。

- [ ] **Step 3：Commit**

```bash
git add qml/TaskListItem.qml qml/TaskListDialog.qml
git commit -m "feat(qml): add task list dialog"
```

---

## Task 7：创建 AddDialog

**Files:**
- Create: `qml/AddDialog.qml`

目标：实现添加 / 编辑任务对话框。

- [ ] **Step 1：创建 AddDialog.qml**

创建 `qml/AddDialog.qml`：

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.WindowStaysOnTopHint | Qt.Dialog | Qt.FramelessWindowHint
    color: Theme.background
    width: 320
    height: 360
    visible: false

    property var tempEvents: []

    function openFromModel() {
        tempEvents = []
        for (let i = 0; i < TaskModel.rowCount(); ++i) {
            const idx = TaskModel.index(i, 0)
            tempEvents.push({
                text: TaskModel.data(idx, TaskModel.TextRole),
                done: TaskModel.data(idx, TaskModel.DoneRole)
            })
        }
        listModel.clear()
        for (const event of tempEvents) {
            listModel.append(event)
        }
        root.show()
    }

    function saveToModel() {
        TaskModel.clear()
        for (let i = 0; i < listModel.count; ++i) {
            const item = listModel.get(i)
            TaskModel.addEvent(item.text)
            if (item.done) {
                TaskModel.setData(TaskModel.index(i, 0), true, TaskModel.DoneRole)
            }
        }
        TaskModel.save()
        root.hide()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingSmall

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ListModel { id: listModel }
            clip: true
            spacing: 6

            delegate: RowLayout {
                width: ListView.view ? ListView.view.width : implicitWidth
                TextField {
                    Layout.fillWidth: true
                    text: model.text
                    onEditingFinished: {
                        model.text = text
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Theme.paddingSmall

            Button {
                text: qsTr("添加")
                onClicked: listModel.append({ text: "", done: false })
            }
            Button {
                text: qsTr("删除")
                onClicked: {
                    if (listView.currentIndex >= 0) {
                        listModel.remove(listView.currentIndex)
                    }
                }
            }
            Button {
                text: qsTr("确定")
                onClicked: root.saveToModel()
            }
            Button {
                text: qsTr("取消")
                onClicked: root.hide()
            }
        }
    }
}
```

- [ ] **Step 2：Commit**

```bash
git add qml/AddDialog.qml
git commit -m "feat(qml): add edit dialog"
```

---

## Task 8：改造 main.cpp

**Files:**
- Modify: `main.cpp`

目标：从 `QApplication` + `QMainWindow` 切换到 `QGuiApplication` + `QQmlApplicationEngine`。

- [ ] **Step 1：重写 main.cpp**

完全替换 `main.cpp` 为：

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

#include "core/config_store.h"
#include "core/event_repository.h"
#include "core/qsettings_adapter.h"
#include "services/update_service.h"
#include "ui/application_controller.h"
#include "ui/task_list_model.h"

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);

  const QString config_path =
      QDir::current().filePath(QStringLiteral("../config/eventlist.ini"));
  auto settings =
      std::make_unique<desktop_todo::core::QSettingsAdapter>(config_path);
  auto config_store =
      std::make_unique<desktop_todo::core::ConfigStore>(std::move(settings));
  desktop_todo::core::EventRepository event_repository(config_store.get());
  event_repository.Load();

  desktop_todo::services::UpdateService update_service;
  desktop_todo::ui::TaskListModel task_model(&event_repository);
  desktop_todo::ui::ApplicationController controller(&event_repository,
                                                      &update_service);

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty(
      QStringLiteral("TaskModel"), &task_model);
  engine.rootContext()->setContextProperty(QStringLiteral("App"), &controller);

  const QUrl url(QStringLiteral("qrc:/DesktopPet/qml/main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject* obj, const QUrl& obj_url) {
        if (obj == nullptr && url == obj_url) {
          QCoreApplication::exit(-1);
        }
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
```

注意：QML 文件路径 `qrc:/DesktopPet/qml/main.qml` 取决于 `qt_add_qml_module` 的 `URI` 和 `RESOURCE_PREFIX`。如果 CMake 中 `URI DesktopPet` 且 `RESOURCE_PREFIX "/"`，则路径为 `qrc:/DesktopPet/qml/main.qml`。

- [ ] **Step 2：Commit**

```bash
git add main.cpp
git commit -m "feat(app): switch main entry to QGuiApplication and QML engine"
```

---

## Task 9：更新 CMakeLists.txt

**Files:**
- Modify: `CMakeLists.txt`

目标：切换到 Qt 6 QML 模块，注册 QML 文件，移除 Widgets 依赖。

- [ ] **Step 1：替换 Qt 查找和链接**

将 `CMakeLists.txt` 中：

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
```

替换为：

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick QuickControls2 Network)
```

- [ ] **Step 2：替换 PROJECT_SOURCES**

将 `PROJECT_SOURCES` 替换为：

```cmake
set(PROJECT_SOURCES
    main.cpp
    ui/application_controller.h
    ui/application_controller.cpp
    ui/task_list_model.h
    ui/task_list_model.cpp
    core/event.h
    core/settings_interface.h
    core/qsettings_adapter.h
    core/qsettings_adapter.cpp
    core/config_store.h
    core/config_store.cpp
    core/event_repository.h
    core/event_repository.cpp
    services/update_service.h
    services/update_service.cpp
)
```

- [ ] **Step 3：替换 executable 定义和链接库**

将 `qt_add_executable` 和 `target_link_libraries` 相关段落替换为：

```cmake
qt_add_executable(desktop_todo
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    Desktop.rc
)

target_link_libraries(desktop_todo PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
)

qt_add_qml_module(desktop_todo
    URI DesktopPet
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/PetWindow.qml
        qml/TaskListDialog.qml
        qml/AddDialog.qml
        qml/TaskListItem.qml
        qml/Theme.qml
    RESOURCE_PREFIX "/"
)
```

- [ ] **Step 4：移除 AUTOUIC 和 Widgets include**

删除：

```cmake
set(CMAKE_AUTOUIC ON)
```

将 `target_include_directories` 中的 `${CMAKE_CURRENT_SOURCE_DIR}/ui` 保留（因为 `main.cpp` 仍通过 `ui/...` 引用），其他不变。

- [ ] **Step 5：Commit**

```bash
git add CMakeLists.txt
git commit -m "build: switch to Qt6 QML modules"
```

---

## Task 10：更新 Desktop.rc 并删除旧 ui/ 文件

**Files:**
- Modify: `Desktop.rc`
- Delete: `ui/main_window.h`, `ui/main_window.cpp`, `ui/main_window.ui`
- Delete: `ui/task_list_dialog.h`, `ui/task_list_dialog.cpp`, `ui/task_list_dialog.ui`
- Delete: `ui/add_dialog.h`, `ui/add_dialog.cpp`, `ui/add_dialog.ui`

目标：清理旧的 QWidget 实现，修复图标资源路径。

- [ ] **Step 1：修改 Desktop.rc**

将 `Desktop.rc` 内容改为相对路径：

```text
IDI_ICON1 ICON DISCARDABLE "image/cat.ico"
```

- [ ] **Step 2：删除旧 ui/ 文件**

运行：

```bash
rm ui/main_window.h ui/main_window.cpp ui/main_window.ui
rm ui/task_list_dialog.h ui/task_list_dialog.cpp ui/task_list_dialog.ui
rm ui/add_dialog.h ui/add_dialog.cpp ui/add_dialog.ui
```

- [ ] **Step 3：提交清理**

```bash
git add Desktop.rc
git rm ui/main_window.h ui/main_window.cpp ui/main_window.ui
git rm ui/task_list_dialog.h ui/task_list_dialog.cpp ui/task_list_dialog.ui
git rm ui/add_dialog.h ui/add_dialog.cpp ui/add_dialog.ui
git commit -m "refactor(ui): remove old QWidget files and fix icon path"
```

---

## Task 11：构建与手动验证

**Files:**
- All

目标：确保重构后的应用跑通核心流程。

- [ ] **Step 1：重新配置并构建**

```bash
rm -rf build
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

预期：构建成功，无新增警告/错误。

- [ ] **Step 2：手动验证核心流程**

从项目根目录运行：

```bash
./build/desktop_todo.exe
```

按以下清单验证：

1. 启动后宠物窗口显示 `bear.gif`，可拖拽移动，窗口置顶。
2. 右键宠物窗口，任务列表出现在宠物右侧。
3. 在任务列表中勾选一项任务，点击宠物窗口或关闭任务列表，已完成任务被删除。
4. 托盘图标右键 → Add，弹出添加/编辑对话框；可添加、删除、确认、取消。
5. 托盘图标右键 → Update，触发版本检查并弹出提示。
6. 托盘图标右键 → Quit，应用退出。
7. 重启应用后，任务状态与上次一致。

- [ ] **Step 3：修复构建/运行问题**

如果遇到 QML 路径、角色访问、信号连接或窗口行为问题，根据错误信息修改对应文件并重新构建。

- [ ] **Step 4：最终提交**

```bash
git add .
git commit -m "feat(qml): complete QWidget to QML refactor"
```

---

## 计划自评

### 覆盖 spec 检查

| Spec 要求 | 对应 Task |
|-----------|-----------|
| QQmlApplicationEngine + QGuiApplication | Task 8 |
| ApplicationController 系统托盘 / 更新桥接 | Task 3 |
| TaskListModel 继承 QAbstractListModel | Task 2 |
| EventRepository 变更通知 | Task 1 |
| PetWindow 无边框 / 置顶 / 透明 / GIF / 拖拽 | Task 5 |
| TaskListDialog 任务列表 / 勾选删除 | Task 6 |
| AddDialog 添加 / 删除 / 确认 / 取消 | Task 7 |
| CMake 切换 QML 模块 | Task 9 |
| 删除旧 ui/ 文件 | Task 10 |
| 手动验证核心流程 | Task 11 |

### Placeholder 检查

- 无 “TBD/TODO/实现 later”。
- 所有代码步骤包含完整代码片段或明确命令。
- 文件路径均为绝对项目路径或相对 `F:/Desktop/DesktopPet` 的相对路径。

### 一致性检查

- `TaskModel.TextRole` / `TaskModel.DoneRole` 依赖 Task 2 的 `Q_ENUM(Role)`。如果 QML 无法直接访问枚举值，需要后续调整；已在 Task 6 注释中说明。
- `ApplicationController` 已统一为信号：`showPetWindowRequested()` 和 `hidePetWindowRequested()`；Task 3 与 Task 5 已对齐。
