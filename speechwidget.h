#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>

/// Form widget for one prepared speech slot.
class SpeechWidget : public QWidget
{
    Q_OBJECT
public:
    struct SpeechData {
        QString pathwayAbbr;       // e.g. "PI"
        QString pathwayFull;       // e.g. "Persuasive Influence"
        QString levelProject;      // e.g. "L1P3"
        QString levelProjectFull;  // e.g. "Level 1-Project 3"
        QString title;
        QString speaker;
        QString projectName;
        QString purpose;
        QString evaluator;
        QString evaluatorSuffix;
    };

    explicit SpeechWidget(int index, QWidget *parent = nullptr);

    SpeechData data() const;
    void setData(const SpeechData &d);
    void setIndex(int index);

    /// Returns the abbreviation for the given full pathway name.
    static QString abbrFor(const QString &fullName);

signals:
    void removeRequested(int index);

private slots:
    void onPathwayChanged(int idx);
    void onLevelProjectChanged(int idx);

private:
    int        m_index;
    QGroupBox *m_group;

    QComboBox *m_pathway;              // dropdown: 6 pathway full names
    QLineEdit *m_pathwayAbbrDisplay;   // read-only, auto-filled
    QComboBox *m_levelProjectCombo;    // dropdown: 14 level-project full names
    QLineEdit *m_levelProjectAbbrDisplay; // read-only, auto-filled
    QLineEdit *m_title;
    QLineEdit *m_speaker;
    QLineEdit *m_projectName;
    QLineEdit *m_purpose;
    QLineEdit *m_evaluator;
    QLineEdit *m_evaluatorSuffix;
};
