/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 SCHUTZ Sacha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "qjsontree.h"
#include <QDebug>
#include <QFile>
#include <QFont>

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem *parent)
{
    mParent = parent;
}

QJsonTreeItem::~QJsonTreeItem()
{
    qDeleteAll(mChilds);
}

void QJsonTreeItem::appendChild(QJsonTreeItem *item)
{
    mChilds.append(item);
}

QJsonTreeItem *QJsonTreeItem::child(int row)
{
    return mChilds.value(row);
}

QJsonTreeItem *QJsonTreeItem::parent()
{
    return mParent;
}

int QJsonTreeItem::childCount() const
{
    return mChilds.count();
}

int QJsonTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<QJsonTreeItem*>(this));

    return 0;
}

void QJsonTreeItem::setValue(const QString &value)
{
    mValue = value;
}

void QJsonTreeItem::setAlias(const QString &alias)
{
    mAlias = alias;
}

void QJsonTreeItem::setType(const QJsonValue::Type &type)
{
    mType = type;
}

QString QJsonTreeItem::value() const
{
    return mValue;
}

QVariant QJsonTreeItem::alias() const
{
    return mAlias;
}

QJsonValue::Type QJsonTreeItem::type() const
{
    return mType;
}

QJsonTreeItem* QJsonTreeItem::load(const QJsonValue& value, QJsonTreeItem* parent)
{
    QJsonTreeItem *rootItem = new QJsonTreeItem(parent);

    if (value.isObject()) {
        //Get all QJsonValue childs
        const QStringList keys = value.toObject().keys();
        for (const QString &key : keys) {
            if (key == "subscriptions") {
                QJsonArray subs = value.toObject().value(key).toArray();

                int index = 0;
                for (const QJsonValue &val : subs) {
                    QJsonTreeItem *child = load(val, rootItem);
                    if (subs.at(index).toObject().contains("topic")) {
                        child->setValue(subs.at(index).toObject().value("topic").toString());
                        child->setType(val.type());
                        rootItem->appendChild(child);
                    }
                    ++index;
                }
            }
        }
    } else if (value.isArray()) {
        //Get all QJsonValue childs
        int index = 0;
        const QJsonArray array = value.toArray();
        for (const QJsonValue &v : array) {
            QJsonTreeItem *child = load(v, rootItem);
            if (array.at(index).toObject().contains("hostname")) {
                child->setValue(array.at(index).toObject().value("hostname").toString());
                child->setType(v.type());
                rootItem->appendChild(child);
            }
            ++index;
        }
    } else {
        //rootItem->setValue(value.toVariant());
        //rootItem->setType(value.type());
    }

    return rootItem;
}

//=========================================================================


QJsonTree::QJsonTree(QObject *parent)
    : QAbstractItemModel(parent)
    , mRootItem{new QJsonTreeItem}
{
    mHeaders.append("Configuration");
}

QJsonTree::~QJsonTree()
{
    delete mRootItem;
}

bool QJsonTree::loadJson(const QJsonArray &json)
{
    QJsonDocument jdoc;
    jdoc.setArray(json);

    if (!jdoc.isNull()) {
        beginResetModel();
        delete mRootItem;
        mRootItem = QJsonTreeItem::load(QJsonValue(jdoc.array()));
        mRootItem->setType(QJsonValue::Array);
        endResetModel();
        return true;
    }

    qDebug()<<Q_FUNC_INFO<<"cannot load json";
    return false;
}


QVariant QJsonTree::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return QString("%1").arg(item->value());

        if (index.column() == 1)
            return item->value();
    } else if (Qt::EditRole == role) {
        if (index.column() == 1)
            return item->value();
    }

    return {};
}

bool QJsonTree::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col = index.column();
    if (Qt::EditRole == role) {
        if (col == 1) {
            QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());
            item->setValue(value.toString());
            emit dataChanged(index, index, {Qt::EditRole});
            return true;
        }
    }

    return false;
}

QVariant QJsonTree::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        return mHeaders.value(section);
    else
        return {};
}

QModelIndex QJsonTree::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    QJsonTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    QJsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return {};
}

QModelIndex QJsonTree::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    QJsonTreeItem *childItem = static_cast<QJsonTreeItem*>(index.internalPointer());
    QJsonTreeItem *parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonTree::rowCount(const QModelIndex &parent) const
{
    QJsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int QJsonTree::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

Qt::ItemFlags QJsonTree::flags(const QModelIndex &index) const
{
    int col   = index.column();
    auto item = static_cast<QJsonTreeItem*>(index.internalPointer());

    auto isArray = QJsonValue::Array == item->type();
    auto isObject = QJsonValue::Object == item->type();

    if ((col == 1) && !(isArray || isObject))
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    else
        return QAbstractItemModel::flags(index);
}
