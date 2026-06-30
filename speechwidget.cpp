#include "speechwidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

// ---------------------------------------------------------------------------
// Pathway: full name → abbreviation
// ---------------------------------------------------------------------------
static const QStringList PATHWAY_NAMES = {
    "Dynamic Leadership",
    "Engaging Humor",
    "Motivational Strategies",
    "Persuasive Influence",
    "Presentation Mastery",
    "Visionary Communication",
};
static const QStringList PATHWAY_ABBRS = {
    "DL", "EH", "MS", "PI", "PM", "VC",
};

// ---------------------------------------------------------------------------
// Level & Project: full name → abbreviation (from pathway.txt)
// ---------------------------------------------------------------------------
static const QStringList LEVEL_PROJECT_NAMES = {
    "Level 1-Project 1",
    "Level 1-Project 2",
    "Level 1-Project 3",
    "Level 1-Project 4",
    "Level 2-Project 1",
    "Level 2-Project 2",
    "Level 2-Project 3",
    "Level 3-Project 1",
    "Level 3-Electives 1",
    "Level 3-Electives 2",
    "Level 4-Project 1",
    "Level 4-Electives 1",
    "Level 5-Project 1",
    "Level 5-Electives 1",
};
static const QStringList LEVEL_PROJECT_ABBRS = {
    "L1P1", "L1P2", "L1P3", "L1P4",
    "L2P1", "L2P2", "L2P3",
    "L3P1", "L3E1", "L3E2",
    "L4P1", "L4E1",
    "L5P1", "L5E1",
};

// static helpers
QString SpeechWidget::abbrFor(const QString &fullName)
{
    int idx = PATHWAY_NAMES.indexOf(fullName);
    return (idx >= 0) ? PATHWAY_ABBRS[idx] : "??";
}

static QString levelAbbrFor(const QString &fullName)
{
    int idx = LEVEL_PROJECT_NAMES.indexOf(fullName);
    return (idx >= 0) ? LEVEL_PROJECT_ABBRS[idx] : "??";
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
SpeechWidget::SpeechWidget(int index, QWidget *parent)
    : QWidget(parent)
    , m_index(index)
{
    m_group = new QGroupBox(this);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(m_group);

    auto *topBar    = new QHBoxLayout;
    auto *btnRemove = new QPushButton(tr("Remove"), m_group);
    btnRemove->setObjectName("removeBtn");
    btnRemove->setFixedWidth(70);
    topBar->addStretch();
    topBar->addWidget(btnRemove);

    auto *form = new QFormLayout;
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // --- Pathway row: combo + read-only abbr ---
    m_pathway = new QComboBox;
    m_pathway->addItems(PATHWAY_NAMES);

    m_pathwayAbbrDisplay = new QLineEdit;
    m_pathwayAbbrDisplay->setReadOnly(true);
    m_pathwayAbbrDisplay->setFixedWidth(40);
    m_pathwayAbbrDisplay->setAlignment(Qt::AlignCenter);

    auto *pathwayRow = new QHBoxLayout;
    pathwayRow->addWidget(m_pathway, 1);
    pathwayRow->addWidget(m_pathwayAbbrDisplay);

    // --- Level & Project row: combo + read-only abbr ---
    m_levelProjectCombo = new QComboBox;
    m_levelProjectCombo->addItems(LEVEL_PROJECT_NAMES);

    m_levelProjectAbbrDisplay = new QLineEdit;
    m_levelProjectAbbrDisplay->setReadOnly(true);
    m_levelProjectAbbrDisplay->setFixedWidth(50);
    m_levelProjectAbbrDisplay->setAlignment(Qt::AlignCenter);

    auto *levelRow = new QHBoxLayout;
    levelRow->addWidget(m_levelProjectCombo, 1);
    levelRow->addWidget(m_levelProjectAbbrDisplay);

    auto makeLine = [](const QString &placeholder = {}) {
        auto *le = new QLineEdit;
        if (!placeholder.isEmpty())
            le->setPlaceholderText(placeholder);
        return le;
    };

    m_title           = makeLine("e.g. Whether you believe it or not, I do.");
    m_speaker         = makeLine("e.g. Dylan Li");
    m_projectName     = makeLine("e.g. Introduction to Vocal Variety and Body Language");
    m_purpose         = makeLine("e.g. The purpose of this project is to practice...");
    m_evaluator       = makeLine("e.g. David Liao");
    m_evaluatorSuffix = makeLine("(Pega)  [optional]");

    form->addRow(tr("Pathway"),          pathwayRow);
    form->addRow(tr("Level & Project"),  levelRow);
    form->addRow(tr("Speech Title"),     m_title);
    form->addRow(tr("Speaker"),          m_speaker);
    form->addRow(tr("Project Name"),     m_projectName);
    form->addRow(tr("Purpose"),          m_purpose);
    form->addRow(tr("Evaluator"),        m_evaluator);
    form->addRow(tr("Evaluator Suffix"), m_evaluatorSuffix);

    auto *groupLayout = new QVBoxLayout(m_group);
    groupLayout->addLayout(topBar);
    groupLayout->addLayout(form);

    connect(btnRemove, &QPushButton::clicked, this, [this]() {
        emit removeRequested(m_index);
    });
    connect(m_pathway, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SpeechWidget::onPathwayChanged);
    connect(m_levelProjectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SpeechWidget::onLevelProjectChanged);

    onPathwayChanged(0);
    onLevelProjectChanged(0);

    setIndex(index);
}

void SpeechWidget::onPathwayChanged(int /*idx*/)
{
    m_pathwayAbbrDisplay->setText(abbrFor(m_pathway->currentText()));
}

void SpeechWidget::onLevelProjectChanged(int /*idx*/)
{
    m_levelProjectAbbrDisplay->setText(levelAbbrFor(m_levelProjectCombo->currentText()));
}

SpeechWidget::SpeechData SpeechWidget::data() const
{
    const QString pathwayFull = m_pathway->currentText();
    const QString levelFull   = m_levelProjectCombo->currentText();
    return {
        abbrFor(pathwayFull),
        pathwayFull,
        levelAbbrFor(levelFull),
        levelFull,
        m_title->text().trimmed(),
        m_speaker->text().trimmed(),
        m_projectName->text().trimmed(),
        m_purpose->text().trimmed(),
        m_evaluator->text().trimmed(),
        m_evaluatorSuffix->text().trimmed()
    };
}

void SpeechWidget::setData(const SpeechData &d)
{
    int pIdx = PATHWAY_NAMES.indexOf(d.pathwayFull);
    if (pIdx >= 0) m_pathway->setCurrentIndex(pIdx);

    int lIdx = LEVEL_PROJECT_NAMES.indexOf(d.levelProjectFull);
    if (lIdx >= 0) m_levelProjectCombo->setCurrentIndex(lIdx);

    m_title->setText(d.title);
    m_speaker->setText(d.speaker);
    m_projectName->setText(d.projectName);
    m_purpose->setText(d.purpose);
    m_evaluator->setText(d.evaluator);
    m_evaluatorSuffix->setText(d.evaluatorSuffix);
}

void SpeechWidget::setIndex(int index)
{
    m_index = index;
    m_group->setTitle(tr("Speech %1").arg(index + 1));
}
