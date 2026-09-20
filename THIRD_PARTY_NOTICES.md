# Third-party notices

`components/ui/ui.c` uses the JD9853A register settings, offsets and color order
from Seeed_GFX2's `Driver_JD9853A.cpp` and `Driver_ST7789.cpp`. The SPI transport,
small font and application UI are implemented for ESP-IDF in this project.

## Seeed_GFX2

MIT License

Copyright (c) 2026 Seeed Studio

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Espressif components

This project uses the following Espressif components through the ESP-IDF
Component Registry:

- [ESP-SR 2.5.3](https://components.espressif.com/components/espressif/esp-sr) -
  Espressif MIT License.
- [ESP-DL 3.3.10](https://components.espressif.com/components/espressif/esp-dl) -
  MIT License.
- [ESP-DSP 1.8.0](https://components.espressif.com/components/espressif/esp-dsp) -
  Apache License 2.0.
- [cJSON 1.7.19~2](https://components.espressif.com/components/espressif/cjson) -
  MIT License.
- [dl_fft 0.7.0](https://components.espressif.com/components/espressif/dl_fft) -
  MIT License.
- [esp_new_jpeg 1.0.2](https://components.espressif.com/components/espressif/esp_new_jpeg) -
  Espressif custom license.

The component versions are pinned in dependencies.lock. The downloaded
component sources and their original license files are under
managed_components in a local build checkout; that directory is ignored by
Git and is not part of this repository. The original license terms and
copyright notices remain applicable when redistributing source code, firmware,
speech models, or prebuilt libraries.

The project source code in this repository is licensed separately under the
MIT License in the root LICENSE file. These third-party components, speech
models, and prebuilt libraries are not relicensed under the project's MIT
License.
