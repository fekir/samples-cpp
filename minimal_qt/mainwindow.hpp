#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	virtual ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
