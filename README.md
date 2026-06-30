# VPE Toolkit

A desktop application for generating formatted Word (.docx) and PDF meeting agendas, built for Toastmasters club VPEs (Vice Presidents of Education).

![Qt](https://img.shields.io/badge/Qt-6.11-41CD52?logo=qt&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus)
![Python](https://img.shields.io/badge/Python-3.13-3776AB?logo=python&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-10%2F11-0078D6?logo=windows)

## What It Does

VPE Toolkit takes structured meeting data (roles, speeches, officers) from a GUI form and generates a publication-ready Word/PDF agenda that preserves all formatting from a docx template.

**Data flow:**
```
Qt Form  →  JSON  →  Python (python-docx)  →  .docx + .pdf
```

## Features

- **4-tab interface** — Meeting Info & Roles, Officers, Speeches (1-4), Generate
- **Dynamic speech slots** — Add/remove prepared speeches; agenda table rows adjust automatically
- **Officer management** — Persistent storage with auto-sync to meeting roles
- **Draft save/restore** — Pick up where you left off
- **Toastmasters Pathways built-in** — 6 pathways, 14 level/project combinations with auto-abbreviation
- **One-click generation** — Produces both `.docx` and `.pdf` in your chosen output directory

## Prerequisites

- **Windows 10/11** (MSVC2022 64-bit)
- **Qt 6.11+** with Widgets and LinguistTools modules
- **Python 3.10+** with dependencies:
  ```
  pip install -r scripts/requirements.txt
  ```
- **Microsoft Word** (required by `docx2pdf` for PDF conversion)

## Build

```bash
# Debug
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<your-qt-path>
cmake --build build

# Release + Deploy (Windows batch)
build_release.bat
```

## Project Structure

```
├── main.cpp                  # App entry + global QSS stylesheet
├── mainwindow.h/.cpp         # Main window with 4 tabs (programmatic UI)
├── speechwidget.h/.cpp       # Reusable widget for one speech slot
├── mainwindow.ui             # Minimal Qt Designer file (UI built in code)
├── CMakeLists.txt            # Qt 6 CMake build config
├── build_release.bat         # One-click release build + windeployqt
├── scripts/
│   ├── generate_agenda.py    # Document generation engine (python-docx)
│   └── requirements.txt      # Python dependencies
├── templates/
│   └── agenda_template.docx  # Base template (formatting source)
└── docs/                     # Reference documents
```

## Usage

1. Launch `VPEToolkit.exe`
2. Fill in meeting info, assign roles, add speeches
3. Go to the **Generate** tab, pick an output folder
4. Click **Generate Word + PDF**

## License

MIT
