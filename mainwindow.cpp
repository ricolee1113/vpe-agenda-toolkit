#include "mainwindow.h"
#include "speechwidget.h"

#include <QApplication>
#include <QTabWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDir>
#include <QTemporaryFile>
#include <QDate>
#include <QStandardPaths>
#include <QScrollBar>
#include <QSettings>
#include <QFrame>

// -------------------------------------------------------------------------
// Constructor / setup
// -------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    loadOfficers();   // populate Officers tab and sync SAA/President
    loadDraft();      // restore last meeting draft (if any)
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("VPE Toolkit – Meeting Agenda Generator"));
    resize(960, 780);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    auto *tabs = new QTabWidget;
    mainLayout->addWidget(tabs);

    tabs->addTab(buildMeetingTab(),   tr("Meeting Info && Roles"));
    tabs->addTab(buildOfficersTab(),  tr("Officers"));
    tabs->addTab(buildSpeechesTab(),  tr("Speeches"));
    tabs->addTab(buildOutputTab(),    tr("Generate"));

    // Auto-sync Officers English Name → President / SAA in Tab 1
    // Officers[0] = President, Officers[6] = SAA
    connect(m_officers[0].nameEn, &QLineEdit::textChanged, m_president, &QLineEdit::setText);
    connect(m_officers[6].nameEn, &QLineEdit::textChanged, m_saa,       &QLineEdit::setText);

    addSpeech();
    addSpeech();
    addNextMeeting();
}

// -------------------------------------------------------------------------
// Tab 1 – Meeting Info & Roles
// -------------------------------------------------------------------------

QWidget *MainWindow::buildMeetingTab()
{
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);

    auto *container = new QWidget;
    auto *vbox      = new QVBoxLayout(container);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(12);

    // --- Meeting Info group ---
    auto *infoGroup = new QGroupBox(tr("Meeting Information"));
    auto *infoForm  = new QFormLayout(infoGroup);
    infoForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_meetingNumber = new QLineEdit;
    m_meetingNumber->setPlaceholderText(tr("e.g. 249th"));

    m_date = new QDateEdit(QDate::currentDate());
    m_date->setCalendarPopup(true);
    m_date->setDisplayFormat("MMM. d (dddd)");

    m_theme         = new QLineEdit;
    m_themeQuestion = new QLineEdit;
    m_themeQuestion->setPlaceholderText(tr("What is the best 'investment' you've ever made?"));

    infoForm->addRow(tr("Meeting Number"), m_meetingNumber);
    infoForm->addRow(tr("Date"),           m_date);
    infoForm->addRow(tr("Theme"),          m_theme);
    infoForm->addRow(tr("Theme Question"), m_themeQuestion);

    // --- Roles group ---
    auto *rolesGroup = new QGroupBox(tr("Role Assignments"));
    auto *rolesForm  = new QFormLayout(rolesGroup);
    rolesForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_saa       = new QLineEdit;
    m_president = new QLineEdit;
    m_tme               = new QLineEdit;
    m_timer             = new QLineEdit;
    m_ahCounter         = new QLineEdit;
    m_voteCounter       = new QLineEdit;
    m_varietySession    = new QLineEdit;
    m_tableTopicsMaster = new QLineEdit;
    m_languageEvaluator = new QLineEdit;

    rolesForm->addRow(tr("President (from Officers)"), m_president);
    rolesForm->addRow(tr("SAA (from Officers)"),       m_saa);
    rolesForm->addRow(tr("TME"),                       m_tme);
    rolesForm->addRow(tr("Timer"),                     m_timer);
    rolesForm->addRow(tr("Ah Counter"),                m_ahCounter);
    rolesForm->addRow(tr("Vote Counter"),              m_voteCounter);
    rolesForm->addRow(tr("Variety Session"),           m_varietySession);
    rolesForm->addRow(tr("Table Topics Master"),       m_tableTopicsMaster);
    rolesForm->addRow(tr("Language Evaluator"),        m_languageEvaluator);

    // --- Next Meeting Announcements group ---
    auto *nextGroup  = new QGroupBox(tr("Next Meeting Announcements"));
    auto *nextVBox   = new QVBoxLayout(nextGroup);

    m_nextMeetingLayout = new QVBoxLayout;
    m_nextMeetingLayout->setSpacing(4);

    m_btnAddNextMeeting = new QPushButton(tr("+ Add Announcement"));
    m_btnAddNextMeeting->setObjectName("addBtn");
    m_btnAddNextMeeting->setFixedWidth(160);

    nextVBox->addLayout(m_nextMeetingLayout);
    nextVBox->addWidget(m_btnAddNextMeeting, 0, Qt::AlignLeft);

    connect(m_btnAddNextMeeting, &QPushButton::clicked, this, &MainWindow::addNextMeeting);

    auto *btnSaveDraft1 = new QPushButton(tr("Save Draft"));
    btnSaveDraft1->setObjectName("saveOfficersBtn");
    btnSaveDraft1->setFixedWidth(110);
    connect(btnSaveDraft1, &QPushButton::clicked, this, &MainWindow::saveDraft);

    vbox->addWidget(infoGroup);
    vbox->addWidget(rolesGroup);
    vbox->addWidget(nextGroup);
    vbox->addWidget(btnSaveDraft1, 0, Qt::AlignLeft);
    vbox->addStretch();

    scroll->setWidget(container);
    return scroll;
}

// -------------------------------------------------------------------------
// Tab 2 – Speeches
// -------------------------------------------------------------------------

QWidget *MainWindow::buildSpeechesTab()
{
    auto *outer = new QWidget;
    auto *vbox  = new QVBoxLayout(outer);
    vbox->setContentsMargins(8, 8, 8, 8);

    auto *btnBar = new QHBoxLayout;
    auto *btnAdd = new QPushButton(tr("+ Add Speech"));
    btnAdd->setObjectName("addBtn");
    auto *lbl    = new QLabel(tr("Supports 1–4 prepared speeches."));
    auto *btnSaveDraft2 = new QPushButton(tr("Save Draft"));
    btnSaveDraft2->setObjectName("saveOfficersBtn");
    btnSaveDraft2->setFixedWidth(110);
    connect(btnSaveDraft2, &QPushButton::clicked, this, &MainWindow::saveDraft);
    btnBar->addWidget(btnAdd);
    btnBar->addWidget(lbl);
    btnBar->addStretch();
    btnBar->addWidget(btnSaveDraft2);

    m_speechScroll    = new QScrollArea;
    m_speechContainer = new QWidget;
    m_speechLayout    = new QVBoxLayout(m_speechContainer);
    m_speechLayout->setContentsMargins(4, 4, 4, 4);
    m_speechLayout->setSpacing(8);
    m_speechLayout->addStretch();

    m_speechScroll->setWidget(m_speechContainer);
    m_speechScroll->setWidgetResizable(true);

    vbox->addLayout(btnBar);
    vbox->addWidget(m_speechScroll);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addSpeech);
    return outer;
}

// -------------------------------------------------------------------------
// Tab 3 – Output / Generate
// -------------------------------------------------------------------------

QWidget *MainWindow::buildOutputTab()
{
    auto *outer = new QWidget;
    auto *vbox  = new QVBoxLayout(outer);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(12);

    auto *dirGroup  = new QGroupBox(tr("Output Directory"));
    auto *dirH      = new QHBoxLayout(dirGroup);
    m_outputDir     = new QLineEdit;
    m_outputDir->setText(
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    auto *btnBrowse = new QPushButton(tr("Browse…"));
    dirH->addWidget(m_outputDir);
    dirH->addWidget(btnBrowse);

    m_btnGenerate = new QPushButton(tr("Generate Word + PDF"));
    m_btnGenerate->setObjectName("generateBtn");

    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setPlaceholderText(tr("Generation log will appear here…"));

    vbox->addWidget(dirGroup);
    vbox->addWidget(m_btnGenerate);
    vbox->addWidget(new QLabel(tr("Log:")));
    vbox->addWidget(m_logView, 1);

    connect(btnBrowse,     &QPushButton::clicked, this, &MainWindow::browseOutputDir);
    connect(m_btnGenerate, &QPushButton::clicked, this, &MainWindow::generate);

    return outer;
}

// -------------------------------------------------------------------------
// Officers tab
// -------------------------------------------------------------------------

// Fixed officer definitions: {settingsKey, English label, Full bilingual doc label}
static const struct { const char *key; const char *en; const char *full; }
OFFICER_DEFS[] = {
    { "president",  "President",                    "President 會長"                       },
    { "vpe",        "VP Education",                 "Vice President of Education 教育副會長" },
    { "vpm",        "VP Membership",                "Membership Team 會員副會長"             },
    { "vppr",       "VP Public Relations",          "Public Relation Team 公關副會長"        },
    { "secretary",  "Secretary",                    "Secretary 秘書長"                      },
    { "treasurer",  "Treasurer",                    "Treasurer 財務長"                      },
    { "saa",        "Sergeant at Arms (SAA)",       "Sergeant at Arms 總務"                 },
};
static constexpr int OFFICER_COUNT = 7;

QWidget *MainWindow::buildOfficersTab()
{
    auto *scroll     = new QScrollArea;
    scroll->setWidgetResizable(true);

    auto *container  = new QWidget;
    auto *vbox       = new QVBoxLayout(container);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(12);

    auto *group      = new QGroupBox(tr("Club Officers  (term ~1 year)"));
    auto *grid       = new QGridLayout(group);
    grid->setColumnStretch(1, 2);  // English name
    grid->setColumnStretch(2, 2);  // Chinese name
    grid->setColumnStretch(3, 3);  // Email
    grid->setColumnStretch(4, 1);  // Extension

    // Header row
    auto makeHdr = [](const QString &t) {
        auto *l = new QLabel(t);
        QFont f = l->font(); f.setBold(true); l->setFont(f);
        return l;
    };
    grid->addWidget(makeHdr(tr("Position")),    0, 0);
    grid->addWidget(makeHdr(tr("English Name")),0, 1);
    grid->addWidget(makeHdr(tr("中文名")),       0, 2);
    grid->addWidget(makeHdr(tr("Email")),        0, 3);
    grid->addWidget(makeHdr(tr("Extension")),    0, 4);

    m_officers.clear();
    for (int i = 0; i < OFFICER_COUNT; ++i) {
        const auto &def = OFFICER_DEFS[i];
        auto *lbl    = new QLabel(tr(def.en));
        auto *nameEn = new QLineEdit;
        auto *nameZh = new QLineEdit;
        auto *mail   = new QLineEdit;
        auto *ext    = new QLineEdit;
        ext->setFixedWidth(65);
        nameEn->setPlaceholderText(tr("e.g. Tina Tsai"));
        nameZh->setPlaceholderText(tr("e.g. 蔡欣穎"));
        mail->setPlaceholderText(tr("email@asus.com"));

        grid->addWidget(lbl,    i + 1, 0);
        grid->addWidget(nameEn, i + 1, 1);
        grid->addWidget(nameZh, i + 1, 2);
        grid->addWidget(mail,   i + 1, 3);
        grid->addWidget(ext,    i + 1, 4);

        m_officers.append({ def.key, def.en, def.full, nameEn, nameZh, mail, ext });

        for (auto *field : {nameEn, nameZh, mail, ext})
            connect(field, &QLineEdit::editingFinished, this, &MainWindow::saveOfficersSilent);
    }

    auto *note = new QLabel(tr(
        "★ Changes to President and SAA name are automatically reflected in Meeting Info tab."));
    note->setWordWrap(true);

    auto *btnSave = new QPushButton(tr("Save Officers"));
    btnSave->setObjectName("saveOfficersBtn");
    btnSave->setFixedWidth(120);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveOfficers);

    vbox->addWidget(group);
    vbox->addWidget(note);
    vbox->addWidget(btnSave, 0, Qt::AlignLeft);
    vbox->addStretch();

    scroll->setWidget(container);
    return scroll;
}

static QString officersIniPath()
{
    return QApplication::applicationDirPath() + "/officers.ini";
}

void MainWindow::loadOfficers()
{
    QSettings s(officersIniPath(), QSettings::IniFormat);
    for (auto &o : m_officers) {
        o.nameEn->setText(s.value(QString("officers/%1_name_en").arg(o.key)).toString());
        o.nameZh->setText(s.value(QString("officers/%1_name_zh").arg(o.key)).toString());
        o.email->setText(s.value(QString("officers/%1_email").arg(o.key)).toString());
        o.ext->setText(s.value(QString("officers/%1_ext").arg(o.key)).toString());
    }
    // Sync to Tab 1 on first load (textChanged won't fire for programmatic setText)
    m_president->setText(m_officers[0].nameEn->text());
    m_saa->setText(m_officers[6].nameEn->text());
}

void MainWindow::saveOfficersSilent()
{
    QSettings s(officersIniPath(), QSettings::IniFormat);
    for (const auto &o : m_officers) {
        s.setValue(QString("officers/%1_name_en").arg(o.key), o.nameEn->text().trimmed());
        s.setValue(QString("officers/%1_name_zh").arg(o.key), o.nameZh->text().trimmed());
        s.setValue(QString("officers/%1_email").arg(o.key),   o.email->text().trimmed());
        s.setValue(QString("officers/%1_ext").arg(o.key),     o.ext->text().trimmed());
    }
    s.sync();
}

void MainWindow::saveOfficers()
{
    saveOfficersSilent();
    QMessageBox::information(this, tr("Saved"), tr("Officer information saved."));
}

// -------------------------------------------------------------------------
// Meeting draft save / load
// -------------------------------------------------------------------------

static QString draftIniPath()
{
    return QApplication::applicationDirPath() + "/meeting_draft.ini";
}

void MainWindow::saveDraft()
{
    QSettings s(draftIniPath(), QSettings::IniFormat);

    s.beginGroup("meeting");
    s.setValue("number",         m_meetingNumber->text().trimmed());
    s.setValue("date",           m_date->date().toString(Qt::ISODate));
    s.setValue("theme",          m_theme->text().trimmed());
    s.setValue("theme_question", m_themeQuestion->text().trimmed());
    s.endGroup();

    s.beginGroup("roles");
    s.setValue("tme",                 m_tme->text().trimmed());
    s.setValue("timer",               m_timer->text().trimmed());
    s.setValue("ah_counter",          m_ahCounter->text().trimmed());
    s.setValue("vote_counter",        m_voteCounter->text().trimmed());
    s.setValue("variety_session",     m_varietySession->text().trimmed());
    s.setValue("table_topics_master", m_tableTopicsMaster->text().trimmed());
    s.setValue("language_evaluator",  m_languageEvaluator->text().trimmed());
    s.endGroup();

    s.beginGroup("next_meetings");
    s.setValue("count", m_nextMeetings.size());
    for (int i = 0; i < m_nextMeetings.size(); ++i) {
        s.setValue(QString("%1_type").arg(i), m_nextMeetings[i].type->text().trimmed());
        s.setValue(QString("%1_date").arg(i), m_nextMeetings[i].date->text().trimmed());
    }
    s.endGroup();

    s.beginGroup("speeches");
    s.setValue("count", m_speeches.size());
    for (int i = 0; i < m_speeches.size(); ++i) {
        auto d = m_speeches[i]->data();
        s.setValue(QString("%1_pathway_full").arg(i),       d.pathwayFull);
        s.setValue(QString("%1_level_project_full").arg(i), d.levelProjectFull);
        s.setValue(QString("%1_title").arg(i),              d.title);
        s.setValue(QString("%1_speaker").arg(i),            d.speaker);
        s.setValue(QString("%1_project_name").arg(i),       d.projectName);
        s.setValue(QString("%1_purpose").arg(i),            d.purpose);
        s.setValue(QString("%1_evaluator").arg(i),          d.evaluator);
        s.setValue(QString("%1_evaluator_suffix").arg(i),   d.evaluatorSuffix);
    }
    s.endGroup();

    s.setValue("output_dir", m_outputDir->text().trimmed());
    s.sync();

    QMessageBox::information(this, tr("Saved"), tr("Draft saved."));
}

void MainWindow::loadDraft()
{
    if (!QFile::exists(draftIniPath())) return;
    QSettings s(draftIniPath(), QSettings::IniFormat);

    s.beginGroup("meeting");
    m_meetingNumber->setText(s.value("number").toString());
    QDate d = QDate::fromString(s.value("date").toString(), Qt::ISODate);
    if (d.isValid()) m_date->setDate(d);
    m_theme->setText(s.value("theme").toString());
    m_themeQuestion->setText(s.value("theme_question").toString());
    s.endGroup();

    s.beginGroup("roles");
    m_tme->setText(s.value("tme").toString());
    m_timer->setText(s.value("timer").toString());
    m_ahCounter->setText(s.value("ah_counter").toString());
    m_voteCounter->setText(s.value("vote_counter").toString());
    m_varietySession->setText(s.value("variety_session").toString());
    m_tableTopicsMaster->setText(s.value("table_topics_master").toString());
    m_languageEvaluator->setText(s.value("language_evaluator").toString());
    s.endGroup();

    s.beginGroup("next_meetings");
    int nmCount = s.value("count", 0).toInt();
    if (nmCount > 0) {
        // Clear default entries first
        while (m_nextMeetings.size() > 0) {
            auto entry = m_nextMeetings.takeFirst();
            m_nextMeetingLayout->removeWidget(entry.row);
            entry.row->deleteLater();
        }
        m_btnAddNextMeeting->setEnabled(true);
        for (int i = 0; i < qMin(nmCount, 3); ++i) {
            addNextMeeting();
            m_nextMeetings[i].type->setText(s.value(QString("%1_type").arg(i)).toString());
            m_nextMeetings[i].date->setText(s.value(QString("%1_date").arg(i)).toString());
        }
    }
    s.endGroup();

    s.beginGroup("speeches");
    int spCount = s.value("count", 0).toInt();
    if (spCount > 0) {
        // Clear default speeches
        while (!m_speeches.isEmpty()) {
            auto *sw = m_speeches.takeFirst();
            m_speechLayout->removeWidget(sw);
            sw->deleteLater();
        }
        for (int i = 0; i < qMin(spCount, 4); ++i) {
            addSpeech();
            SpeechWidget::SpeechData d;
            d.pathwayFull      = s.value(QString("%1_pathway_full").arg(i)).toString();
            d.levelProjectFull = s.value(QString("%1_level_project_full").arg(i)).toString();
            d.title            = s.value(QString("%1_title").arg(i)).toString();
            d.speaker          = s.value(QString("%1_speaker").arg(i)).toString();
            d.projectName      = s.value(QString("%1_project_name").arg(i)).toString();
            d.purpose          = s.value(QString("%1_purpose").arg(i)).toString();
            d.evaluator        = s.value(QString("%1_evaluator").arg(i)).toString();
            d.evaluatorSuffix  = s.value(QString("%1_evaluator_suffix").arg(i)).toString();
            m_speeches[i]->setData(d);
        }
    }
    s.endGroup();

    QString outDir = s.value("output_dir").toString();
    if (!outDir.isEmpty()) m_outputDir->setText(outDir);
}

// -------------------------------------------------------------------------
// Next Meeting entries
// -------------------------------------------------------------------------

void MainWindow::addNextMeeting()
{
    if (m_nextMeetings.size() >= 3) return;

    const int idx = m_nextMeetings.size();

    auto *row     = new QWidget;
    auto *hbox    = new QHBoxLayout(row);
    hbox->setContentsMargins(0, 0, 0, 0);

    auto *lblIdx  = new QLabel(QString("%1.").arg(idx + 1));
    lblIdx->setFixedWidth(18);

    auto *type    = new QLineEdit;
    type->setPlaceholderText(tr("Meeting type, e.g. Regular Meeting"));

    auto *date    = new QLineEdit;
    date->setPlaceholderText(tr("Date, e.g. 4/23 (Thursday)"));
    date->setFixedWidth(160);

    auto *btnDel  = new QPushButton(tr("✕"));
    btnDel->setObjectName("delAnnouncementBtn");
    btnDel->setFixedWidth(28);

    hbox->addWidget(lblIdx);
    hbox->addWidget(type, 1);
    hbox->addWidget(date);
    hbox->addWidget(btnDel);

    m_nextMeetingLayout->addWidget(row);
    m_nextMeetings.append({ type, date, row });

    // Disable "Add" if at limit
    m_btnAddNextMeeting->setEnabled(m_nextMeetings.size() < 3);

    connect(btnDel, &QPushButton::clicked, this, [this, idx]() {
        removeNextMeeting(idx);
    });
}

void MainWindow::removeNextMeeting(int index)
{
    // Always keep at least one entry
    if (m_nextMeetings.size() <= 1) return;
    if (index < 0 || index >= m_nextMeetings.size()) return;

    auto entry = m_nextMeetings.takeAt(index);
    m_nextMeetingLayout->removeWidget(entry.row);
    entry.row->deleteLater();

    // Re-number the remaining labels
    for (int i = index; i < m_nextMeetings.size(); ++i) {
        auto *lbl = qobject_cast<QLabel *>(
            m_nextMeetings[i].row->layout()->itemAt(0)->widget());
        if (lbl) lbl->setText(QString("%1.").arg(i + 1));
    }

    m_btnAddNextMeeting->setEnabled(true);
}

// -------------------------------------------------------------------------
// Speech management
// -------------------------------------------------------------------------

void MainWindow::addSpeech()
{
    if (m_speeches.size() >= 4) {
        QMessageBox::information(this, tr("Limit reached"),
                                 tr("Maximum 4 prepared speeches per meeting."));
        return;
    }

    auto *sw = new SpeechWidget(m_speeches.size(), m_speechContainer);
    connect(sw, &SpeechWidget::removeRequested, this, &MainWindow::removeSpeech);

    m_speechLayout->insertWidget(m_speeches.size(), sw);
    m_speeches.append(sw);

    QApplication::processEvents();
    m_speechScroll->verticalScrollBar()->setValue(
        m_speechScroll->verticalScrollBar()->maximum());
}

void MainWindow::removeSpeech(int index)
{
    if (index < 0 || index >= m_speeches.size()) return;

    auto *sw = m_speeches.takeAt(index);
    m_speechLayout->removeWidget(sw);
    sw->deleteLater();

    for (int i = index; i < m_speeches.size(); ++i)
        m_speeches[i]->setIndex(i);
}

// -------------------------------------------------------------------------
// JSON serialisation
// -------------------------------------------------------------------------

QString MainWindow::formatDateDisplay(const QDate &d) const
{
    static const char *abbr[] = {"Jan.","Feb.","Mar.","Apr.","May","Jun.",
                                  "Jul.","Aug.","Sep.","Oct.","Nov.","Dec."};
    int day = d.day();
    QString suffix;
    if      (day >= 11 && day <= 13) suffix = "th";
    else if (day % 10 == 1)          suffix = "st";
    else if (day % 10 == 2)          suffix = "nd";
    else if (day % 10 == 3)          suffix = "rd";
    else                             suffix = "th";

    return QString("%1 %2%3 (%4)")
        .arg(abbr[d.month() - 1])
        .arg(day)
        .arg(suffix)
        .arg(d.toString("dddd"));
}

QJsonObject MainWindow::buildJson() const
{
    QJsonObject root;

    root["meeting_number"]      = m_meetingNumber->text().trimmed();
    root["date_display"]        = formatDateDisplay(m_date->date());
    root["theme"]               = m_theme->text().trimmed();
    root["theme_question"]      = m_themeQuestion->text().trimmed();
    root["saa"]                 = m_saa->text().trimmed();
    root["president"]           = m_president->text().trimmed();
    root["tme"]                 = m_tme->text().trimmed();
    root["timer"]               = m_timer->text().trimmed();
    root["ah_counter"]          = m_ahCounter->text().trimmed();
    root["vote_counter"]        = m_voteCounter->text().trimmed();
    root["variety_session"]     = m_varietySession->text().trimmed();
    root["table_topics_master"] = m_tableTopicsMaster->text().trimmed();
    root["language_evaluator"]  = m_languageEvaluator->text().trimmed();

    // Next meeting announcements list
    QJsonArray nextArr;
    for (const auto &e : m_nextMeetings) {
        QJsonObject item;
        item["type"] = e.type->text().trimmed();
        item["date"] = e.date->text().trimmed();
        nextArr.append(item);
    }
    root["next_meetings"] = nextArr;

    // Officers – name in document format: "English(中文)" or just "English"
    QJsonArray officersArr;
    for (const auto &o : m_officers) {
        QJsonObject oj;
        oj["position"] = o.positionFull;
        const QString en = o.nameEn->text().trimmed();
        const QString zh = o.nameZh->text().trimmed();
        oj["name"]  = zh.isEmpty() ? en : QString("%1(%2)").arg(en, zh);
        oj["email"] = o.email->text().trimmed();
        oj["ext"]   = o.ext->text().trimmed();
        officersArr.append(oj);
    }
    root["officers"] = officersArr;

    // Speeches
    QJsonArray speeches;
    for (const auto *sw : m_speeches) {
        auto d = sw->data();
        QJsonObject sp;
        sp["pathway_abbr"]       = d.pathwayAbbr;
        sp["pathway_full"]       = d.pathwayFull;
        sp["level_project"]      = d.levelProject;
        sp["level_project_full"] = d.levelProjectFull;
        sp["title"]              = d.title;
        sp["speaker"]            = d.speaker;
        sp["project_name"]       = d.projectName;
        sp["purpose"]            = d.purpose;
        sp["evaluator"]          = d.evaluator;
        sp["evaluator_suffix"]   = d.evaluatorSuffix;
        speeches.append(sp);
    }
    root["speeches"] = speeches;
    return root;
}

// -------------------------------------------------------------------------
// Output
// -------------------------------------------------------------------------

void MainWindow::browseOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select Output Folder"), m_outputDir->text());
    if (!dir.isEmpty())
        m_outputDir->setText(dir);
}

void MainWindow::generate()
{
    if (m_speeches.isEmpty()) {
        QMessageBox::warning(this, tr("No Speeches"),
                             tr("Please add at least one prepared speech."));
        return;
    }

    QStringList missing;
    if (m_meetingNumber->text().trimmed().isEmpty()) missing << tr("Meeting Number");
    if (m_president->text().trimmed().isEmpty())     missing << tr("President");
    if (m_tme->text().trimmed().isEmpty())           missing << tr("TME");

    if (!missing.isEmpty()) {
        QMessageBox::warning(this, tr("Missing Fields"),
                             tr("Please fill in: %1").arg(missing.join(", ")));
        return;
    }

    QTemporaryFile tmpJson(QDir::tempPath() + "/vpetoolkit_XXXXXX.json");
    if (!tmpJson.open()) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot create temporary file."));
        return;
    }
    tmpJson.setAutoRemove(false);
    tmpJson.write(QJsonDocument(buildJson()).toJson());
    tmpJson.close();

    QString outputDir = m_outputDir->text().trimmed();
    if (!QDir().mkpath(outputDir)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Cannot create output directory: %1").arg(outputDir));
        return;
    }

    QString pythonExe = "python";
#ifdef Q_OS_WIN
    QString candidate = R"(C:\Users\Spade-10\AppData\Local\Programs\Python\Python313\python.exe)";
    if (QFile::exists(candidate))
        pythonExe = candidate;
#endif

    QString scriptPath = QApplication::applicationDirPath() + "/scripts/generate_agenda.py";
    if (!QFile::exists(scriptPath)) {
        scriptPath = QDir::cleanPath(
            QDir(QApplication::applicationDirPath())
                .absoluteFilePath("../../scripts/generate_agenda.py"));
    }
    if (!QFile::exists(scriptPath)) {
        QMessageBox::critical(this, tr("Script Not Found"),
                              tr("generate_agenda.py not found:\n%1").arg(scriptPath));
        return;
    }

    const QString tmpJsonPath = tmpJson.fileName();

    m_logView->clear();
    m_logView->append(tr("Running generation script…"));
    m_btnGenerate->setEnabled(false);

    auto *proc = new QProcess(this);
    connect(proc, &QProcess::readyReadStandardOutput, this, [proc, this]() {
        const QString out = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
        if (!out.isEmpty()) m_logView->append(out);
    });
    connect(proc, &QProcess::readyReadStandardError, this, [proc, this]() {
        const QString err = QString::fromUtf8(proc->readAllStandardError()).trimmed();
        if (!err.isEmpty())
            m_logView->append("<span style='color:red'>" + err.toHtmlEscaped() + "</span>");
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [proc, this, tmpJsonPath](int exitCode, QProcess::ExitStatus) {
        const QString rem = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
        if (!rem.isEmpty()) m_logView->append(rem);
        const QString remErr = QString::fromUtf8(proc->readAllStandardError()).trimmed();
        if (!remErr.isEmpty())
            m_logView->append("<span style='color:red'>" + remErr.toHtmlEscaped() + "</span>");
        QFile::remove(tmpJsonPath);
        onProcessFinished(exitCode);
        proc->deleteLater();
    });

    proc->start(pythonExe, {scriptPath, tmpJsonPath, outputDir});
    if (!proc->waitForStarted(3000)) {
        m_logView->append("<span style='color:red'>Failed to start Python process.</span>");
        m_btnGenerate->setEnabled(true);
        proc->deleteLater();
    }
}

void MainWindow::onProcessFinished(int exitCode)
{
    m_btnGenerate->setEnabled(true);

    QString log = m_logView->toPlainText();
    if (exitCode == 0) {
        for (const QString &line : log.split('\n', Qt::SkipEmptyParts)) {
            if (line.startsWith("SUCCESS|")) {
                QStringList parts = line.split('|');
                if (parts.size() >= 3) {
                    m_logView->append(tr("<b>✓ Word:</b> %1").arg(parts[1]));
                    m_logView->append(tr("<b>✓ PDF :</b> %1").arg(parts[2]));
                }
            }
        }
        QMessageBox::information(this, tr("Done"),
                                 tr("Files generated in:\n%1").arg(m_outputDir->text()));
    } else {
        m_logView->append(
            "<span style='color:red'><b>Generation failed.</b></span>");
        QMessageBox::critical(this, tr("Generation Failed"),
                              tr("See the log for details."));
    }
}
