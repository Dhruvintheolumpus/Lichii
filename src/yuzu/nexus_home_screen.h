// SPDX-FileCopyrightText: Copyright 2026 NEXUS OS contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

class QLabel;

class NexusHomeScreen final : public QWidget {
    Q_OBJECT
public:
    explicit NexusHomeScreen(QWidget* parent = nullptr);

signals:
    void OpenLibraryRequested();
    void SettingsRequested();
    void StoreRequested();

private:
    QLabel* clock_label = nullptr;
};
