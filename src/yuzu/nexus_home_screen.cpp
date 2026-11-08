// SPDX-FileCopyrightText: Copyright 2026 NEXUS OS contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "nexus_home_screen.h"

#include <algorithm>

#include <QDateTime>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
constexpr int ItemTypeRole = Qt::UserRole + 1;
constexpr int TitleRole = Qt::UserRole + 3;
constexpr int FullPathRole = Qt::UserRole + 4;
constexpr int ProgramIdRole = Qt::UserRole + 5;
constexpr int GameItemType = QStandardItem::UserType + 1;

QString CardStyle() {
    return QStringLiteral(
        "QToolButton { background:#ffffff; color:#202631; border:1px solid #dfe3e9;"
        " border-radius:14px; padding:10px; font-size:13px; font-weight:600; }"
        "QToolButton:hover { border:2px solid #5b9bea; background:#f8fbff; }"
        "QToolButton:focus { border:3px solid #2686e8; background:#ffffff; }"
        "QToolButton:pressed { background:#e9eff8; }");
}

QPushButton* MakeNavButton(const QString& text, QWidget* parent) {
    auto* button = new QPushButton(text, parent);
    button->setMinimumHeight(42);
    button->setCursor(Qt::PointingHandCursor);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setStyleSheet(QStringLiteral(
        "QPushButton { background:transparent; color:#515b6b; border:0; border-radius:10px;"
        " padding:8px 14px; font-size:13px; font-weight:600; }"
        "QPushButton:hover { background:#e4eaf2; color:#202631; }"
        "QPushButton:focus { border:2px solid #2686e8; }"
        "QPushButton:pressed { background:#d8e2ef; }"));
    return button;
}

void CollectGames(QStandardItem* parent, QList<QStandardItem*>& games) {
    if (!parent)
        return;
    for (int row = 0; row < parent->rowCount(); ++row) {
        auto* item = parent->child(row, 0);
        if (!item)
            continue;
        if (item->data(ItemTypeRole).toInt() == GameItemType &&
            !item->data(FullPathRole).toString().isEmpty()) {
            games.append(item);
        } else if (item->hasChildren()) {
            CollectGames(item, games);
        }
    }
}
} // namespace

NexusHomeScreen::NexusHomeScreen(QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("NexusHomeScreen"));
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet(QStringLiteral("#NexusHomeScreen { background:#eef0f4; }"));

    QSettings settings;
    settings.beginGroup(QStringLiteral("NEXUS"));
    profile_name = settings.value(QStringLiteral("profileName"), QStringLiteral("Player 1")).toString();
    settings.endGroup();

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(38, 24, 38, 20);
    root->setSpacing(16);

    auto* top = new QHBoxLayout;
    auto* identity = new QLabel(QStringLiteral("NEXUS"), this);
    identity->setStyleSheet(QStringLiteral("color:#202631; font-size:22px; font-weight:800; letter-spacing:3px;"));
    top->addWidget(identity);

    auto* home_tag = new QLabel(QStringLiteral("HOME"), this);
    home_tag->setStyleSheet(QStringLiteral("color:#7b8492; font-size:11px; font-weight:700; letter-spacing:2px;"));
    top->addWidget(home_tag);
    top->addStretch();

    profile_label = new QLabel(this);
    profile_label->setText(QStringLiteral("●  %1").arg(profile_name));
    profile_label->setStyleSheet(QStringLiteral(
        "QLabel { color:#3d4654; background:#ffffff; border:1px solid #dfe3e9;"
        " border-radius:16px; padding:8px 13px; font-size:12px; font-weight:600; }"));
    top->addWidget(profile_label);

    auto* profile_button = MakeNavButton(QStringLiteral("Edit profile"), this);
    top->addWidget(profile_button);
    connect(profile_button, &QPushButton::clicked, this, [this] {
        bool accepted = false;
        const QString name = QInputDialog::getText(this, tr("NEXUS profile"),
            tr("Local display name (stored on this PC):"), QLineEdit::Normal,
            profile_name, &accepted).trimmed();
        if (accepted && !name.isEmpty())
            SetProfileName(name.left(32));
    });

    clock_label = new QLabel(this);
    clock_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    clock_label->setMinimumWidth(178);
    clock_label->setStyleSheet(QStringLiteral("color:#424b58; font-size:13px; font-weight:600;"));
    top->addWidget(clock_label);
    root->addLayout(top);

    auto* welcome = new QLabel(QStringLiteral("Your games. Your space."), this);
    welcome->setStyleSheet(QStringLiteral("color:#202631; font-size:27px; font-weight:650;"));
    root->addWidget(welcome);

    auto* library_header = new QHBoxLayout;
    auto* games_title = new QLabel(QStringLiteral("GAMES"), this);
    games_title->setStyleSheet(QStringLiteral("color:#667181; font-size:11px; font-weight:800; letter-spacing:2px;"));
    library_header->addWidget(games_title);
    game_count_label = new QLabel(QStringLiteral("Scanning your library…"), this);
    game_count_label->setStyleSheet(QStringLiteral("color:#8a929e; font-size:11px;"));
    library_header->addWidget(game_count_label);
    library_header->addStretch();

    search_edit = new QLineEdit(this);
    search_edit->setPlaceholderText(QStringLiteral("Search games"));
    search_edit->setClearButtonEnabled(true);
    search_edit->setFixedWidth(210);
    search_edit->setStyleSheet(QStringLiteral(
        "QLineEdit { background:#ffffff; color:#242a34; border:1px solid #dce1e8;"
        " border-radius:10px; padding:8px 11px; }"
        "QLineEdit:focus { border:2px solid #5b9bea; }"));
    library_header->addWidget(search_edit);
    root->addLayout(library_header);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setMinimumHeight(190);
    scroll->setMaximumHeight(220);
    scroll->setStyleSheet(QStringLiteral("QScrollArea { background:transparent; }"));
    game_cards_host = new QWidget(scroll);
    game_cards_host->setStyleSheet(QStringLiteral("background:transparent;"));
    auto* cards_layout = new QHBoxLayout(game_cards_host);
    cards_layout->setContentsMargins(2, 4, 2, 8);
    cards_layout->setSpacing(14);
    cards_layout->addStretch();
    scroll->setWidget(game_cards_host);
    root->addWidget(scroll);

    auto* empty_hint = new QLabel(
        QStringLiteral("Your library is managed by Lichii. Add a game directory in Game Library to see your games here."),
        this);
    empty_hint->setWordWrap(true);
    empty_hint->setStyleSheet(QStringLiteral("color:#7b8492; font-size:12px;"));
    root->addWidget(empty_hint);

    root->addStretch(1);

    auto* nav = new QHBoxLayout;
    nav->setSpacing(7);
    auto* library = MakeNavButton(QStringLiteral("▦  Game Library"), this);
    auto* controllers = MakeNavButton(QStringLiteral("◉  Controllers"), this);
    auto* album = MakeNavButton(QStringLiteral("▧  Album"), this);
    auto* store = MakeNavButton(QStringLiteral("◇  Store"), this);
    auto* settings_button = MakeNavButton(QStringLiteral("⚙  Settings"), this);
    nav->addWidget(library);
    nav->addWidget(controllers);
    nav->addWidget(album);
    nav->addWidget(store);
    nav->addWidget(settings_button);
    nav->addStretch();
    root->addLayout(nav);

    auto* footer = new QLabel(
        QStringLiteral("NEXUS OS is an independent community interface for Lichii. Not affiliated with Nintendo."),
        this);
    footer->setStyleSheet(QStringLiteral("color:#9aa1ab; font-size:10px;"));
    root->addWidget(footer);

    connect(library, &QPushButton::clicked, this, &NexusHomeScreen::OpenLibraryRequested);
    connect(settings_button, &QPushButton::clicked, this, &NexusHomeScreen::SettingsRequested);
    connect(controllers, &QPushButton::clicked, this, &NexusHomeScreen::SettingsRequested);
    connect(album, &QPushButton::clicked, this, &NexusHomeScreen::SettingsRequested);
    connect(store, &QPushButton::clicked, this, &NexusHomeScreen::StoreRequested);
    connect(search_edit, &QLineEdit::textChanged, this, &NexusHomeScreen::BuildGameCards);

    clock_timer = new QTimer(this);
    clock_timer->setInterval(1000);
    connect(clock_timer, &QTimer::timeout, this, [this] {
        clock_label->setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM   h:mm AP")));
    });
    clock_timer->start();
    clock_label->setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM   h:mm AP")));
    BuildGameCards();
}

void NexusHomeScreen::SetGameModel(QStandardItemModel* model) {
    if (game_model == model)
        return;
    if (game_model)
        disconnect(game_model, nullptr, this, nullptr);
    game_model = model;
    if (game_model) {
        connect(game_model, &QStandardItemModel::modelReset, this, &NexusHomeScreen::BuildGameCards);
        connect(game_model, &QStandardItemModel::rowsInserted, this, &NexusHomeScreen::BuildGameCards);
        connect(game_model, &QStandardItemModel::rowsRemoved, this, &NexusHomeScreen::BuildGameCards);
        connect(game_model, &QStandardItemModel::dataChanged, this, &NexusHomeScreen::BuildGameCards);
    }
    BuildGameCards();
}

void NexusHomeScreen::RefreshGames() {
    BuildGameCards();
}

void NexusHomeScreen::SetProfileName(const QString& name) {
    profile_name = name;
    profile_label->setText(QStringLiteral("●  %1").arg(profile_name));
    QSettings settings;
    settings.beginGroup(QStringLiteral("NEXUS"));
    settings.setValue(QStringLiteral("profileName"), profile_name);
    settings.endGroup();
    settings.sync();
}

void NexusHomeScreen::BuildGameCards() {
    if (!game_cards_host)
        return;
    auto* layout = qobject_cast<QHBoxLayout*>(game_cards_host->layout());
    if (!layout)
        return;

    while (QLayoutItem* item = layout->takeAt(0)) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    QList<QStandardItem*> games;
    if (game_model) {
        for (int row = 0; row < game_model->rowCount(); ++row)
            CollectGames(game_model->item(row, 0), games);
    }

    std::sort(games.begin(), games.end(), [](const QStandardItem* left, const QStandardItem* right) {
        return left->data(TitleRole).toString().compare(right->data(TitleRole).toString(),
            Qt::CaseInsensitive) < 0;
    });

    const QString filter = search_edit ? search_edit->text().trimmed() : QString{};
    int visible_count = 0;
    for (auto* game : games) {
        const QString title = game->data(TitleRole).toString();
        if (!filter.isEmpty() && !title.contains(filter, Qt::CaseInsensitive))
            continue;

        auto* card = new QToolButton(game_cards_host);
        card->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        card->setText(title.isEmpty() ? game->text() : title);
        card->setIcon(QIcon::fromTheme(QStringLiteral("applications-games")));
        const QIcon game_icon = qvariant_cast<QIcon>(game->data(Qt::DecorationRole));
        if (!game_icon.isNull())
            card->setIcon(game_icon);
        else if (game->data(Qt::DecorationRole).canConvert<QPixmap>())
            card->setIcon(QIcon(QPixmap::fromImage(game->data(Qt::DecorationRole).value<QImage>())));
        card->setIconSize(QSize(112, 112));
        card->setFixedSize(154, 166);
        card->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        card->setToolTip(game->data(Qt::ToolTipRole).toString());
        card->setStyleSheet(CardStyle());
        card->setCursor(Qt::PointingHandCursor);
        card->setFocusPolicy(Qt::StrongFocus);

        const QString path = game->data(FullPathRole).toString();
        const quint64 program_id = game->data(ProgramIdRole).toULongLong();
        connect(card, &QToolButton::clicked, this, [this, path, program_id] {
            emit LaunchGameRequested(path, program_id);
        });
        layout->addWidget(card);
        ++visible_count;
    }
    layout->addStretch();

    if (game_count_label) {
        game_count_label->setText(QStringLiteral("%1 game%2")
            .arg(visible_count).arg(visible_count == 1 ? QString{} : QStringLiteral("s")));
    }
}
