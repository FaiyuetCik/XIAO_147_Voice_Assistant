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

## ESP-SR and dependencies

ESP-SR 2.5.3, ESP-DL 3.3.10 and their managed dependencies retain their own
licenses in `managed_components`. Speech models and prebuilt libraries must
be distributed subject to the respective Espressif license files.
