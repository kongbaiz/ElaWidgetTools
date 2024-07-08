﻿#include "ElaMenu.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "DeveloperComponents/ElaMenuStyle.h"
#include "ElaApplication.h"
#include "private/ElaMenuPrivate.h"
ElaMenu::ElaMenu(QWidget* parent)
    : QMenu(parent), d_ptr(new ElaMenuPrivate())
{
    Q_D(ElaMenu);
    d->q_ptr = this;
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("ElaMenu");
    d->_menuStyle = new ElaMenuStyle(style());
    setStyle(d->_menuStyle);
}

ElaMenu::ElaMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent), d_ptr(new ElaMenuPrivate())
{
    Q_D(ElaMenu);
    d->q_ptr = this;
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("ElaMenu");
    d->_menuStyle = new ElaMenuStyle(style());
    setStyle(d->_menuStyle);
}

ElaMenu::~ElaMenu()
{
}

void ElaMenu::setMenuItemHeight(int menuItemHeight)
{
    Q_D(ElaMenu);
    d->_menuStyle->setMenuItemHeight(menuItemHeight);
}

int ElaMenu::getMenuItemHeight() const
{
    Q_D(const ElaMenu);
    return d->_menuStyle->getMenuItemHeight();
}

QAction* ElaMenu::addMenu(QMenu* menu)
{
    return QMenu::addMenu(menu);
}

ElaMenu* ElaMenu::addMenu(const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    QMenu::addAction(menu->menuAction());
    return menu;
}

ElaMenu* ElaMenu::addMenu(const QIcon& icon, const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    menu->setIcon(icon);
    QMenu::addAction(menu->menuAction());
    return menu;
}

ElaMenu* ElaMenu::addMenu(ElaIconType icon, const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    QMenu::addAction(menu->menuAction());
    menu->menuAction()->setProperty("ElaIconType", QChar((unsigned short)icon));
    return menu;
}

QAction* ElaMenu::addElaIconAction(ElaIconType icon, const QString& text)
{
    QAction* action = new QAction(text, this);
    action->setProperty("ElaIconType", QChar((unsigned short)icon));
    QMenu::addAction(action);
    return action;
}

QAction* ElaMenu::addElaIconAction(ElaIconType icon, const QString& text, const QKeySequence& shortcut)
{
    QAction* action = new QAction(text, this);
    action->setShortcut(shortcut);
    action->setProperty("ElaIconType", QChar((unsigned short)icon));
    QMenu::addAction(action);
    return action;
}

bool ElaMenu::event(QEvent* event)
{
    Q_D(ElaMenu);
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);

        if (mouseEvent)
        {
            d->_mousePressPoint = mapToGlobal(mouseEvent->pos());
            if (ElaApplication::containsCursorToItem(this))
            {
                QAction* action = actionAt(mouseEvent->pos());
                if (action && !action->menu() && action->isEnabled())
                {
                    d->_isCloseAnimation = true;
                }
            }
        }
    }
    return QMenu::event(event);
}

void ElaMenu::showEvent(QShowEvent* event)
{
    QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "geometry");
    posAnimation->setEasingCurve(QEasingCurve::InOutSine);
    posAnimation->setDuration(250);
    posAnimation->setStartValue(QRect(QPoint(x() - 15, y()), QSize(1, 1)));
    posAnimation->setEndValue(geometry());
    posAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QMenu::showEvent(event);
}

void ElaMenu::closeEvent(QCloseEvent* event)
{
    Q_D(ElaMenu);
    if (d->_isCloseAnimation && windowOpacity() == 1)
    {
        d->_isCloseAnimation = false;
        event->ignore();
        QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "windowOpacity");
        connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
            close();
            setWindowOpacity(1);
        });
        opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
        opacityAnimation->setDuration(300);
        opacityAnimation->setStartValue(1);
        opacityAnimation->setEndValue(0);
        opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        QMenu::closeEvent(event);
    }
}
