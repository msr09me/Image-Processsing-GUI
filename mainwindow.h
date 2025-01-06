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
    void on_LoadImagePushButton_clicked();

    void on_NegativePushButton_clicked();

    void on_LogPushButton_clicked();

    void on_GammaPushButton_clicked();

    void on_GammaSlider_valueChanged(int value);

    void on_UndoPushButton_clicked();

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
};
#endif // MAINWINDOW_H
