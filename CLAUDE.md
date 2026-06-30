# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

始終使用繁體中文進行回覆
始終使用馬斯克的 第一性原理 來回覆問題
當正在Debug時，我預設會把錯誤訊息貼在error.txt，先找根目錄的資料夾有沒有這個檔案，有就讀，如你判斷是無關報錯，再通知我


## Project Overview

VPEToolkit is a Qt 6 desktop GUI application written in C++17, targeting Windows (MSVC2022 64-bit). The project is in early development — currently a working Qt template with an empty main window.

## Build System

**CMake** with Qt 6 auto tools (AutoMOC, AutoUIC, AutoRCC). The configured kit is `Desktop Qt 6.11.0 MSVC2022 64bit`.

**Build (Debug):**
```bash
cmake -B build/Desktop_Qt_6_11_0_MSVC2022_64bit-Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Desktop_Qt_6_11_0_MSVC2022_64bit-Debug
```

**Run:**
```
build/Desktop_Qt_6_11_0_MSVC2022_64bit-Debug/VPEToolkit.exe
```

Qt Creator is the primary IDE — open `CMakeLists.txt` directly and use the configured kit.

## Architecture

```
main.cpp
  └─ QApplication (translation loaded from VPEToolkit_zh_TW.ts)
       └─ MainWindow : QMainWindow
            └─ Ui::MainWindow (generated from mainwindow.ui)
```

- **mainwindow.ui** — Qt Designer XML; defines an 800×600 window with menu bar and status bar. Edit visually in Qt Designer or directly in XML.
- **mainwindow.h / mainwindow.cpp** — Standard Qt widget pattern: constructor calls `setupUi(this)`, which wires the generated `ui_mainwindow.h`.
- **VPEToolkit_zh_TW.ts** — Traditional Chinese translation source. Run `lupdate` to extract new strings, `lrelease` to compile to `.qm`.

## Meeting Agenda Generator Feature

The main feature generates Word (.docx) and PDF meeting agendas for IA TMC (Incredible Asus Toastmasters Club).

**Data flow:**
1. Qt form → JSON (temp file) → `QProcess` → `scripts/generate_agenda.py` → output `.docx` + `.pdf`

**Key files:**
- `templates/agenda_template.docx` — original document used as the base (never modify by hand; all content is overwritten by the script)
- `scripts/generate_agenda.py` — uses `python-docx` to replace text in specific table cells/paragraphs while preserving all formatting. Supports 1–4 speeches dynamically.
- `speechwidget.h/.cpp` — `QWidget` subclass for one prepared speech slot; emits `removeRequested(int)` for dynamic add/remove.

**Document structure (table indices in `doc.tables`):**
- `[0]` — date/location row
- `[1]` — main 21-row agenda (row 2 contains a nested table for Timer/AhCounter/VoteCounter)
- `[2]` — speech objectives (3 rows × N speeches)
- `[3]` — officer directory (fixed; not modified)

**Python dependencies:** `python-docx`, `docx2pdf` (requires Microsoft Word for PDF conversion on Windows). Install via `scripts/requirements.txt`.

## Qt Conventions in This Project

- UI changes go in `mainwindow.ui` (designed) or `mainwindow.cpp` (programmatic).
- New signals/slots follow Qt's `connect()` syntax; prefer the typed `&Class::signal` form.
- New source files must be added to `CMakeLists.txt` under `SOURCES`/`HEADERS`.
- Translation strings use `tr("...")` — CMake's `qt_add_translations` target handles `.ts` → `.qm` compilation.
