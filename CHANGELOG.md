# Changelog / 更新日志

## v1.1 — 2026-09-16

- Continuous command recognition after a single Hi ESP wake.
- Return to wake-word mode after 15 seconds without a successful command; every successful command restarts the timer.
- Keep LISTENING visible after actions; show SAY COMMAND on the active home page.
- Retain a 500 ms inter-command guard and the WakeNet clean-crash workaround.
- Updated English/Chinese usage and explicit validation status in STATUS.md.

- 一次 Hi ESP 唤醒后可连续识别命令。
- 连续 15 秒没有成功命令才退出，每条成功命令重置计时。
- 动作后保持 LISTENING，会话中的主页显示 SAY COMMAND。
- 保留 500 ms 命令间隔和 WakeNet clean 崩溃规避措施。
- 更新中英文使用说明，并在 STATUS.md 区分已验证与待验证项目。

## Initial version / 初始版本

Five offline English commands with LCD actions, one command per wake and a six-second command window; hardware-confirmed before v1.1 development.

五条离线英文命令及 LCD 动作，每次唤醒一条命令、6 秒等待窗口；在开发 v1.1 前已通过实机确认。
