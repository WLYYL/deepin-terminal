/*
 *  Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd
 *
 * Author:     wangliang <wangliang@uniontech.com>
 *
 * Maintainer: wangliang <wangliang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ut_termwidget_test.h"

#include "service.h"
#include "termwidget.h"
#include "termwidgetpage.h"
#include "qtermwidget.h"
#include "TerminalDisplay.h"
#include "stub.h"

//Qt单元测试相关头文件
#include <QTest>
#include <QtGui>
#include <QSignalSpy>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QHostInfo>

UT_TermWidget_Test::UT_TermWidget_Test()
{
    if (!Service::instance()->property("isServiceInit").toBool()) {
        Service::instance()->init();
        Service::instance()->setProperty("isServiceInit", true);
    }

    m_normalTermProperty[QuakeMode] = false;
    m_normalTermProperty[SingleFlag] = true;
    m_normalWindow = new NormalWindow(m_normalTermProperty, nullptr);
}

UT_TermWidget_Test::~UT_TermWidget_Test()
{
    delete m_normalWindow;
}

#ifdef UT_TERMWIDGET_TEST
TEST_F(UT_TermWidget_Test, TermWidgetTest)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;

    bool isInRemoteServer = termWidget->isInRemoteServer();
    EXPECT_EQ(isInRemoteServer, false);

    TermWidgetPage *termPage = termWidget->parentPage();
    EXPECT_NE(termPage, nullptr);
    EXPECT_EQ(QString(termPage->metaObject()->className()), QString("TermWidgetPage"));

    //这两个函数未实现
    termWidget->skipToPreCommand();
    termWidget->skipToNextCommand();

    termWidget->setEnterSzCommand(false);
    EXPECT_EQ(termWidget->enterSzCommand(), false);

    termWidget->setEnterSzCommand(true);
    EXPECT_EQ(termWidget->enterSzCommand(), true);

    //设置透明度
    for (qreal opacity = 0.01; opacity <= 1.0; opacity += 0.01) {
        termWidget->setTermOpacity(opacity);
    }

    QStringList fontFamilyList;
    fontFamilyList << "Courier 10 Pitch" << "DejaVu Sans Mono" << "Liberation Mono"
                   << "Noto Mono" << "Noto Sans Mono" << "Noto Sans Mono CJK JP"
                   << "Noto Sans Mono CJK KR" << "Noto Sans Mono CJK SC"
                   << "Noto Sans Mono CJK TC";
    for (int i = 0; i < fontFamilyList.size(); i++) {
        QString fontFamily = fontFamilyList.at(i);
        termWidget->setTermFont(fontFamily);
        QFont currFont = termWidget->getTerminalFont();
        EXPECT_EQ(currFont.family(), fontFamily);
    }

    QString lastFontFamily = fontFamilyList.last();
    QFont currTermFont = termWidget->getTerminalFont();
    EXPECT_EQ(currTermFont.family(), lastFontFamily);

    //字体大小大于20时界面提示符显示会有异常
    //设置字体大小时会不停刷日志：Using a variable-width font in the terminal.  This may cause performance degradation and display/alignment errors.
    for (int fontSize = 5; fontSize <= 50; fontSize++) {
        termWidget->setTermFontSize(fontSize);
        QFont currFont = termWidget->getTerminalFont();
        EXPECT_EQ(currFont.pointSize(), fontSize);
    }

}

TEST_F(UT_TermWidget_Test, getsetEncode)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;

    QList<QByteArray> encodeList;
    encodeList << "UTF-8" << "GB18030" << "GB2312" << "GBK" /*简体中文*/
               << "BIG5" << "BIG5-HKSCS" //<< "EUC-TW"      /*繁体中文*/
               << "EUC-JP"  << "SHIFT_JIS"  //<< "ISO-2022-JP"/*日语*/
               << "EUC-KR" //<< "ISO-2022-KR" //<< "UHC"      /*韩语*/
               << "IBM864" << "ISO-8859-6" << "ARABIC" << "WINDOWS-1256"   /*阿拉伯语*/
               << "ISO-8859-13" << "ISO-8859-4" << "WINDOWS-1257"  /*波罗的海各国语*/
               << "ISO-8859-14"    /*凯尔特语*/
               << "IBM-852" << "ISO-8859-2" << "x-mac-CE" << "WINDOWS-1250" /*中欧*/
               << "IBM855" << "ISO-8859-5"  << "KOI8-R" << "MAC-CYRILLIC" << "WINDOWS-1251" //<< "ISO-IR-111" /*西里尔语*/
               << "CP866" /*西里尔语或俄语*/
               << "KOI8-U" << "x-MacUkraine" /*西里尔语或乌克兰语*/
               << "ISO-8859-7" << "x-mac-GREEK" << "WINDOWS-1253"  /*希腊语*/
               << "IBM862" << "ISO-8859-8-I" << "WINDOWS-1255"//<< "x-mac-HEBREW"  /*希伯来语*/
               << "ISO-8859-8" /*希伯来语*/
               << "ISO-8859-10"     /*北欧语*/
               << "ISO-8859-3"      /*西欧语*/
               << "TIS-620"         /*泰语*/
               << "IBM857" << "ISO-8859-9" << "x-mac-TURKISH" << "WINDOWS-1254" /*土耳其语*/
               << "WINDOWS-1258" //<< "TCVN" << "VISCII"  /*越南语*/
               << "IBM850" << "ISO-8859-1" << "ISO-8859-15" << "x-ROMAN8" << "WINDOWS-1252"; /*西方国家*/

    for (int i = 0; i < encodeList.size(); i++) {
        QString encode = QString(encodeList.at(i));
        termWidget->setEncode(encode);
        EXPECT_EQ(termWidget->encode(), encode);
        qDebug() << termWidget->encode();

        termWidget->setRemoteEncode(encode);
        EXPECT_EQ(termWidget->RemoteEncode(), encode);
        qDebug() << termWidget->RemoteEncode();
    }
}


//用于customContextMenuCall测试打桩
void stub_showPlugin(const QString &name)
{
    Q_UNUSED(name)
}

//用于customContextMenuCall测试打桩
void stub_addMenuActions(const QPoint &pos)
{
    Q_UNUSED(pos)
}

//用于customContextMenuCall测试打桩
QAction *stub_exec(const QPoint &pos, QAction *at = nullptr)
{
    Q_UNUSED(pos)
    Q_UNUSED(at)

    return nullptr;
}

TEST_F(UT_TermWidget_Test, customContextMenuCall)
{
    Stub s;
    s.set(ADDR(MainWindow, showPlugin), stub_showPlugin);
    s.set(ADDR(TermWidget, addMenuActions), stub_addMenuActions);
    s.set((QAction *(QMenu::*)(const QPoint &, QAction *))ADDR(QMenu, exec), stub_exec);

    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    QPoint pos(50, 50);
    termWidget->customContextMenuCall(pos);

    s.reset(ADDR(MainWindow, showPlugin));
    s.reset(ADDR(TermWidget, addMenuActions));
    s.reset((QAction *(QMenu::*)(const QPoint &, QAction *))ADDR(QMenu, exec));
}

TEST_F(UT_TermWidget_Test, addMenuActions)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    termWidget->m_menu = new DMenu(termWidget);

    QPoint pos(50, 50);
    termWidget->addMenuActions(pos);
}

TEST_F(UT_TermWidget_Test, onSettingValueChanged)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    termWidget->onSettingValueChanged("basic.interface.opacity");
    termWidget->onSettingValueChanged("basic.interface.font");
    termWidget->onSettingValueChanged("basic.interface.font_size");
    termWidget->onSettingValueChanged("advanced.cursor.cursor_shape");
    termWidget->onSettingValueChanged("advanced.cursor.cursor_blink");
    termWidget->onSettingValueChanged("advanced.scroll.scroll_on_key");
    termWidget->onSettingValueChanged("advanced.cursor.auto_copy_selection");
}

TEST_F(UT_TermWidget_Test, search)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    termWidget->search("~", true, false);
    termWidget->search("~", false, true);
}

TEST_F(UT_TermWidget_Test, onTouchPadSignal)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    termWidget->onTouchPadSignal("pinch", "in", 2);
    termWidget->onTouchPadSignal("pinch", "out", 2);
}

TEST_F(UT_TermWidget_Test, showFlowMessage)
{
    m_normalWindow->resize(800, 600);
    m_normalWindow->show();
    EXPECT_EQ(m_normalWindow->isVisible(), true);

    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    EXPECT_EQ(currTermPage->isVisible(), true);

    TermWidget *termWidget = currTermPage->m_currentTerm;
    termWidget->showFlowMessage(true);
    EXPECT_EQ(termWidget->m_flowMessage->isVisible(), true);
}

/*******************************************************************************
 1. @函数:    setRemoteTabFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证是否能正确设置term中远程标签标题格式的值
*******************************************************************************/
TEST_F(UT_TermWidget_Test, setRemoteTabFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *termWidget = currTermPage->m_currentTerm;
    QString remoteFormat = "%U";
    termWidget->setRemoteTabFormat(remoteFormat);
    // 验证远程标签标签标题是否正确
    EXPECT_EQ(termWidget->m_tabFormat.remoteTabFormat, remoteFormat);
    EXPECT_EQ(termWidget->getRemoteTabTitleFormat(), remoteFormat);
}

/*******************************************************************************
 1. @函数:    setTabFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证是否能正确设置term中标签标题格式的值
*******************************************************************************/
TEST_F(UT_TermWidget_Test, setTabFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    QString tabFormat = "%w";
    term->setTabFormat(tabFormat);
    // 验证标签标签标题是否正确
    EXPECT_EQ(term->m_tabFormat.currentTabFormat, tabFormat);
    EXPECT_EQ(term->getTabTitleFormat(), tabFormat);
    // 若没有连接远程
    if (!term->isConnectRemote()) {
        EXPECT_EQ(term->getCurrentTabTitleFormat(), tabFormat);
    }
}

/*******************************************************************************
 1. @函数:    renameTabFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试重命名标签标题格式是否正确
*******************************************************************************/
TEST_F(UT_TermWidget_Test, renameTabFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    QString tabFormat = "%D";
    QString remoteFormat = "%h";
    term->renameTabFormat(tabFormat, remoteFormat);
    // 验证标签标签标题是否正确
    EXPECT_EQ(term->m_tabFormat.currentTabFormat, tabFormat);
    EXPECT_EQ(term->getTabTitleFormat(), tabFormat);
    // 若没有连接远程
    if (!term->isConnectRemote()) {
        EXPECT_EQ(term->getCurrentTabTitleFormat(), tabFormat);
    }
    // 验证远程标签标题是否正确
    EXPECT_EQ(term->m_tabFormat.remoteTabFormat, remoteFormat);
    EXPECT_EQ(term->getRemoteTabTitleFormat(), remoteFormat);
    EXPECT_EQ(term->m_tabFormat.isGlobal, false);
}

/*******************************************************************************
 1. @函数:    getTabTitle
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证获取标签标题是否正确
*******************************************************************************/
TEST_F(UT_TermWidget_Test, getTabTitle)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 有内容
    QString tabFormat = "ut_test";
    term->setTabFormat(tabFormat);
    // 验证的获取的标签标题是否是给定值
    EXPECT_EQ(term->getTabTitle(), tabFormat);
    // 空标签
    tabFormat = "";
    term->setTabFormat(tabFormat);
    EXPECT_EQ(term->getTabTitle(), "Terminal");

    // 设置远程标题
    QString tabTitle = "remote_current_title";
    term->m_isConnectRemote = true;
    term->setRemoteTabFormat(tabTitle);
    EXPECT_EQ(term->getTabTitle(), tabTitle);
    // 设置非远程标题
    tabTitle = "current_title";
    term->m_isConnectRemote = false;
    term->setTabFormat(tabTitle);
    EXPECT_EQ(term->getTabTitle(), tabTitle);
}

/*******************************************************************************
 1. @函数:    initTabTitleArgs
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证初始化标签标题参数列表是否正确
*******************************************************************************/
TEST_F(UT_TermWidget_Test, initTabTitleArgs)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    term->initTabTitleArgs();
    // 验证初始化标签标题参数列表是否正确
    // 标签标题
    QStringList strTabArgs = TAB_ARGS.split(" ");
    EXPECT_EQ(strTabArgs.count(), term->m_tabArgs.count());
    // 填充标签标题参数
    for (QString arg : strTabArgs) {
        EXPECT_EQ(term->m_tabArgs.contains(arg), true);
    }
    // 远程标签标题
    // 参数数量是否一致
    QStringList strRemoteTabArgs = REMOTE_ARGS.split(" ");
    EXPECT_EQ(strRemoteTabArgs.count(), term->m_remoteTabArgs.count());
    for (QString arg : strRemoteTabArgs) {
        EXPECT_EQ(term->m_remoteTabArgs.contains(arg), true);
    }

}

/*******************************************************************************
 1. @函数:    getCurrentTabTitleFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    获取的当前标签标题格式
*******************************************************************************/
TEST_F(UT_TermWidget_Test, getCurrentTabTitleFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置远程标题
    QString tabTitle = "remote_current_title";
    term->m_isConnectRemote = true;
    term->setRemoteTabFormat(tabTitle);
    EXPECT_EQ(term->getCurrentTabTitleFormat(), tabTitle);
    // 设置非远程标题
    tabTitle = "current_title";
    term->m_isConnectRemote = false;
    term->setTabFormat(tabTitle);
    EXPECT_EQ(term->getCurrentTabTitleFormat(), tabTitle);
}

/*******************************************************************************
 1. @函数:    getTabTitleFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证获取的标签标题格式
*******************************************************************************/
TEST_F(UT_TermWidget_Test, getTabTitleFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置标签标题
    QString tabTitle = "tab_title";
    term->setTabFormat(tabTitle);
    EXPECT_EQ(term->getTabTitleFormat(), tabTitle);
}

/*******************************************************************************
 1. @函数:    getRemoteTabTitleFormat
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证获取远程标签标题格式
*******************************************************************************/
TEST_F(UT_TermWidget_Test, getRemoteTabTitleFormat)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置远程标签标题
    QString remoteTabTitle = "remote_tab_title";
    term->setRemoteTabFormat(remoteTabTitle);
    EXPECT_EQ(term->getRemoteTabTitleFormat(), remoteTabTitle);
}

/*******************************************************************************
 1. @函数:    showShellMessage
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证shell消息报错弹窗是否正常显示
*******************************************************************************/
TEST_F(UT_TermWidget_Test, showShellMessage)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置远程标签标题
    QString shellWarning = "shell message";
    term->showShellMessage(shellWarning);
    // 获取DFloatingMessage
    QList<DFloatingMessage *> list = term->findChildren<DFloatingMessage *>();
    QString strShellMessage;
    // 获取label的提示信息
    for (DFloatingMessage *msg : list) {
        QList<QLabel *> lst = msg->findChildren<QLabel *>();
        for (QLabel *label : lst) {
            strShellMessage = label->text();
        }
    }
    EXPECT_EQ(strShellMessage, shellWarning);
}

/*******************************************************************************
 1. @函数:    setEncode
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试setEncode接口是否正常使用
*******************************************************************************/
TEST_F(UT_TermWidget_Test, setEncode)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置编码
    QString encode = "GBK";
    term->setEncode(encode);
    EXPECT_EQ(term->encode(), encode);
    EXPECT_EQ(term->m_encode, encode);
    // 切换编码
    encode = "UTF-8";
    term->setEncode(encode);
    EXPECT_EQ(term->encode(), encode);
    EXPECT_EQ(term->m_encode, encode);
}

/*******************************************************************************
 1. @函数:    encode
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试获取encode接口是否获取争取的值
*******************************************************************************/
TEST_F(UT_TermWidget_Test, encode)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 测试函数修改是否获取正确的值
    QString encode = "GBK";
    term->setEncode(encode);
    EXPECT_EQ(term->encode(), encode);
    // 测试直接修改变量是否获取正确的值
    encode = "UTF-8";
    term->m_encode = encode;
    EXPECT_EQ(term->encode(), encode);
}

/*******************************************************************************
 1. @函数:    setEncode
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试setRemoteEncode接口是否正常使用
*******************************************************************************/
TEST_F(UT_TermWidget_Test, setRemoteEncode)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置远程编码
    QString remoteEncode = "GBK";
    term->setRemoteEncode(remoteEncode);
    EXPECT_EQ(term->RemoteEncode(), remoteEncode);
    EXPECT_EQ(term->m_remoteEncode, remoteEncode);
    // 切换编码
    remoteEncode = "UTF-8";
    term->setRemoteEncode(remoteEncode);
    EXPECT_EQ(term->RemoteEncode(), remoteEncode);
    EXPECT_EQ(term->m_remoteEncode, remoteEncode);
}

/*******************************************************************************
 1. @函数:    RemoteEncode
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试获取远程encode接口是否获取争取的值
*******************************************************************************/
TEST_F(UT_TermWidget_Test, RemoteEncode)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 测试函数修改是否获取正确的值
    QString remoteEncode = "GBK";
    term->setRemoteEncode(remoteEncode);
    EXPECT_EQ(term->RemoteEncode(), remoteEncode);
    // 测试直接修改变量是否获取正确的值
    remoteEncode = "UTF-8";
    term->m_remoteEncode = remoteEncode;
    EXPECT_EQ(term->RemoteEncode(), remoteEncode);
}

/*******************************************************************************
 1. @函数:    modifyRemoteTabTitle
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证修改远程标签标题参数
*******************************************************************************/
TEST_F(UT_TermWidget_Test, modifyRemoteTabTitle)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 初始化测试数据
    ServerConfig serverConfig;
    serverConfig.m_address = "127.0.0.1";
    serverConfig.m_userName = "uos";
    serverConfig.m_serverName = "ut_test";
    // 参数传入
    term->modifyRemoteTabTitle(serverConfig);
    // 验证是否正确
    // 远程主机名
    EXPECT_EQ(term->m_remoteTabArgs[REMOTE_HOST_NAME], serverConfig.m_address);
    // 用户名 %u
    EXPECT_EQ(term->m_remoteTabArgs[USER_NAME], serverConfig.m_userName);
    // 用户名@ %U
    EXPECT_EQ(term->m_remoteTabArgs[USER_NAME_L], serverConfig.m_userName + QString("@"));
}

/*******************************************************************************
 1. @函数:    setIsConnectRemote
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    测试接口是否能够正确设置变量值
*******************************************************************************/
TEST_F(UT_TermWidget_Test, setIsConnectRemote)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置远程变量状态
    term->setIsConnectRemote(true);
    // 验证是否正确
    EXPECT_EQ(term->isConnectRemote(), true);
    EXPECT_EQ(term->m_isConnectRemote, true);
    // 设置远程变量状态
    term->setIsConnectRemote(false);
    // 验证是否正确
    EXPECT_EQ(term->isConnectRemote(), false);
    EXPECT_EQ(term->m_isConnectRemote, false);
}

/*******************************************************************************
 1. @函数:    isConnectRemote
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    是否连接远程
*******************************************************************************/
TEST_F(UT_TermWidget_Test, isConnectRemote)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 通过函数设置远程变量状态
    term->setIsConnectRemote(true);
    // 验证是否正确
    EXPECT_EQ(term->isConnectRemote(), true);
    // 设置远程变量状态
    term->m_isConnectRemote = false;
    // 验证是否正确
    EXPECT_EQ(term->isConnectRemote(), false);
}

/*******************************************************************************
 1. @函数:    selectEncode
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-03
 4. @说明:    验证在连接远程和未连接远程状态下，修改编码是否正确
*******************************************************************************/
TEST_F(UT_TermWidget_Test, selectEncode)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 设置连接远程
    term->setIsConnectRemote(true);
    // 编码变量
    QString encode = "GBK";
    // 设置编码
    term->selectEncode(encode);
    // 此时远程编码应该和设置的编码一致
    EXPECT_EQ(term->RemoteEncode(), encode);
    // 设置断开远程
    term->setIsConnectRemote(false);
    // 改变编码
    encode = "BIG5";
    term->selectEncode(encode);
    // 此时当前编码和设置的编码一致
    EXPECT_EQ(term->encode(), encode);
}

/*******************************************************************************
 1. @函数:    onTitleArgsChange
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-14
 4. @说明:    测试标签标题变化
*******************************************************************************/
TEST_F(UT_TermWidget_Test, onTitleArgsChange)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    // %D %d
    // 第一种情况
    term->onTitleArgsChange(DIR_L, "~");
    QString longDir = term->m_tabArgs[DIR_L];
    QString shortDir = term->m_tabArgs[DIR_S];
    EXPECT_EQ(longDir, "~");
    EXPECT_EQ(shortDir, QDir::homePath().split("/").last());
    // 第二种情况
    term->onTitleArgsChange(DIR_L, QDir::homePath());
    longDir = term->m_tabArgs[DIR_L];
    shortDir = term->m_tabArgs[DIR_S];
    EXPECT_EQ(longDir, "~");
    EXPECT_EQ(shortDir, QDir::homePath().split("/").last());
    // 第三种情况
    term->onTitleArgsChange(DIR_L, "/");
    longDir = term->m_tabArgs[DIR_L];
    shortDir = term->m_tabArgs[DIR_S];
    EXPECT_EQ(longDir, QString("/"));
    EXPECT_EQ(shortDir, QString("/"));
    // 一般情况
    term->onTitleArgsChange(DIR_L, "/home/777/aaa");
    longDir = term->m_tabArgs[DIR_L];
    shortDir = term->m_tabArgs[DIR_S];
    EXPECT_EQ(longDir, QString("/home/777/aaa"));
    EXPECT_EQ(shortDir, QString("aaa"));

    // %h
    term->onTitleArgsChange(LOCAL_HOST_NAME, "dzw");
    QString localHostName = term->m_tabArgs[LOCAL_HOST_NAME];
    EXPECT_EQ(localHostName, "dzw");
}

/*******************************************************************************
 1. @函数:    onHostnameChanged
 2. @作者:    ut000610 戴正文
 3. @日期:    2020-12-14
 4. @说明:    更新当前主机名
*******************************************************************************/
TEST_F(UT_TermWidget_Test, onHostnameChanged)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;
    // 获取当前主机名
    QString hostName = QHostInfo::localHostName();
    term->onHostnameChanged();
    QString result = term->m_tabArgs[LOCAL_HOST_NAME];
    EXPECT_EQ(hostName, result);
}

TEST_F(UT_TermWidget_Test, onHorizontalSplit)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onHorizontalSplit();
}

TEST_F(UT_TermWidget_Test, onVerticalSplit)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onVerticalSplit();
}

TEST_F(UT_TermWidget_Test, splitHorizontal)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->splitHorizontal();
}

TEST_F(UT_TermWidget_Test, splitVertical)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->splitVertical();
}

TEST_F(UT_TermWidget_Test, onCloseCurrWorkSpace)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onCloseCurrWorkSpace();
}

TEST_F(UT_TermWidget_Test, onCloseOtherWorkSpaces)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onCloseOtherWorkSpaces();
}

TEST_F(UT_TermWidget_Test, onCreateNewTab)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onCreateNewTab();
}

TEST_F(UT_TermWidget_Test, onSwitchFullScreen)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onSwitchFullScreen();
}

TEST_F(UT_TermWidget_Test, openBing)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->openBing();
}

TEST_F(UT_TermWidget_Test, openBaidu)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->openBaidu();
}

TEST_F(UT_TermWidget_Test, openGithub)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->openGithub();
}

TEST_F(UT_TermWidget_Test, openStackOverflow)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->openStackOverflow();
}


TEST_F(UT_TermWidget_Test, onShowSearchBar)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onShowSearchBar();
}

TEST_F(UT_TermWidget_Test, onShowEncoding)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onShowEncoding();
}

TEST_F(UT_TermWidget_Test, onShowCustomCommands)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onShowCustomCommands();
}

TEST_F(UT_TermWidget_Test, onShowRemoteManagement)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onShowRemoteManagement();
}


TEST_F(UT_TermWidget_Test, onUploadFile)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onUploadFile();
}

TEST_F(UT_TermWidget_Test, onDownloadFile)
{
    // 获取当前term
    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
    TermWidget *term = currTermPage->m_currentTerm;

    term->onDownloadFile();
}

// 测试该函数会crash，待查找原因
//TEST_F(UT_TermWidget_Test, onShowSettings)
//{
//    // 获取当前term
//    TermWidgetPage *currTermPage = m_normalWindow->currentPage();
//    TermWidget *term = currTermPage->m_currentTerm;

//    term->onShowSettings();
//}

#endif
