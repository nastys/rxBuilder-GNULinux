/*
 * Copyright © 2015 nastys
 *
 * This file is part of rxBuilder.
 * rxBuilder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rxBuilder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rxBuilder.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Build_clicked()
{
    ui->Build->setEnabled(false);
    ui->devk->setEnabled(false);
    ui->Build->setText("Checking system...");

    QFile file("/usr/bin/git");
    if(file.exists())
    {
        file.setFileName(QDir::homePath()+"/devkitpro/devkitARM/bin/arm-none-eabi-g++");
        if(file.exists())
            ui->Build->setText("Detected.");
        else
        {
            ui->Build->setText("Installing devkitPro...");
            on_devk_clicked();
        }
        shell("mkdir -p \""+QDir::homePath()+"/rxTools-builder/\"");
        shell("cd \""+QDir::homePath()+"/rxTools-builder/\" && git clone --recursive https://github.com/roxas75/rxTools.git");
        shell("export DEVKITPRO=\""+QDir::homePath()+"/devkitpro/\" && export DEVKITARM=\""+QDir::homePath()+"/devkitpro/devkitARM/\" && export PATH=$PATH:\""+QDir::homePath()+"/devkitpro/devkitARM/bin\" && cd \""+QDir::homePath()+"/rxTools-builder/rxTools\" && make clean && make release ; sleep 5");
        QDesktopServices::openUrl(QUrl(QDir::homePath()+"/rxTools-builder/rxTools/release"));
        ui->Build->setText("Build complete.\n"+QDir::homePath()+"/rxTools-builder/rxTools/release");
    }
    else
    {
        ui->Build->setText("Please install git.");
        ui->Build->setEnabled(true);
        ui->devk->setEnabled(true);
        QDesktopServices::openUrl(QUrl("apt://git"));
    }
}

void MainWindow::on_devk_clicked()
{
    ui->Build->setEnabled(false);
    ui->devk->setEnabled(false);
    ui->Build->setText("Installing DevKitPro...");
    shell("mkdir -p \""+QDir::homePath()+"/devkitpro/\"");
    shell("wget http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/devkitARMupdate.pl/download -O \""+QDir::homePath()+"/devkitpro/devkitARMupdate.pl\"");
    shell("perl \""+QDir::homePath()+"/devkitpro/devkitARMupdate.pl\" \""+QDir::homePath()+"/devkitpro/\"");
    shell("cd \""+QDir::homePath()+"/devkitpro/\" && git clone https://github.com/smealum/ctrulib.git");
    shell("export DEVKITPRO=\""+QDir::homePath()+"/devkitpro/\" && export DEVKITARM=\""+QDir::homePath()+"/devkitpro/devkitARM/\" && cd \""+QDir::homePath()+"/devkitpro/ctrulib/libctru\" && make && make install ; sleep 5");
    ui->Build->setEnabled(true);
    ui->devk->setEnabled(true);
    ui->Build->setText("DevKitPro installed.");
}

void MainWindow::shell(QString command)
{
    QFile xterm("/usr/bin/xterm");
    QProcess shell;
    if(xterm.exists())
        shell.start("xterm", QStringList() << "-e" << command);
    else
        shell.start("bash", QStringList() << "-c" << command);
    shell.waitForFinished(-1);
}
