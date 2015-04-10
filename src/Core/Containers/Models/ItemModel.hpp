/*
Copyright (C) 2014-2015 by Rafał Soszyński <rsoszynski121 [at] gmail [dot] com>
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
#pragma once

#include <QtCore>
#include "Core/Utils/Serial.hpp"

class ItemBase;

class ItemModel : public QAbstractTableModel {
public:
	static const int Name              = 0;
	static const int Type              = 1;
	static const int Price             = 2;
	static const int Quality           = 3;
	static const int CanBeDrawn        = 4;
	static const int Effects           = 5;
	static const int ColumnCount       = 6;

	explicit ItemModel(QObject * parent = nullptr);
	ItemModel(const ItemModel &) = delete;
	ItemModel(ItemModel &&) = delete;
	void operator=(const ItemModel &) = delete;
	void operator=(ItemModel &&) = delete;
	~ItemModel();

	int columnCount(const QModelIndex & index = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role) const;
	bool empty() const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	bool hasItem(const QString & name) const;
	const ItemBase * itemInRow(int row) const;
	const ItemBase * item(UID uid) const;
	const ItemBase * item(const QModelIndex & index) const;
	const ItemBase * item(const QString & name) const;
	const QList <ItemBase *> & items() const;
	bool isChanged() const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	QDataStream & toDataStream(QDataStream & out) const;

	void addNewItem();
	QDataStream & fromDataStream(QDataStream & in);
	bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	void removeItem(UID uid);
	bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
	void reset();
	void setChanged(bool changed);
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

	//TMP for migration from parsers
	void addItemBase(ItemBase * item);
private:
	void addItem(int row, ItemBase * item);
	void removeItemFromRow(int row);

	bool changed_;
	QList <ItemBase *> items_;
	Serial serial_;
	QHash <UID, ItemBase *> uidToItem_;
};
QDataStream & operator<<(QDataStream & out, const ItemModel & model);
QDataStream & operator>>(QDataStream & in, ItemModel & model);
