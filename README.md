# QuickEdit

QuickEdit is a small desktop text editor built with **Qt Widgets** and **CMake**.
Open a folder, browse its contents in a tree, and edit your files in tabs.

The project is open source and beginner-friendly: whether you are a senior
engineer, a student, or this is your very first fork, you are welcome to open an
issue or a pull request.

---

## Features

- Open a folder and browse it in a tree view (`File > Open Folder`).
- Click a file to open it in a tab; files are read and written as UTF-8 text.
- Clicking a file that is already open focuses its existing tab instead of
  opening a duplicate.
- Save the content of the current tab back to disk (`File > Save`).
- Create a new empty file in the opened folder (`File > New File`).
- Closable tabs.
- All user-visible strings go through `tr()`; a French translation file
  (`QuickEdit_fr_FR.ts`) is ready to be filled in.

## Requirements

| Tool         | Version / note                            |
| ------------ | ----------------------------------------- |
| CMake        | 3.16 or newer                             |
| C++ compiler | C++17 (MinGW, MSVC, Clang, GCC, ...)      |
| Qt           | Qt 6 recommended; Qt 5 is also supported  |
| Qt modules   | `Widgets`, `LinguistTools`                |

## Build and run

### With Qt Creator (recommended)

1. Install Qt with the [Qt Online Installer](https://www.qt.io/download-qt-installer)
   and select a desktop kit (for example **Desktop Qt 6.11.1 MinGW 64-bit**).
2. In Qt Creator, open `CMakeLists.txt` (`File > Open File or Project`).
3. Select the desktop kit and let Qt Creator configure the project.
4. Press **Run** (Ctrl+R).

### Command line - Windows (MinGW + Qt 6)

Adapt the paths to your own Qt and compiler versions.

```powershell
$env:PATH = "C:\Qt\6.11.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;$env:PATH"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64
cmake --build build
.\build\QuickEdit.exe
```

### Command line - Linux / macOS

Adapt `CMAKE_PREFIX_PATH` to your Qt installation, or omit it if Qt is already
on the system path.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
cmake --build build
./build/QuickEdit
```

## Project structure

| File                      | Role                                                                    |
| ------------------------- | ----------------------------------------------------------------------- |
| `CMakeLists.txt`          | Builds the single `QuickEdit` target (Qt 6, with a Qt 5 fallback).      |
| `main.cpp`                | Entry point; loads `:/i18n/QuickEdit_<locale>` at startup.              |
| `mainwindow.h/.cpp`       | All application logic (open folder, open/save file, new file).          |
| `mainwindow.ui`           | Qt Designer UI: a splitter with a tree view and a tab widget.           |
| `CustomFileSystemModel.h` | Header-only `QFileSystemModel` subclass that shows the opened folder name as the first column header. |
| `QuickEdit_fr_FR.ts`      | French translation source file.                                         |

## How it works

- `mainwindow.ui` defines the layout: `Hierarchy` (a `QTreeView`) on the left,
  `tabs` (a `QTabWidget`) on the right, separated by `splitter`.
- **Open Folder** asks for a directory with `QFileDialog`, builds a
  `CustomFileSystemModel` for it, and anchors the tree on that directory.
- Clicking a file reads it and creates a `QTextEdit` inside a new tab. The full
  file path is stored in the editor's `whatsThis()` property.
- **Save** takes the active tab, reads the path stored in `whatsThis()`, and
  writes the editor content back to that file.
- **New File** asks for a name and creates an empty file in the folder that is
  currently open in the tree.

## Contributing

Contributions are welcome, from first-time contributors to experienced
developers. Fork [neylorxt/QuickEdit](https://github.com/neylorxt/QuickEdit) and
follow these steps:

1. Create a branch for your change (for example
   `git checkout -b fix-tab-close`).
2. Make your change and build the project to make sure it compiles.
3. Run the application and test your change manually - there is no automated
   test suite, CI, or linter yet.
4. Commit with a clear message and open a pull request describing what you did.

Guidelines:

- Wrap all new user-visible strings in `tr()`.
- Comments are currently a mix of English and French; both are fine.
- If you add or change user-visible strings, refresh the translation file with
  `lupdate`:
  `C:\Qt\6.11.1\mingw_64\bin\lupdate.exe . -ts QuickEdit_fr_FR.ts`
  (on Linux/macOS: `<Qt install>/bin/lupdate . -ts QuickEdit_fr_FR.ts`).
- Keep pull requests focused on a single change when possible.

## Known issues and roadmap

- Closing a tab does nothing: `tabCloseRequested` is connected inside
  `MainWindow::~MainWindow()` instead of the constructor.
- `Open File`, `New Folder`, and `Save As` are present in the menu but not
  implemented yet.
- There is no prompt for unsaved changes when a tab is closed.
- Translations are not embedded in the executable yet; the interface stays in
  English even when a translation is available.
- No automated tests yet; a small test target for the file handling logic would
  be a good first contribution.

## License

No `LICENSE` file has been added yet. Until one is chosen, the default copyright
rules apply (all rights reserved), so please contact the project maintainer
before reusing or redistributing the code.
