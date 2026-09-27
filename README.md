# Ai-Doctor-App

Offline desktop AI medical assistant built with **C++17**, **Qt 6 Widgets**, and **llama.cpp** (GGUF models).

Provides general educational health information. **Not** a licensed medical professional and **not** for diagnosis or emergency care.

---

## Features

- Medical disclaimer on startup
- ChatGPT-style dark UI
- Local GGUF model inference (no internet required)
- Background inference thread (GUI never freezes)
- Settings: model path, temperature, max tokens, GPU layers

---

## Requirements

| Component | Version |
|-----------|---------|
| Windows   | 10/11   |
| CMake     | 3.16+   |
| Qt        | 6.x (Widgets) |
| Compiler  | MSVC 2019+, MinGW, or Clang |
| llama.cpp | Included in `third_party/llama.cpp` |

## Installation

### 1. Install Qt 6

Install [Qt 6](https://www.qt.io/download) with the **MSVC** or **MinGW** kit and **Qt Widgets** module.

### 2. Clone this project (with llama.cpp)

```bash
git clone <your-repo-url> AI-Doctor
cd AI-Doctor
git submodule update --init --recursive
```

If `third_party/llama.cpp` is empty:

```bash
git clone --depth 1 https://github.com/ggerganov/llama.cpp.git third_party/llama.cpp
```

### 3. Download a GGUF model

See [models/README.md](models/README.md). Place the file at:

```
models/model.gguf
```

