#include "mainwindow.h"

#define OFFSET 23
#define FILTER "Python Source File (*.py)"

MainWindow::MainWindow()
{
    setupHierarchy();
    setupInterpreter();
    setupSignaling();
    setupMenu();
    setCanonicalPath("");
}

MainWindow::MainWindow(const QString &path)
{
    setupHierarchy();
    setupInterpreter();
    setupSignaling();
    setupMenu();
    loadFile(path);
}

MainWindow::~MainWindow()
{
    delete result;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (eventuallySave()) {
        saveSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::newDocument()
{
    MainWindow *w = new MainWindow();
    w->move(x() + OFFSET, y() + OFFSET);
    w->show();
}

void MainWindow::openDocument()
{
    QString path = QFileDialog::getOpenFileName(this, "", canonicalDir, FILTER);
    if (!path.isEmpty()) {
        MainWindow *w = findMainWindow(path);
        if (w) {
            w->show();
            w->raise();
            w->activateWindow();
            return;
        }
        if (canonicalPath.isEmpty() && !editor->document()->isModified()) {
            loadFile(path);
            return;
        }
        w = new MainWindow(path);
        if (w->canonicalPath.isEmpty()) {
            delete w;
            return;
        }
        w->move(x() + OFFSET, y() + OFFSET);
        w->show();
    }
}

bool MainWindow::saveDocument()
{
    if (canonicalPath.isEmpty()) {
        return saveDocumentAs();
    } else {
        return saveFile(canonicalPath);
    }
}

bool MainWindow::saveDocumentAs()
{
    QString path;
    QFileDialog dialog(this, "", canonicalDir, FILTER);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() == QDialog::Accepted) {
        path = dialog.selectedFiles().value(0);
    }
    if (path.isEmpty()) {
        return false;
    }
    return saveFile(path);
}

bool MainWindow::exportResult()
{
    QString path;
    QString filter("");
    if (result->startsWith("\x89PNG\r\n\x1a\n")) {
        filter.append("Portable Network Graphics (*.png)");
    } else if (result->startsWith("\xff\xd8")) {
        filter.append("JPEG Image (*.jpg)");
    } else if (result->startsWith("%PDF")) {
        filter.append("Portable Document Format (*.pdf)");
    }
    QFileDialog dialog(this, "", canonicalDir, filter);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() == QDialog::Accepted) {
        path = dialog.selectedFiles().value(0);
    }
    if (path.isEmpty()) {
        return false;
    }
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        QString text = QString("Cannot save file %1:\n%2.").arg(path).arg(file.errorString());
        QMessageBox::warning(this, "flat", text);
        return false;
    }
    QDataStream out(&file);
    out.writeRawData(result->data(), result->length());
    return true;
}

void MainWindow::focusChanged(QWidget *before, QWidget *now)
{
    if (qobject_cast<PythonEdit *>(before) || qobject_cast<LineEdit *>(before)) {
        disconnect(undoAction, SIGNAL(triggered()), before, SLOT(undo()));
        disconnect(redoAction, SIGNAL(triggered()), before, SLOT(redo()));
        disconnect(cutAction, SIGNAL(triggered()), before, SLOT(cut()));
        disconnect(copyAction, SIGNAL(triggered()), before, SLOT(copy()));
        disconnect(pasteAction, SIGNAL(triggered()), before, SLOT(paste()));
        disconnect(selectAllAction, SIGNAL(triggered()), before, SLOT(selectAll()));

        disconnect(before, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
        disconnect(before, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
        disconnect(before, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
        disconnect(before, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));
    }
    PythonEdit *pythonEdit = qobject_cast<PythonEdit *>(now);
    LineEdit *lineEdit = qobject_cast<LineEdit *>(now);
    if (pythonEdit || lineEdit) {
        connect(undoAction, SIGNAL(triggered()), now, SLOT(undo()));
        connect(redoAction, SIGNAL(triggered()), now, SLOT(redo()));
        connect(cutAction, SIGNAL(triggered()), now, SLOT(cut()));
        connect(copyAction, SIGNAL(triggered()), now, SLOT(copy()));
        connect(pasteAction, SIGNAL(triggered()), now, SLOT(paste()));
        connect(selectAllAction, SIGNAL(triggered()), now, SLOT(selectAll()));

        connect(now, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
        connect(now, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
        connect(now, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
        connect(now, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));

        if (pythonEdit) {
            undoAction->setEnabled(pythonEdit->document()->isUndoAvailable());
            redoAction->setEnabled(pythonEdit->document()->isRedoAvailable());
            cutAction->setEnabled(pythonEdit->textCursor().hasSelection());
            copyAction->setEnabled(pythonEdit->textCursor().hasSelection());
        } else {
            undoAction->setEnabled(lineEdit->isUndoAvailable());
            redoAction->setEnabled(lineEdit->isRedoAvailable());
            cutAction->setEnabled(lineEdit->hasSelectedText());
            copyAction->setEnabled(lineEdit->hasSelectedText());
        }
    }
}

void MainWindow::toggleFinder()
{
    if (finder->isVisible()) {
        finder->hide();
    } else {
        if (goer->isVisible()) goer->hide();
        finder->show();
    }
}

void MainWindow::find(const QString &needle, QTextDocument::FindFlags options)
{
    previousNeedle = needle;
    if (!editor->find(needle, options)) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition((options & QTextDocument::FindBackward) == 0 ? QTextCursor::Start : QTextCursor::End);
        cursor = editor->document()->find(needle, cursor, options);
        if (cursor.isNull()) {
            message->setText(QString("\"%1\" not found.").arg(needle));
        } else {
            editor->setTextCursor(cursor);
            message->setText("Wrapped around.");
        }
    }
}

void MainWindow::replace(const QString &needle, const QString &replacement)
{
    int count = 0;
    QTextDocument *document = editor->document();
    QTextCursor cursor, c;
    cursor = c = QTextCursor(document);
    cursor.beginEditBlock();
    while (true) {
        c = document->find(needle, c, QTextDocument::FindCaseSensitively);
        if (c.isNull()) break;
        c.insertText(replacement);
        count++;
    }
    cursor.endEditBlock();
    message->setText(QString(count == 1 ? "%1 replacement." : "%1 replacements.").arg(count));
}

void MainWindow::findNext()
{
    if (!previousNeedle.isEmpty()) find(previousNeedle);
}

void MainWindow::findPrevious()
{
    if (!previousNeedle.isEmpty()) find(previousNeedle, QTextDocument::FindBackward);
}

void MainWindow::toggleGoer()
{
    if (goer->isVisible()) {
        goer->hide();
    } else {
        if (finder->isVisible()) finder->hide();
        goer->show();
    }
}

void MainWindow::go(int line)
{
    line = qMin(line, editor->document()->blockCount());
    QTextBlock block = editor->document()->findBlockByNumber(line - 1);
    QTextCursor cursor = editor->textCursor();
    int column = qMin(cursor.columnNumber(), block.length() - 1);
    cursor.setPosition(block.position() + column);
    editor->setTextCursor(cursor);
    editor->ensureCursorVisible();
}

void MainWindow::run()
{
    exportAction->setEnabled(false);
    runAction->setEnabled(false);
    stopAction->setEnabled(true);
    activity->startAnimating();
    console->clear();
    server->listen(QHostAddress::LocalHost);
#ifdef Q_OS_MAC
    QString thirdparty = qApp->applicationDirPath() + "/../Resources";
#else
    QString thirdparty = qApp->applicationDirPath() + "/thirdparty";
#endif
    QProcessEnvironment environment;
    environment.insert("PYTHONPATH", thirdparty + "/lib");
    environment.insert("EVEN_SERVER_PORT", QString::number(server->serverPort()));
    QStringList arguments;
    arguments << "-u";
    arguments << "-c" << editor->toPlainText();
    process->setProcessEnvironment(environment);
    process->setWorkingDirectory(canonicalDir);
#ifdef Q_OS_WIN
    process->start(thirdparty + "/pypy/pypy", arguments);
#else
    process->start(thirdparty + "/pypy/bin/pypy", arguments);
#endif
    if (!process->waitForStarted()) {
        done();
        QMessageBox::warning(this, "Even", "Python interpreter not found.");
    }
}

void MainWindow::log()
{
    console->insertPlainText(process->readAll());
    console->moveCursor(QTextCursor::End);
}

void MainWindow::accept()
{
    if (socketConnected) return;
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), this, SLOT(discard()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(read()));
    socketConnected = true;
    payloadSize = 0;
    result->clear();
    QTimer::singleShot(0, this, SLOT(read()));
}

void MainWindow::discard()
{
    socket->deleteLater();
    socketConnected = false;
    if (server->hasPendingConnections()) QTimer::singleShot(0, this, SLOT(accept()));
}

void MainWindow::read()
{
    quint32 available = socket->bytesAvailable();
    if (available == 0) return;
    if (payloadSize == 0) {
        if (available < sizeof(quint32)) return;
        QDataStream(socket) >> payloadSize;
        available -= 4;
    }
    quint32 done = result->length();
    quint32 total = qMin(done + available, payloadSize);
    result->append(socket->read(total - done));
    if (total >= payloadSize) {
        socket->write("1", 1);
        viewer->load(*result);
    }
}

void MainWindow::stop()
{
    process->terminate();
}

void MainWindow::done()
{
    server->close();
    if (socketConnected) socket->close();
    exportAction->setEnabled(!result->isEmpty());
    runAction->setEnabled(true);
    stopAction->setEnabled(false);
    activity->stopAnimating(process->exitStatus() != QProcess::NormalExit);
}

void MainWindow::splitterMoved(QMoveEvent *e)
{
    int x = e->pos().x();
    activity->move(x+10, activity->y());
    coordinates->move(x+30, coordinates->y());
    message->move(x+190, message->y());
}

void MainWindow::zoomChanged(qreal scale)
{
    QString text = QString::number(scale * 100.0, 'f', 1);
    if (text.endsWith('0')) text.chop(1);
    if (text.endsWith('.')) text.chop(1);
    text.append("%");
    zoom->setText(text);
}

void MainWindow::positionChanged()
{
    QTextCursor cursor = editor->textCursor();
    int line = cursor.block().blockNumber() + 1;
    int column = cursor.columnNumber() + 1;
    coordinates->setText(QString("Line: %1  Column: %2").arg(line).arg(column));
}

void MainWindow::setupHierarchy()
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    hSplitter = new QSplitter(this);
    hSplitter->setHandleWidth(1);
    hSplitter->setChildrenCollapsible(false);

    setCentralWidget(hSplitter);

    viewer = new ViewerWidget(hSplitter);

    vSplitter = new MovingSplitter(hSplitter);
    vSplitter->setHandleWidth(1);
    vSplitter->setChildrenCollapsible(false);
    vSplitter->setOrientation(Qt::Vertical);

    QFont body("Menlo");
    body.insertSubstitutions("Menlo", QStringList() << "Ubuntu Mono" << "Consolas" << "Source Code Pro" << "Inconsolata");
    body.setStyleHint(QFont::Monospace);
#ifdef Q_OS_LINUX
    body.setPixelSize(15);
#else
    body.setPixelSize(13);
#endif

    editor = new PythonEdit();
    editor->setFrameShape(QFrame::NoFrame);
    editor->setFont(body);
    editor->setLineWrapMode(QTextEdit::NoWrap);
    editor->setAcceptRichText(false);

    finder = new FindWidget();
    finder->setHidden(true);

    goer = new GoWidget();
    goer->setHidden(true);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(editor);
    vbox->addWidget(finder);
    vbox->addWidget(goer);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(1);
    QWidget *container = new QWidget(vSplitter);
    container->setLayout(vbox);

    console = new BaseEdit(vSplitter);
    console->setFrameShape(QFrame::NoFrame);
    console->setFont(body);
    console->setLineWrapMode(QTextEdit::NoWrap);
    console->setAcceptRichText(false);
    console->setFocusPolicy(Qt::ClickFocus);
    console->setTabChangesFocus(true);

    QStatusBar *bar = new QStatusBar(this);
    bar->setSizeGripEnabled(false);
    this->setStatusBar(bar);

    QFont caption = qApp->font();
    caption.setPixelSize(11);

    zoom = new QLabel(bar);
    zoom->setGeometry(30, 0, 40, 22);
    zoom->setFont(caption);
    zoomChanged(1.0);

    activity = new ActivityWidget(bar);
    activity->setGeometry(0, 6, 12, 12);
    activity->hide();

    coordinates = new QLabel(bar);
    coordinates->setGeometry(0, 0, 160, 22);
    coordinates->setFont(caption);
    positionChanged();

    message = new HidingLabel(bar);
    message->setGeometry(0, 0, 320, 22);
    message->setFont(caption);

    vSplitter->setSizes(QList<int>() << 600 << 100);
    hSplitter->setSizes(QList<int>() << 100 << 100);

    this->resize(960, 540);
}

void MainWindow::setupInterpreter()
{
    server = new QTcpServer(this);
    socketConnected = false;
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    result = new QByteArray();
}

void MainWindow::setupSignaling()
{
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(vSplitter, SIGNAL(moved(QMoveEvent*)), this, SLOT(splitterMoved(QMoveEvent*)));
    connect(viewer, SIGNAL(zoomSignal(qreal)), this, SLOT(zoomChanged(qreal)));
    connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(positionChanged()));
    connect(editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));
    connect(finder, SIGNAL(findSignal(QString)), this, SLOT(find(QString)));
    connect(finder, SIGNAL(replaceSignal(QString,QString)), this, SLOT(replace(QString,QString)));
    connect(goer, SIGNAL(goSignal(int)), this, SLOT(go(int)));
    connect(server, SIGNAL(newConnection()), this, SLOT(accept()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(log()));
    connect(process, SIGNAL(finished(int)), this, SLOT(done()));
}

void MainWindow::setupMenu()
{
    QMenu *m;

    m = menuBar()->addMenu("File");
    m->addAction("New", this, SLOT(newDocument()), QKeySequence::New);
    m->addAction("Open...", this, SLOT(openDocument()), QKeySequence::Open);
    m->addSeparator();
    m->addAction("Close", this, SLOT(close()), QKeySequence::Close);
    m->addAction("Save", this, SLOT(saveDocument()), QKeySequence::Save);
    m->addAction("Save As...", this, SLOT(saveDocumentAs()), QKeySequence::SaveAs);
    m->addSeparator();
    exportAction = m->addAction("Export...", this, SLOT(exportResult()), QKeySequence(Qt::CTRL + Qt::Key_E));
    exportAction->setEnabled(false);
#ifndef Q_OS_MAC
    m->addSeparator();
    m->addAction("Exit", this, SLOT(close()), QKeySequence::Quit);
#endif

    m = menuBar()->addMenu("Edit");
    undoAction = m->addAction("Undo");
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction = m->addAction("Redo");
    redoAction->setShortcut(QKeySequence::Redo);
    m->addSeparator();
    cutAction = m->addAction("Cut");
    cutAction->setShortcut(QKeySequence::Cut);
    copyAction = m->addAction("Copy");
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction = m->addAction("Paste");
    pasteAction->setShortcut(QKeySequence::Paste);
    selectAllAction = m->addAction("Select All");
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    m->addSeparator();
    m->addAction("Find/Replace All...", this, SLOT(toggleFinder()), QKeySequence::Find);
    m->addAction("Find Next", this, SLOT(findNext()), QKeySequence::FindNext);
    m->addAction("Find Previous", this, SLOT(findPrevious()), QKeySequence::FindPrevious);
    m->addSeparator();
    m->addAction("Go to Line...", this, SLOT(toggleGoer()), QKeySequence(Qt::CTRL + Qt::Key_L));
    m->addSeparator();
    m->addAction("Dedent", editor, SLOT(dedent()), QKeySequence(Qt::CTRL + Qt::Key_BracketLeft));
    m->addAction("Indent", editor, SLOT(indent()), QKeySequence(Qt::CTRL + Qt::Key_BracketRight));
    m->addAction("Comment", editor, SLOT(comment()), QKeySequence(Qt::CTRL + Qt::Key_Slash));
    m->addSeparator();
    m->addAction("Duplicate", editor, SLOT(duplicate()), QKeySequence(Qt::CTRL + Qt::Key_D));

    m = menuBar()->addMenu("View");
    m->addAction("Actual Size", viewer, SLOT(zoomReset()), QKeySequence(Qt::CTRL + Qt::Key_0));
    m->addAction("Fit Width", viewer, SLOT(zoomWidth()), QKeySequence(Qt::CTRL + Qt::Key_9));
    m->addAction("Zoom In", viewer, SLOT(zoomIn()), QKeySequence::ZoomIn);
    m->addAction("Zoom Out", viewer, SLOT(zoomOut()), QKeySequence::ZoomOut);

    m = menuBar()->addMenu("Script");
    runAction = m->addAction("Run", this, SLOT(run()), QKeySequence(Qt::CTRL + Qt::Key_R));
    stopAction = m->addAction("Stop", this, SLOT(stop()));
    stopAction->setEnabled(false);
}

void MainWindow::setCanonicalPath(const QString &path)
{
    if (path.isEmpty()) {
        setWindowFilePath("Untitled");
        canonicalDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    } else {
        QFileInfo info(path);
        canonicalPath = info.canonicalFilePath();
        canonicalDir = info.absoluteDir().canonicalPath();
        setWindowFilePath(canonicalPath);
    }
    editor->document()->setModified(false);
    setWindowModified(false);
}

bool MainWindow::eventuallySave()
{
    if (editor->document()->isModified()) {
        QString name = QFileInfo(windowFilePath()).fileName();
        QMessageBox box(this);
        box.setText(QString("Do you want to save the changes made to the document “%1”?").arg(name));
        box.setInformativeText("Your changes will be lost if you don’t save them.");
        box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Save);
        box.setWindowModality(Qt::WindowModal);
        box.setIcon(QMessageBox::Warning);
        switch (box.exec()) {
        case QMessageBox::Save:
            return saveDocument();
        case QMessageBox::Cancel:
            return false;
        }
    }
    return true;
}

MainWindow *MainWindow::findMainWindow(const QString &path)
{
    QString p = QFileInfo(path).canonicalFilePath();
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        MainWindow *w = qobject_cast<MainWindow *>(widget);
        if (w && w->canonicalPath == p) {
            return w;
        }
    }
    return 0;
}

void MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        QString text = QString("Cannot load file %1:\n%2.").arg(path).arg(file.errorString());
        QMessageBox::warning(this, "Even", text);
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    NORMALIZE(text);
    editor->setPlainText(text);
    setCanonicalPath(path);
}

bool MainWindow::saveFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        QString text = QString("Cannot save file %1:\n%2.").arg(path).arg(file.errorString());
        QMessageBox::warning(this, "Even", text);
        return false;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << editor->toPlainText();
    setCanonicalPath(path);
    return true;
}

void MainWindow::loadSettings()
{

}

void MainWindow::saveSettings()
{

}
