# 🩺 AI Doctor App

An offline, desktop-based AI medical assistant built with **C++17**, **Qt 6 (Widgets)**, and **llama.cpp** (running local GGUF models).

Provides general educational health information with zero internet requirement. All AI inference runs 100% locally on your machine for complete privacy.

> [!IMPORTANT]
> **Medical Disclaimer:** This application is for **educational and informational purposes only**. It is **not** a licensed medical diagnostic tool and must **not** be used for medical emergencies or clinical diagnosis. Always consult a qualified physician for healthcare advice.

---

## ✨ Features

- 🔒 **100% Offline & Private:** Runs local LLMs via `llama.cpp` (no API keys, no internet tracking).
- 💬 **ChatGPT-style Dark UI:** Clean, responsive chat interface styled with custom Qt stylesheets.
- ⚡ **Non-Blocking Architecture:** Model inference runs in a dedicated background worker thread—GUI never freezes.
- ⚙️ **Configurable Settings:** Customize `.gguf` model path, temperature, max tokens, and GPU layer offloading directly from the UI.
- 🛡️ **Built-in Safety Disclaimer:** Enforces an acknowledgment dialog on startup.

---

## 📋 Prerequisites

Before building the project, ensure you have the following installed:

| Requirement | Recommended Version | Notes |
|-------------|---------------------|-------|
| **OS** | Windows 10 / 11 (64-bit) | Supported & tested |
| **CMake** | 3.16 or higher | [Download CMake](https://cmake.org/download/) |
| **Compiler** | MSVC (Visual Studio 2019/2022) or MinGW 64-bit | C++17 support required |
| **Qt 6** | Qt 6.5+ (Widgets component) | [Download Qt](https://www.qt.io/download) |
| **AI Model** | Any `.gguf` quantized model | e.g. Phi-3 Mini, Llama-3-8B-Instruct (Q4_K_M) |

---

## 🚀 Getting Started (Step-by-Step)

### Step 1: Clone the Repository (with Submodules)

This project uses `llama.cpp` as a Git submodule. Clone the repository with `--recurse-submodules`:

```bash
git clone --recurse-submodules https://github.com/<your-username>/Ai-Doctor-App.git
cd Ai-Doctor-App
```

> **Note for existing clones:** If you cloned without `--recurse-submodules`, initialize `llama.cpp` by running:
> ```bash
> git submodule update --init --recursive
> ```

---

### Step 2: Download a GGUF Model

Because AI model weights are large (several gigabytes), they are not stored in Git.

1. Download any compatible GGUF model (e.g. from [Hugging Face](https://huggingface.co/models?search=gguf)).
   - **Recommended for general PCs:** [Phi-3-mini-4k-instruct-Q4_K_M.gguf](https://huggingface.co/microsoft/Phi-3-mini-4k-instruct-gguf) (~2.3 GB)
   - **Alternative:** [Meta-Llama-3-8B-Instruct-Q4_K_M.gguf](https://huggingface.co/QuantFactory/Meta-Llama-3-8B-Instruct-GGUF) (~4.9 GB)
2. Place the downloaded `.gguf` file inside the `models/` directory:
   ```
   models/model.gguf
   ```
   *(Or you can keep it anywhere on your drive and browse to it from the app's Settings menu).*

---

### Step 3: Build the Project

You can build the project using **Qt Creator**, **Visual Studio**, or the **Command Line**.

#### Option A: Using Qt Creator (Easiest)

1. Open **Qt Creator**.
2. Select **File → Open File or Project...** and choose `CMakeLists.txt` in the project root.
3. Select your installed **Qt 6 Desktop Kit** (MSVC or MinGW).
4. Select the **Release** build configuration.
5. Click the green **Run (▶)** button or press `Ctrl + R`.

---

#### Option B: Command Line (MSVC / PowerShell)

Replace `C:\Qt\6.8.0\msvc2019_64` with your actual Qt installation directory:

```powershell
# 1. Create and configure build directory
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2019_64"

# 2. Compile in Release mode
cmake --build build --config Release
```

The compiled binary and dependencies will be located in `build/Release/AIDoctor.exe`.

---

#### Option C: Command Line (MinGW)

```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\mingw_64"
cmake --build build
```

---

## 🏃 Running the Application

1. Launch `AIDoctor.exe` (or run `run.bat`).
2. Accept the **Medical Disclaimer** popup.
3. The application will automatically attempt to load `models/model.gguf`.
4. If your model file is located elsewhere:
   - Go to **File → Settings** (or click the Settings gear icon).
   - Click **Browse** next to **Model File Path** and select your `.gguf` file.
   - Adjust **Temperature**, **Max Tokens**, or **GPU Layers** if desired.
   - Click **Save**.
5. Type your question in the chat box and press **Send**!

---

## ⚙️ Configuration & Settings

| Setting | Default | Description |
|---------|---------|-------------|
| **Model Path** | `models/model.gguf` | Absolute or relative path to your downloaded `.gguf` file |
| **Temperature** | `0.7` | Controls randomness (lower = more factual/deterministic, higher = more creative) |
| **Max Tokens** | `512` | Maximum length of generated response |
| **GPU Layers** | `0` | Number of model layers to offload to your GPU (set to `0` for CPU only) |

---

## 📁 Repository Structure

```
Ai-Doctor-App/
├── CMakeLists.txt        # Top-level CMake configuration
├── .gitmodules           # Submodule config for llama.cpp
├── .gitignore            # Git ignore rules (build artifacts, models, caches)
├── run.bat               # Convenience launcher script for Windows
├── src/                  # C++ application source and header files
│   ├── main.cpp          # App entrypoint
│   ├── MainWindow.cpp    # Primary UI controller
│   ├── ChatWidget.cpp    # Chat bubble renderers
│   ├── LLMEngine.cpp     # llama.cpp inference integration
│   ├── Worker.cpp        # Background thread executor
│   └── Settings.cpp      # Settings storage & dialog
├── ui/                   # Qt Designer UI forms (.ui files)
├── resources/            # Stylesheets (.qss) and Qt resource collections (.qrc)
├── prompts/              # System prompt defining AI Doctor personality & boundaries
├── models/               # Place your local .gguf models here
└── third_party/
    └── llama.cpp         # llama.cpp engine (linked as Git submodule)
```

---

## ❓ Frequently Asked Questions (FAQ) & Troubleshooting

<details>
<summary><b>1. CMake cannot find Qt 6 package (<code>find_package(Qt6 REQUIRED)</code> failed)</b></summary>
<p>

Provide the path to your Qt installation directory via `CMAKE_PREFIX_PATH`. For example:
```bash
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2019_64"
```
Or set the `Qt6_DIR` environment variable to point to `.../lib/cmake/Qt6`.
</p>
</details>

<details>
<summary><b>2. The <code>third_party/llama.cpp</code> directory is empty</b></summary>
<p>

Run:
```bash
git submodule update --init --recursive
```
If you downloaded the repository as a `.zip` from GitHub instead of using `git clone`, Git submodules are not included in GitHub ZIP downloads. You must run:
```bash
git clone --depth 1 https://github.com/ggerganov/llama.cpp.git third_party/llama.cpp
```
</p>
</details>

<details>
<summary><b>3. Model fails to load or app crashes when sending a prompt</b></summary>
<p>

- Verify that your `.gguf` file exists and matches the path in **Settings**.
- Make sure you have sufficient RAM for your chosen model:
  - 3B parameter model (Q4): requires ~2–3 GB RAM.
  - 7B/8B parameter model (Q4): requires ~5–8 GB RAM.
- If you enabled GPU layers but do not have a CUDA/OpenCL-capable build, keep **GPU Layers** set to `0`.
</p>
</details>

<details>
<summary><b>4. Missing Qt DLLs when launching <code>AIDoctor.exe</code> directly</b></summary>
<p>

If you run the executable outside of Qt Creator, Windows may ask for `Qt6Widgets.dll` or `Qt6Core.dll`. The CMake configuration automatically runs `windeployqt` during post-build. If needed, manually run:
```powershell
windeployqt.exe build\Release\AIDoctor.exe
```
or launch via [`run.bat`](file:///d:/Ai-Doctor-App/run.bat).
</p>
</details>

---

## 📄 License

- **Application Source Code:** © 2026 Shujaullah-Dev.
- **llama.cpp:** Licensed under the MIT License (see `third_party/llama.cpp/LICENSE`).
