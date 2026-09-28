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
## Building

### Qt Creator

1. Open `CMakeLists.txt` as a project.
2. Select a **Qt 6** kit (MSVC or MinGW).
3. Configure and build **Release**.
4. Run `AIDoctor`.

### Command line (MSVC)

```powershell
cd AI-Doctor
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2019_64"
cmake --build build --config Release
```

### Command line (MinGW)

```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\mingw_64"
cmake --build build
```

The executable is written to `build/` (or `build/Release/` with MSVC multi-config generators).

---

## Running

1. Start `AIDoctor.exe`.
2. Read and accept the **Medical Disclaimer**.
3. Wait for the model to load (status bar shows progress).
4. Type a health question and click **Send**.

If the model fails to load, open **File → Settings**, click **Browse**, and select your `.gguf` file.

---

## Settings

| Option | Description |
|--------|-------------|
| Model File Path | Path to a `.gguf` model |
| Temperature | Randomness (0.0 = focused, 1.0+ = creative) |
| Max Tokens | Maximum response length |
| GPU Layers | Layers offloaded to GPU (0 = CPU only) |

---

## Project Structure

```
AI-Doctor/
├── CMakeLists.txt
├── src/           # C++ application code
├── ui/            # Qt Designer layouts
├── prompts/       # System prompt
├── models/        # Place model.gguf here
├── resources/     # Dark theme stylesheet
└── third_party/
    └── llama.cpp  # Local inference engine
```

---

## Troubleshooting

### Model fails to load

- Confirm the `.gguf` path in Settings is correct.
- Use a model compatible with your RAM (3B Q4 ≈ 2–3 GB).
- Check the file is not corrupted (re-download if needed).

### Slow responses

- Use a smaller quantized model (Q4_K_M).
- Increase **GPU Layers** if you have an NVIDIA GPU and built with CUDA.
- Lower **Max Tokens**.

### Qt not found during CMake configure

Set `CMAKE_PREFIX_PATH` to your Qt installation, e.g.:

```
C:\Qt\6.8.0\msvc2019_64
```

### Application closes immediately

You must click **I Understand** on the disclaimer dialog to continue.

---

## Disclaimer

This software is for **educational purposes only**. It does not provide medical diagnosis or treatment. Always consult a qualified healthcare provider for medical advice. In an emergency, call your local emergency number immediately.

---

## License

Application source: Shujaullah-Dev.

llama.cpp: see `third_party/llama.cpp/LICENSE`.
