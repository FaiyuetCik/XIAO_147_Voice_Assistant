# Validation record / 验证记录

Updated / 更新日期: 2026-09-16

## Hardware-verified / 已通过实机验证

ESP-IDF 5.3.2, ESP-SR 2.5.3, ESP-DL 3.3.10. XIAO ESP32-S3 Plus 1.47-inch JD9853A board; 16 MB flash, 8 MB PSRAM, 240 MHz CPU.

- Full build and firmware/model flashing succeeded. / 编译及固件、模型烧录成功。
- Both models initialized with 512-sample audio frames; all five commands registered. / 两个模型均以 512 采样点帧初始化，五条命令已加载。
- Owner confirmed readable LCD text, red/green/blue blocks, Next page, Go home, and approximately six-second silent-wake timeout. / 用户确认文字、红绿蓝色块、切页、返回主页和约 6 秒静默超时均正常。
- Serial logs recorded all five actions in one continuous boot without the earlier crash. / 同一次连续运行的串口日志记录全部五条动作，未再出现此前崩溃。

| Uptime (ms) | Command | Destination |
| --- | --- | --- |
| 12848 | show red | Color |
| 59378 | show green | Color |
| 72208 | show blue | Color |
| 82728 | next page | Information |
| 92468 | go home | Home |

A later wake at 116668 ms timed out at 122698 ms (6.03 seconds). At 123008 ms, health logging reported seven wakes, five successful commands and 5,239,268 bytes of free heap including PSRAM. Functional checks do not establish recognition accuracy or long-duration reliability.

后续在 116668 ms 唤醒，122698 ms 超时返回（6.03 秒）。123008 ms 健康日志记录 7 次唤醒、5 次命令成功，含 PSRAM 的空闲堆为 5,239,268 字节。上述功能测试不代表准确率或长期稳定性测试。

## Runtime fix / 运行时修复

The initial build crashed after `show red` inside WakeNet `model_clean` → `dl_convq_queue_bzero` (LoadProhibited). The tested version avoids this callback, continuously feeds WakeNet, and accepts wake events only while idle. MultiNet is cleaned between command sessions.

初版在识别 `show red` 后进入 WakeNet clean 回调时崩溃。已验证版本避开该回调，持续向 WakeNet 输入音频，仅在空闲状态接受唤醒；命令会话间保留 MultiNet clean。

## Tested artifacts and limits / 已测试产物与限制

- Application: 2,033,712 bytes; SHA256 `17F93902B7609C9B7D6E65E16206B898CA8DA847BEC1746AA627F1DF7A86BA4B`.
- Model pack: 3,052,231 bytes; SHA256 `43CF83104AD227722C6E4BEF844E52D78F9C0A742A0965BF3FDFA7AE43C9C942`.
- These identify the tested local artifacts; fresh builds may differ. Binaries are not included in this repository. / 哈希标识已测试的本地产物，重新构建可能不同；仓库未包含二进制。
- The 2 MB application partition has about 3% free. / 2 MB 应用分区仅剩约 3%。
- No AFE noise reduction, free dictation, touch navigation or computer control. / 无 AFE 降噪、自由听写、触摸导航或电脑控制。
- Controlled accuracy, false-trigger, noisy-environment and endurance testing remain open. / 尚未完成受控准确率、误唤醒、噪声环境和长时间运行测试。
