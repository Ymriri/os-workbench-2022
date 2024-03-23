# L0

为止原因，在Mac上使用UTM开虚拟机，然后使用XQartz远程显示正常，但是按键无法输入，尝试过加参数"ssh -XY" 以及换其他服务器依旧是这样，最后发现只有shift、control、option、command、LOCK可以正常输入，所以使用这五个按键完成实验0。
* Command：ESC，退出游戏
* Lock:up
* Shift:down
* Control:left
* Option:right

在屏幕渲染出一个可以移动的红色🟥，用上面几个按键控制（我上不了南大是有原因的，作业都只能完成他们的demo级别...）

## 运行
进入`amgame` 目录运行 `make run`

## 作业完成情况
- [x] 实现print输出
- [x] 控制屏幕渲染
- [x] 用户按键事件捕获