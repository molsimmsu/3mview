#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setGeometry(0, 0, 700, 700);

    ost::gui::GLWin* gl_win;
    QGLFormat f;
    f.setOverlay(true);
    f.setStereo(true);

    ostGL = new ost::gui::GLCanvas(gl_win, this, f);
    ostGL->resize(width(), height());
    ostGL->show();

    ost::gfx::Scene& scene = ost::gfx::Scene::Instance();
    scene.SetBackground(ost::gfx::Color(0.1, 0.1, 0.5));
    scene.SetShowCenter(true);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ostGL->resize(width(), height());
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	QApplication::exit();
}
