#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>
#include <QtWidgets>
#include "movingsplitter.h"
#include "viewerwidget.h"
#include "pythonedit.h"
#include "findwidget.h"
#include "gowidget.h"
#include "baseedit.h"
#include "activitywidget.h"
#include "hidinglabel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    MainWindow(const QString &path);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveDocumentAs();
    bool exportResult();

    void focusChanged(QWidget *old, QWidget *now);

    void toggleFinder();
    void find(const QString &needle, QTextDocument::FindFlags options = 0);
    void replace(const QString &needle, const QString &replacement);
    void findNext();
    void findPrevious();

    void toggleGoer();
    void go(int line);

    void run();
    void log();
    void accept();
    void discard();
    void read();
    void stop();
    void done();

    void splitterMoved(QMoveEvent *e);
    void zoomChanged(qreal scale);
    void positionChanged();

private:
    void setupHierarchy();
    void setupInterpreter();
    void setupSignaling();
    void setupMenu();

    void setCanonicalPath(const QString &path);
    bool eventuallySave();
    MainWindow *findMainWindow(const QString &path);
    void loadFile(const QString &path);
    bool saveFile(const QString &path);
    void loadSettings();
    void saveSettings();

private:
    QSplitter *hSplitter;
    MovingSplitter *vSplitter;
    ViewerWidget *viewer;
    PythonEdit *editor;
    FindWidget *finder;
    GoWidget *goer;
    BaseEdit *console;
    QLabel *zoom;
    ActivityWidget *activity;
    QLabel *coordinates;
    HidingLabel *message;

    QAction *exportAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *runAction;
    QAction *stopAction;

    QString previousNeedle;
    QString canonicalPath;
    QString canonicalDir;

    QTcpServer *server;
    QTcpSocket *socket;
    bool socketConnected;
    quint32 payloadSize;
    QProcess *process;
    QByteArray *result;
};

#endif // MAINWINDOW_H
