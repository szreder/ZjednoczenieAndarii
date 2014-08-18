﻿/*
Copyright (C) 2014 by Rafał Soszyński <rsoszynski121 [at] gmail [dot] com>
This file is part of The Chronicles Of Andaria Project.

	The Chronicles of Andaria Project is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The Chronicles of Andaria Project is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with The Chronicles Of Andaria.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Editor/MainWindow.h"
#include "Editor/Editors/ItemsEditor.h"

MainWindow::MainWindow()
{
	resize(1024, 768);

	initMenuAndActions();
	initContentEditors();
	initModelsList();
	initEditorsWidgets();
	initLayout();
}

MainWindow::~MainWindow()
{
	qDeleteAll(contentEditors_);
}

void MainWindow::initMenuAndActions()
{
	QMenu *fileMenu = menuBar()->addMenu(Menu::File::Main);

	QAction *menuFileNew    = new QAction(Menu::File::New, this);
	QAction *menuFileLoad   = new QAction(Menu::File::Open, this);
	menuFileSave            = new QAction(Menu::File::Save, this);
	menuFileSaveAs          = new QAction(Menu::File::SaveAs, this);
	QAction *menuFileQuit   = new QAction(Menu::File::Quit, this);

	connect(menuFileNew, &QAction::triggered, this, &MainWindow::onNewActivated);
	connect(menuFileLoad, &QAction::triggered, this, &MainWindow::onLoadActivated);
	connect(menuFileSave, &QAction::triggered, this, &MainWindow::onSaveActivated);
	connect(menuFileSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsActivated);
	connect(menuFileQuit, &QAction::triggered, this, &MainWindow::onQuitActivated);

	menuFileNew->setShortcut(Shortcut::Menu::File::New);
	menuFileLoad->setShortcut(Shortcut::Menu::File::Open);
	menuFileSave->setShortcut(Shortcut::Menu::File::Save);
	menuFileQuit->setShortcut(Shortcut::Menu::File::Quit);

	menuFileSave->setEnabled(false);
	menuFileSaveAs->setEnabled(false);

	fileMenu->addAction(menuFileNew);
	fileMenu->addAction(menuFileLoad);
	fileMenu->addAction(menuFileSave);
	fileMenu->addAction(menuFileSaveAs);
	fileMenu->addSeparator();
	fileMenu->addAction(menuFileQuit);
}

void MainWindow::initContentEditors()
{
	//ADD CONTENT EDITORS HERE
	contentEditors_.append(new ItemsEditor(this));

	for (ContentEditor *editor : contentEditors_)
		connect(this, &MainWindow::modelSaved, editor, &ContentEditor::modelSaved);
}

void MainWindow::initModelsList()
{
	modelsList_ = new QListWidget();
	for (ContentEditor *editor : contentEditors_)
		modelsList_->addItem(editor->title());
}

void MainWindow::initEditorsWidgets()
{
	modelsWidgets_ = new QStackedWidget();
	for (ContentEditor *editor : contentEditors_)
		modelsWidgets_->addWidget(editor->placeholder());

	connect (modelsList_, &QListWidget::currentRowChanged, modelsWidgets_, &QStackedWidget::setCurrentIndex);
}

void MainWindow::initLayout()
{
	placeholder = new QSplitter(this);
	placeholder->addWidget(modelsList_);
	placeholder->addWidget(modelsWidgets_);

	setCentralWidget(placeholder);

	modelsList_->setMaximumWidth(100);

	placeholder->setVisible(false);

	statusBar()->showMessage(Message::HowToStart);
}

void MainWindow::startEditing()
{
	menuFileSave->setEnabled(true);
	menuFileSaveAs->setEnabled(true);

	placeholder->setVisible(true);
}

bool MainWindow::loadContent(const QString &path)
{
	if (path.isEmpty())
		return false;

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&file);

	for (ContentEditor * editor : contentEditors_)
		editor->loadFromStream(in);

	file.close();
	statusBar()->showMessage(Message::ContentLoaded);
	return true;
}

bool MainWindow::saveContent(const QString &path)
{
	if (path.isEmpty())
		return false;

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);

	for (ContentEditor * editor : contentEditors_)
		editor->saveToStream(out);

	file.close();
	statusBar()->showMessage(Message::ContentSaved);
	return true;
}

int MainWindow::checkForUnsavedChanges()
{
	bool unsavedChanges = false;
	for (ContentEditor *editor : contentEditors_)
		if (editor->isChanged()){
			unsavedChanges = true;
			break;
		}
		if (!unsavedChanges)
			return QMessageBox::Discard;

		QMessageBox msgBox;
	msgBox.setText("Content has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);

	return msgBox.exec();
}

void MainWindow::onNewActivated()
{
	startEditing();

	switch (checkForUnsavedChanges()) {
		case QMessageBox::Save:
			onSaveActivated();
			return;
		case QMessageBox::Discard:

			for (ContentEditor *editor : contentEditors_)
				editor->clear();
			return;
		default :
			return;
	}
}

void MainWindow::onLoadActivated()
{
	switch (checkForUnsavedChanges()) {
		case QMessageBox::Save:
			onSaveActivated();
			return;
		case QMessageBox::Discard:
			break;
		default :
			return;
	}

	QString path = QFileDialog::getOpenFileName(this, Title::OpenFileDialog, QString(), String::ContentFiles);
	if (loadContent(path))
		startEditing();
}

void MainWindow::onSaveActivated()
{
	if (openedFilePath.isEmpty())
		onSaveAsActivated();

	if (saveContent(openedFilePath))
		emit modelSaved();
}

void MainWindow::onSaveAsActivated()
{
	QString path = QFileDialog::getSaveFileName(this, Title::SaveFileDialog, QString(), String::ContentFiles);

	if (saveContent(path)){
		emit modelSaved();
		openedFilePath = path;
	}
}

void MainWindow::onQuitActivated()
{
	close();
}
