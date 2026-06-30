#pragma once
#include <QMainWindow>
#include <QVector>

class QTabWidget;
class QLineEdit;
class QDateEdit;
class QTextEdit;
class QLabel;
class QScrollArea;
class QVBoxLayout;
class QPushButton;
class SpeechWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void addSpeech();
    void removeSpeech(int index);
    void addNextMeeting();
    void removeNextMeeting(int index);
    void saveOfficers();
    void saveDraft();
    void browseOutputDir();
    void generate();
    void onProcessFinished(int exitCode);

private:
    // Meeting Info
    QLineEdit *m_meetingNumber;
    QDateEdit *m_date;
    QLineEdit *m_theme;
    QLineEdit *m_themeQuestion;

    // Core Roles (SAA & President auto-synced from Officers tab)
    QLineEdit *m_saa;
    QLineEdit *m_president;
    QLineEdit *m_tme;
    QLineEdit *m_timer;
    QLineEdit *m_ahCounter;
    QLineEdit *m_voteCounter;
    QLineEdit *m_varietySession;
    QLineEdit *m_tableTopicsMaster;
    QLineEdit *m_languageEvaluator;

    // Next meeting announcements (1–3 entries)
    struct NextMeetingEntry {
        QLineEdit *type;
        QLineEdit *date;
        QWidget   *row;
    };
    QVector<NextMeetingEntry> m_nextMeetings;
    QVBoxLayout *m_nextMeetingLayout;
    QPushButton *m_btnAddNextMeeting;

    // Officers (persistent, ~1-year term)
    struct OfficerEntry {
        QString    key;          // QSettings key prefix
        QString    positionEn;   // English label for UI
        QString    positionFull; // Full bilingual name for document
        QLineEdit *nameEn;       // English name (used in agenda roles)
        QLineEdit *nameZh;       // Chinese name (combined in officer table)
        QLineEdit *email;
        QLineEdit *ext;
    };
    QVector<OfficerEntry> m_officers;

    // Speeches
    QScrollArea  *m_speechScroll;
    QWidget      *m_speechContainer;
    QVBoxLayout  *m_speechLayout;
    QVector<SpeechWidget *> m_speeches;

    // Output
    QLineEdit *m_outputDir;
    QTextEdit *m_logView;
    QPushButton *m_btnGenerate;

    void setupUi();
    QWidget *buildMeetingTab();
    QWidget *buildOfficersTab();
    QWidget *buildSpeechesTab();
    QWidget *buildOutputTab();

    void loadOfficers();
    void saveOfficersSilent();
    void loadDraft();

    QJsonObject buildJson() const;
    QString formatDateDisplay(const QDate &d) const;
};
