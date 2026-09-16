# Validation record / 验证记录

## v1.1 — Continuous commands / 连续命令

Updated / 更新日期: 2026-09-16

- Implemented: one wake starts a continuous session; each successful command resets the 15-second inactivity timer. Decoder timeouts reset only the decoder, not the session. / 一次唤醒开启连续会话，每次成功命令重置 15 秒无命令计时；模型内部超时仅重置解码器，不退出会话。
- LCD stays in LISTENING after actions; the active home page says SAY COMMAND. / 执行动作后保持 LISTENING，会话中的主页提示 SAY COMMAND。
- A 500 ms post-command guard is retained; pause briefly between commands. / 保留命令后 500 ms 间隔，请在两条命令之间短暂停顿。
- Build passed with ESP-IDF 5.3.2, ESP-SR 2.5.3 and ESP-DL 3.3.10. Application: 2,033,808 bytes, about 3% partition space remaining. / 编译通过，应用为 2,033,808 字节，分区剩余约 3%。
- Application flashed to the connected board and flash hash verified. Existing models and partitions were unchanged. / 应用已烧录并校验，模型与分区未更改。
- Serial boot confirmed v1.1, LCD initialization, both models, five commands and microphone READY. / 串口确认 v1.1 启动、LCD 初始化、双模型、五条命令和麦克风就绪。
- **Pending:** user confirmation and serial evidence of multiple commands after a single wake, timer reset after commands, 15-second idle exit and re-wake. These are not yet marked hardware-verified. / **待验证：** 单次唤醒多命令、命令后计时重置、15 秒无命令退出及再次唤醒；尚未标为实机通过。

## Previous single-command baseline / 上一版单命令基线

The owner confirmed all five commands, LCD text, RGB blocks, page navigation, home return and the original six-second timeout. Serial logs recorded all five commands in one continuous boot without the earlier WakeNet-clean crash. These tests apply to the previous single-command behavior, not v1.1's continuous session.

用户确认旧版五条命令、文字、三色色块、切页、返回主页及原 6 秒超时正常；同一次连续运行的串口记录全部五条命令，未再出现 WakeNet clean 崩溃。这些验证针对旧版单命令模式，不代表 v1.1 连续会话已验证。

## Limits / 限制

Fixed vocabulary only; no free dictation, AFE noise reduction, touch navigation or computer control. Controlled accuracy, false-trigger, noise and endurance tests remain open. The application partition should be enlarged before substantial feature additions, with the relocated model partition reflashed.

仅固定词表；无自由听写、AFE 降噪、触摸导航或电脑控制。尚未完成受控准确率、误触发、噪声及长时间运行测试。增加较多功能前应扩大应用分区，并重新烧录移动后的模型分区。
