#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage>

#include "imageprocessingbackend.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_UndoPushButton_clicked();

    void on_actionNegative_triggered();
    void on_actionLog_triggered();
    void on_actionGamma_triggered();

    void on_actionLoad_Image_triggered();

    void on_RedoPushushButton_clicked();

    void on_GammaSlider_sliderReleased();

private:
    Ui::MainWindow *ui;
    // Member variables
    QString inputImagePath; // To store the file path of the loaded image
   // QImage originalImage;   // To store the original image
    QByteArray originalImageToRawData(const QImage &image);
    QImage rawDataToQImage(const QByteArray &data, int width, int height, QImage::Format format);
    Image originalImage;
    Image image;
    Image previousImage;
    Image redoImage;
};
#endif // MAINWINDOW_H
