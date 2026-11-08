// SPDX-FileCopyrightText: Copyright 2026 NEXUS OS contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "nexus_home_screen.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {
QPushButton* MakeAction(const QString& title, const QString& subtitle, QWidget* parent) {
    auto* button = new QPushButton(parent);
    button->setText(title + QStringLiteral("\n") + subtitle);
    button->setMinimumSize(172, 112);
    button->setCursor(Qt::PointingHandCursor);
    button->setFocusPolicy(Qt::StrongFocus);
    button->setStyleSheet(QStringLiteral(
        "QPushButton { background:#f8f9fb; color:#242832; border:1px solid #d9dde5;"
        " border-radius:15px; padding:18px; text-align:left; font-size:17px; font-weight:600; }"
        "QPushButton:hover { background:#ffffff; border:2px solid #5a9be8; }"
        "QPushButton:focus { background:#ffffff; border:3px solid #2686e8; }"
        "QPushButton:pressed { background:#e9eef6; }"));
    return button;
}
}

NexusHomeScreen::NexusHomeScreen(QWidget* parent) : QWidget(parent) {
    setObjectName(QStringLiteral("NexusHomeScreen"));
    setAutoFillBackground(true);
    setStyleSheet(QStringLiteral("#NexusHomeScreen { background:#eef0f4; }"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(52, 34, 52, 28);
    root->setSpacing(22);

    auto* top = new QHBoxLayout;
    auto* identity = new QLabel(QStringLiteral("NEXUS  /  HOME"), this);
    identity->setStyleSheet(QStringLiteral("color:#303744; font-size:17px; font-weight:700; letter-spacing:2px;"));
    top->addWidget(identity);
    top->addStretch();

    clock_label = new QLabel(this);
    clock_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    clock_label->setStyleSheet(QStringLiteral("color:#3b414c; font-size:15px;"));
    top->addWidget(clock_label);
    root->addLayout(top);

    auto* welcome = new QLabel(QStringLiteral("Your games. Your space."), this);
    welcome->setStyleSheet(QStringLiteral("color:#202631; font-size:30px; font-weight:600;"));
    root->addWidget(welcome);

    auto* hint = new QLabel(
        QStringLiteral("A calm, controller-friendly home for your Lichii game library."),
        this);
    hint->setStyleSheet(QStringLiteral("color:#687180; font-size:14px;"));
    root->addWidget(hint);

    root->addStretch(1);

    auto* actions = new QHBoxLayout;
    actions->setSpacing(14);
    auto* library = MakeAction(QStringLiteral("Game Library"), QStringLiteral("Browse and launch games"), this);
    auto* settings = MakeAction(QStringLiteral("System Settings"), QStringLiteral("Configure Lichii"), this);
    auto* store = MakeAction(QStringLiteral("Nintendo Store"), QStringLiteral("Open official store website"), this);
    actions->addWidget(library);
    actions->addWidget(settings);
    actions->addWidget(store);
    actions->addStretch(1);
    root->addLayout(actions);

    auto* footer = new QLabel(
        QStringLiteral("NEXUS OS is an independent community interface for Lichii. Not affiliated with Nintendo."),
        this);
    footer->setStyleSheet(QStringLiteral("color:#8a919d; font-size:11px;"));
    root->addWidget(footer);

    connect(library, &QPushButton::clicked, this, &NexusHomeScreen::OpenLibraryRequested);
    connect(settings, &QPushButton::clicked, this, &NexusHomeScreen::SettingsRequested);
    connect(store, &QPushButton::clicked, this, &NexusHomeScreen::StoreRequested);

    auto* clock_timer = new QTimer(this);
    clock_timer->setInterval(1000);
    connect(clock_timer, &QTimer::timeout, this, [this] {
        clock_label->setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM   h:mm AP")));
    });
    clock_timer->start();
    clock_label->setText(QDateTime::currentDateTime().toString(QStringLiteral("ddd, d MMM   h:mm AP")));
}
