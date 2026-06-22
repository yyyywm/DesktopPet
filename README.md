# 桌面宠物&&ToDoList 😀

> 灵感来源: 网上的很多todo-list软件不好用, 不够便捷.
功能会在使用中逐渐完善, 现在只有一些基本功能

## Now 😦

* 目前 ToDoList 的基本显示功能已经完成，能够作为日常使用的工具，还可以隐藏到托盘。
* 已完成一次结构重构：
  1. 项目拆分为 `core` / `services` / `ui` / `app` 四层，职责清晰。
  2. 消除了全局状态，统一通过 `EventRepository` 管理任务数据。
  3. 统一使用 Google C++ 命名规范。
  4. 修复了删除索引漂移、添加对话框计数不同步、配置延迟写入、版本检查同步阻塞等问题。

## 项目结构

```
DesktopPet
├── core/          # 领域模型、配置抽象、事件仓储
├── services/      # 公共服务（版本检查等）
├── ui/            # 窗口与 .ui 文件
├── main.cpp       # 依赖装配入口
├── config/        # .ini 配置文件
└── image/         # 图片与 GIF 资源
```

## 构建

```bash
cmake -S . -B build -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH="D:/Qt/6.2.4/mingw_64/lib/cmake"
cmake --build build
```

运行 `build/desktop_todo.exe` 即可。程序使用 `../config/eventlist.ini` 保存任务数据。

## The goal 😶‍🌫️

### ToDoList

* 任务列表的展示.
* 选择框删除已完成的任务
* 根据事情的重要性,改变任务的颜色.（DLC）
* 任务提醒
* 番茄时钟, 高效工作. 
  * 可以直接在宠物旁边直接加个时钟进行倒计时
* ......


### Desktop Pet

* 微信消息提醒
  * 可以弹出微信消息
* 鼠标点击触发TodoList显示
  * 用户自定义任务
* 宠物互动
  * 提醒喝水
  * 时间显示（或者整点报时）
  * 偶尔弹出消息
  * ......



## How to use it

添加事件：在托盘图标那里，右键点击add，弹出窗口。

目前添加和删除事件都在那个窗口里面。

右键桌面宠物可以显示任务列表，左键隐藏列表。



## Frame 💭

......



## THE NEXT VERSION：（有空再写）

> 1.  允许用户自定义图标和宠物样式
> 2. 整点报时
> 3. 番茄时钟
> 4. （消息提醒）