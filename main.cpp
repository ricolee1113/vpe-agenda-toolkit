#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStyleFactory>
#include <QFont>

static const char *APP_STYLE = R"(
/* ── Global ─────────────────────────────────────── */
* {
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 10pt;
}
QMainWindow, QDialog {
    background: #EDF0F3;
}

/* ── Tab Widget ──────────────────────────────────── */
QTabWidget::pane {
    border: 1px solid #BDC8D4;
    border-top: 2px solid #2471A3;
    background: #F4F6F8;
    border-radius: 0 4px 4px 4px;
}
QTabBar::tab {
    padding: 7px 22px;
    margin-right: 3px;
    border: 1px solid #BDC8D4;
    border-bottom: none;
    border-radius: 5px 5px 0 0;
    background: #E2E8EE;
    color: #5D6D7E;
}
QTabBar::tab:selected {
    background: #FFFFFF;
    color: #1A3A5C;
    font-weight: 600;
    border-bottom: 2px solid #FFFFFF;
}
QTabBar::tab:hover:!selected {
    background: #D4E6F1;
    color: #1A3A5C;
}

/* ── Group Boxes ─────────────────────────────────── */
QGroupBox {
    border: 1px solid #C5D2DF;
    border-radius: 7px;
    margin-top: 18px;
    padding: 14px 10px 10px 10px;
    background: #FFFFFF;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 14px;
    padding: 1px 8px;
    color: #1A3A5C;
    font-weight: 600;
}

/* ── Inputs ──────────────────────────────────────── */
QLineEdit, QDateEdit {
    border: 1px solid #C5D2DF;
    border-radius: 4px;
    padding: 5px 9px;
    background: #FFFFFF;
    min-height: 26px;
    color: #000000;
    selection-background-color: #2471A3;
    selection-color: #FFFFFF;
}
QLineEdit:focus, QDateEdit:focus {
    border: 1.5px solid #2471A3;
    background: #EBF5FB;
}
QLineEdit:read-only {
    background: #EBF0F5;
    color: #000000;
    border-color: #D0DAE5;
}


/* ── ComboBox ────────────────────────────────────── */
QComboBox {
    border: 1px solid #C5D2DF;
    border-radius: 4px;
    padding: 5px 9px;
    background: #FFFFFF;
    min-height: 26px;
    color: #000000;
    selection-background-color: #2471A3;
}
QComboBox:focus {
    border: 1.5px solid #2471A3;
    background: #EBF5FB;
}
QComboBox::drop-down {
    width: 22px;
    border: none;
    border-left: 1px solid #C5D2DF;
    background: #EDF2F7;
    border-radius: 0 4px 4px 0;
}
QComboBox QAbstractItemView {
    border: 1px solid #BDC8D4;
    border-radius: 4px;
    background: #FFFFFF;
    selection-background-color: #2471A3;
    selection-color: #FFFFFF;
    padding: 3px;
    outline: none;
}
QComboBox QAbstractItemView::item {
    min-height: 24px;
    padding: 2px 8px;
}

/* ── Buttons (default) ───────────────────────────── */
QPushButton {
    border: 1px solid #B0BDC8;
    border-radius: 5px;
    padding: 5px 14px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #F8FAFB, stop:1 #EDF0F3);
    color: #2C3E50;
    min-height: 26px;
}
QPushButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #EBF5FB, stop:1 #D6EAF8);
    border-color: #2471A3;
    color: #1A3A5C;
}
QPushButton:pressed {
    background: #C8DFF0;
    border-color: #1A5E8A;
}
QPushButton:disabled {
    background: #F0F3F5;
    color: #A0AEBB;
    border-color: #D5DCE4;
}

/* ── Generate button ─────────────────────────────── */
QPushButton#generateBtn {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2E86C1, stop:1 #1B6FA0);
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    font-size: 11pt;
    font-weight: 600;
    min-height: 42px;
}
QPushButton#generateBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #3498DB, stop:1 #2471A3);
}
QPushButton#generateBtn:pressed {
    background: #1A6FA0;
}
QPushButton#generateBtn:disabled {
    background: #7FB3D3;
    color: #DDEEF7;
}

/* ── Remove Speech button ────────────────────────── */
QPushButton#removeBtn {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #FDECEA, stop:1 #F5B7B1);
    border: 1px solid #E59D97;
    color: #922B21;
    padding: 3px 10px;
    min-height: 22px;
    font-size: 9pt;
}
QPushButton#removeBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #F5B7B1, stop:1 #EC9C97);
    border-color: #CB4335;
}
QPushButton#removeBtn:pressed {
    background: #E8938C;
}

/* ── Save Officers button ────────────────────────── */
QPushButton#saveOfficersBtn {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #D5F5E3, stop:1 #A9DFBF);
    border: 1px solid #7DCEA0;
    color: #1E8449;
    font-weight: 600;
    padding: 5px 18px;
}
QPushButton#saveOfficersBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #ABEBC6, stop:1 #82D4A8);
    border-color: #1E8449;
}

/* ── Add buttons (+) ─────────────────────────────── */
QPushButton#addBtn {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #EBF5FB, stop:1 #D4E6F1);
    border: 1px solid #85C1E9;
    color: #1A5276;
    font-weight: 600;
    padding: 5px 14px;
}
QPushButton#addBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #D4E6F1, stop:1 #A9CCE3);
    border-color: #2471A3;
}

/* ── Delete Announcement button ──────────────────── */
QPushButton#delAnnouncementBtn {
    background: transparent;
    border: 1px solid #E59D97;
    border-radius: 4px;
    color: #922B21;
    font-weight: bold;
    min-height: 24px;
    min-width: 28px;
    padding: 0 4px;
    font-size: 11pt;
}
QPushButton#delAnnouncementBtn:hover {
    background: #FADBD8;
    border-color: #CB4335;
}

/* ── TextEdit (log) ──────────────────────────────── */
QTextEdit {
    border: 1px solid #C5D2DF;
    border-radius: 5px;
    padding: 6px;
    background: #FAFCFD;
    font-family: "Consolas", "Courier New", monospace;
    font-size: 9pt;
    color: #000000;
    line-height: 1.4;
}

/* ── ScrollArea ──────────────────────────────────── */
QScrollArea {
    border: none;
    background: transparent;
}
QScrollArea > QWidget > QWidget {
    background: transparent;
}

/* ── ScrollBar ───────────────────────────────────── */
QScrollBar:vertical {
    background: #EDF0F3;
    width: 8px;
    border-radius: 4px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #A9B8C5;
    border-radius: 4px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: #7F8C9A; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background: #EDF0F3;
    height: 8px;
    border-radius: 4px;
    margin: 0;
}
QScrollBar::handle:horizontal {
    background: #A9B8C5;
    border-radius: 4px;
    min-width: 24px;
}
QScrollBar::handle:horizontal:hover { background: #7F8C9A; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ── Labels ──────────────────────────────────────── */
QLabel {
    background: transparent;
    color: #000000;
}
)";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Fusion renders consistently across platforms and works well with custom QSS
    a.setStyle(QStyleFactory::create("Fusion"));

    QFont appFont("Segoe UI", 10);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    a.setFont(appFont);

    a.setStyleSheet(APP_STYLE);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "VPEToolkit_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
