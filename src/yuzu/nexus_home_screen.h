// SPDX-FileCopyrightText: Copyright 2026 NEXUS OS contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QLineEdit;
class QStandardItemModel;
class QTimer;

class NexusHomeScreen final : public QWidget {
    Q_OBJECT
public:
    explicit NexusHomeScreen(QWidget* parent = nullptr);
    void SetGameModel(QStandardItemModel* model);
    void RefreshGames();

signals:
    void OpenLibraryRequested();
    void SettingsRequested();
    void StoreRequested();
    void ScreenshotRequested();
    void LaunchGameRequested(const QString& game_path, quint64 program_id);

private:
    void BuildGameCards();
    void SetProfileName(const QString& name);

    QLabel* clock_label = nullptr;
    QLabel* profile_label = nullptr;
    QLabel* game_count_label = nullptr;
    QLineEdit* search_edit = nullptr;
    QWidget* game_cards_host = nullptr;
    QStandardItemModel* game_model = nullptr;
    QTimer* clock_timer = nullptr;
    QString profile_name;
};
