# XIAO 1.47 Voice Assistant v1.1

Offline English wake-word and command recognition with an LCD interface, built with ESP-IDF and ESP-SR for the XIAO ESP32-S3 Plus 1.47-inch JD9853A display board.

基于 ESP-IDF 与 ESP-SR，为 XIAO ESP32-S3 Plus 1.47 英寸 JD9853A 屏幕板开发的离线英文唤醒与命令识别应用。

[English](#english) | [中文](#中文)
## Demo

Click the preview to open a dedicated page with a play button:

[![Open the playable demo](https://faiyuetcik.github.io/XIAO_147_Voice_Assistant/poster.jpg)](https://faiyuetcik.github.io/XIAO_147_Voice_Assistant/)

The video is also stored at [`demo/show.mp4`](demo/show.mp4). GitHub sanitizes `<video>` tags in repository README files, so the Pages preview provides the inline player while keeping the MP4 in this repository.

If you want to try replicating on the same hardware as mine, you can check out this link:
https://www.seeedstudio.com/1-47-Inch-Touch-Display-Powered-by-XIAO-ESP32-S3-Plus-p-6996.html
## English

### Features and hardware

- Wake with **Hi E S P**, then speak one of five English commands.
- On-device WakeNet 9 (`wn9_hiesp`) and MultiNet 7 English (`mn7_en`); no network, cloud account, API key, or computer required during operation.
- Home, color and information pages on a 172 × 320 LCD; the last recognized command stays visible.
- Continuous commands after one wake. Each successful recognition restarts a 15-second inactivity timer; the screen stays in `LISTENING` until timeout.
- Tested hardware: XIAO ESP32-S3 Plus 1.47-inch JD9853A display board, 16 MB flash, 8 MB Octal PSRAM, and PDM microphone. Other 1.47-inch ESP boards are not automatically compatible: verify controller, pins and memory first.

This is a standalone firmware application, not a general speech-to-text library. It does not provide free dictation, AI chat, keyboard/mouse control, touch navigation, or AFE noise reduction.

### Usage

1. Power the board and wait for `SAY HI ESP`.
2. Say “Hi E S P”: pronounce E, S and P as English letter names.
3. Wait for `LISTENING`, then say a command from the table below.
4. Continue with another command without waking again, for example `Show red`, then `Show green`. Leave a short pause (at least about 0.5 seconds) between commands. After 15 seconds without a recognized command, it returns to `SAY HI ESP`; wake it again to start a new session.

| Command | Action |
| --- | --- |
| `Show red` | Open the color page with a red block |
| `Show green` | Open the color page with a green block |
| `Show blue` | Open the color page with a blue block |
| `Next page` | Cycle Home → Color → Information → Home |
| `Go home` | Return to the home command list |

### Pin mapping

| Signal | GPIO |
| --- | --- |
| PDM microphone clock / data | 1 / 2 |
| LCD CS / DC | 3 / 4 |
| LCD SCK / MOSI | 7 / 9 |
| LCD reset / backlight | 13 / 12 |
| SD card CS (held high; SD unused) | 43 |

Audio: 16 kHz, 16-bit mono. LCD: 10 MHz SPI, BGR, column offset 34, MADCTL `0x48`, inversion off. These settings are board-specific.

### Build and flash

Validated versions: **ESP-IDF 5.3.2**, **ESP-SR 2.5.3**, **ESP-DL 3.3.10**. Keep the component manifest and `dependencies.lock` together. The root CMake file includes an IDF 5.3 compatibility definition for `MALLOC_CAP_SIMD`; other toolchain versions have not been verified.

Install ESP-IDF 5.3.2 with ESP32-S3 tools, then open its configured terminal:

```sh
git clone https://github.com/FaiyuetCik/XIAO_147_Voice_Assistant.git
cd XIAO_147_Voice_Assistant
idf.py build
idf.py -p COM33 flash monitor
```

Replace `COM33` with your board's serial port (for example `/dev/ttyACM0` on Linux). Exit the monitor with Ctrl+]. Initial builds need internet access to download managed components; inference is offline. The checked-in `sdkconfig` preserves the tested configuration; `sdkconfig.defaults` records the principal options.

Use a **full `flash`**, not only `app-flash`, on first installation or after changing models/partitions. The model partition must be programmed along with the application. Flashing replaces firmware and may overwrite existing board data; back up anything needed first.

The custom partition table reserves 2 MB for the application and 4 MB for models. The tested application is 2,033,808 bytes (only about 3% application space remains), and its model pack is 3,052,231 bytes. Before substantial feature additions, enlarge the application partition and reflash the relocated model partition.

### Source layout and customization

```text
main/main.c                 Wake/command state machine and vocabulary
main/idf_component.yml     Pinned speech dependencies
components/audio_input/   PDM microphone capture
components/ui/            JD9853A driver and LCD pages
components/xiao_147_board/ Board initialization helper (not used by current app)
sdkconfig                 Tested build configuration
sdkconfig.defaults        Principal configuration defaults
partitions.csv            Application/model flash layout
```

To change commands, update the vocabulary in `main/main.c`, command IDs in `components/ui/include/ui.h`, and actions/home-page labels in `components/ui/ui.c` together. The current result range check and command-to-color mapping assume the existing IDs; update them when adding commands. Rebuild and flash after edits.

### Validation and troubleshooting

The previous single-command version was hardware-verified for all five commands, LCD text/colors, page navigation, return home and its six-second timeout. v1.1 was additionally verified with all five commands after one wake, and a 15.03-second exit after the final command; the owner confirmed the display behavior and timeout. See STATUS.md for details. Serial logs also recorded all five actions in one continuous boot after the runtime fix. These are functional checks, not a measured accuracy benchmark; noise, distance and pronunciation can affect recognition.

- No response: wait for startup to finish, say the wake phrase first, and do not speak the command until `LISTENING` appears.
- `Missing model` / `Model error`: check model selections and perform a full flash.
- No color change: inspect serial `COMMAND` and `ACTION` logs to distinguish recognition from display issues; verify the exact board and pin map.
- The initial build crashed in the WakeNet clean callback after recognizing a command. This version avoids that callback, keeps WakeNet continuously fed and accepts wake events only while idle. Do not reintroduce that reset without validating it against the pinned model/runtime.

See [STATUS.md](STATUS.md) for the test record and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for third-party attribution. Downloaded dependencies and model binaries are not vendored; their upstream licenses still apply. The project source code in this repository is licensed under the MIT License; see [LICENSE](LICENSE). ESP-SR, ESP-DL, and the speech models remain subject to their respective upstream licenses.

## 中文

### 功能与硬件

- 说 **Hi E S P** 唤醒，再说五条英文命令之一。
- 使用板端 WakeNet 9（`wn9_hiesp`）和 MultiNet 7 英文模型（`mn7_en`）；运行时不需要网络、云账号、API key 或电脑。
- 172 × 320 LCD 提供主页、颜色页和信息页，保留最后一次成功识别的命令。
- 一次唤醒后连续识别命令；每次识别成功重置 15 秒无命令计时，超时前屏幕保持 `LISTENING`。
- 已测试：XIAO ESP32-S3 Plus 1.47 英寸 JD9853A 屏幕板，16 MB Flash、8 MB Octal PSRAM 和 PDM 麦克风。其他 1.47 英寸 ESP 屏幕板不保证兼容，请先核对屏幕控制器、引脚和内存。

这是一个独立固件应用，不是通用语音转文字库。目前不支持自由听写、AI 对话、键鼠控制、触摸导航或 AFE 降噪，也不与 Pocket_AI_Terminal 集成。

### 使用方法

1. 上电，等待屏幕显示 `SAY HI ESP`。
2. 说 “Hi E S P”：Hi 后面的 E、S、P 分别按英文字母名称读。
3. 等待 `LISTENING`，再说下表中的命令。
4. 可以直接继续说下一条命令，例如先 `Show red`，再 `Show green`，无需重复唤醒。命令之间短暂停顿（至少约 0.5 秒）；连续 15 秒未识别到命令才返回 `SAY HI ESP`，之后需重新唤醒。

| 英文命令 | 实际功能 |
| --- | --- |
| `Show red` | 进入颜色页，显示红色色块 |
| `Show green` | 进入颜色页，显示绿色色块 |
| `Show blue` | 进入颜色页，显示蓝色色块 |
| `Next page` | 主页 → 颜色页 → 信息页 → 主页循环 |
| `Go home` | 返回命令列表主页 |

### 硬件引脚

| 信号 | GPIO |
| --- | --- |
| PDM 麦克风时钟 / 数据 | 1 / 2 |
| LCD CS / DC | 3 / 4 |
| LCD SCK / MOSI | 7 / 9 |
| LCD 复位 / 背光 | 13 / 12 |
| SD 卡 CS（保持高电平，未使用 SD） | 43 |

音频为 16 kHz、16 bit、单声道。LCD 使用 10 MHz SPI、BGR、列偏移 34、MADCTL `0x48`、关闭反色。这些设置针对已测试的屏幕板。

### 编译与烧录

已验证版本：**ESP-IDF 5.3.2、ESP-SR 2.5.3、ESP-DL 3.3.10**。保留组件清单和 `dependencies.lock`。根目录 CMake 包含 IDF 5.3 所需的 `MALLOC_CAP_SIMD` 兼容定义；尚未验证其他工具链版本。

安装 ESP-IDF 5.3.2 及 ESP32-S3 工具，在配置好环境的终端运行：

```sh
git clone https://github.com/FaiyuetCik/XIAO_147_Voice_Assistant.git
cd XIAO_147_Voice_Assistant
idf.py build
idf.py -p COM33 flash monitor
```

将 `COM33` 替换为实际串口；Linux 可能是 `/dev/ttyACM0`。按 Ctrl+] 退出串口监视器。首次构建需要联网下载依赖，但语音识别运行时完全离线。仓库保留已测试的 `sdkconfig`，主要选项另存于 `sdkconfig.defaults`；修改 defaults 不会覆盖已有 sdkconfig，请用 `idf.py menuconfig` 核对实际配置。

首次安装、更换模型或分区后，必须完整执行 **`flash`**，不要只执行 `app-flash`，否则模型分区可能缺失或不匹配。烧录会替换固件，并可能覆盖板上已有数据，请提前备份。

当前分区为应用 2 MB、模型 4 MB。已测试应用大小为 2,033,808 字节，应用分区仅剩约 3%；模型包为 3,052,231 字节。增加较多功能前应扩大应用分区，并重新烧录移动后的模型分区。

### 代码结构与修改命令

- `main/main.c`：唤醒、命令识别状态机和词表。
- `main/idf_component.yml`、`dependencies.lock`：依赖及固定版本。
- `components/audio_input/`：PDM 麦克风采集。
- `components/ui/`：JD9853A 驱动与三个 LCD 页面。
- `components/xiao_147_board/`：板级初始化辅助组件，当前主程序未调用。
- `sdkconfig`、`sdkconfig.defaults`、`partitions.csv`：构建配置与 Flash 分区。

修改命令时，需要同步更新 `main/main.c` 词表、`components/ui/include/ui.h` 命令 ID，以及 `components/ui/ui.c` 动作和主页文字。当前识别结果范围检查、颜色映射依赖现有 ID，新增命令时也要调整，然后重新编译烧录。

### 实测与排查

上一版单命令模式已通过实机确认：五条命令、文字、色块、切页、返回主页和 6 秒超时均正常。v1.1 另已实测一次唤醒连续执行五条命令，并在最后命令后 15.03 秒退出；用户确认屏幕与超时正常，详细记录见 STATUS.md。修复后的串口日志也在同一次连续运行中记录了全部五条命令执行。这是功能验证，不是准确率测试；噪声、距离和发音仍会影响识别。

- 没有响应：等待启动完成，先唤醒，看到 `LISTENING` 后再说命令。
- 出现 `Missing model` / `Model error`：检查模型配置，完整烧录。
- 没有色块：查看串口 `COMMAND` 和 `ACTION` 日志，区分识别与显示问题，并核对板型和引脚。
- 初版曾在命令识别后调用 WakeNet clean 时崩溃；当前版本避开该回调，持续输入 WakeNet，仅在空闲状态接受唤醒事件。未验证前不要恢复该复位调用。

测试记录见 [STATUS.md](STATUS.md)，第三方声明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。仓库不包含下载依赖和模型二进制，其上游许可证仍适用；第三方声明不等于本项目整体的开源许可授权。
