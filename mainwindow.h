#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage>

#include "imageprocessingbackend.h"
#include "ImageFilter.h"

enum FilterType { Box, Gaussian, Median };

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

    void on_actionNegative_triggered();
    void on_actionLog_triggered();
    void on_actionGamma_triggered();
    void on_GammaSlider_sliderReleased();


    void on_actionLoad_Image_triggered();
    void on_UndoPushButton_clicked();
    void on_RedoPushushButton_clicked();

    void on_actionBox_Filter_triggered();

    void on_kernelSizeSlider_valueChanged(int value);

    void on_kernelSizeSlider_sliderReleased();

    void on_kernelSizeSpinBox_valueChanged(int value);

    void on_actionGaussian_Filter_triggered();

    void on_actionMedian_Filter_triggered();

private:
    Ui::MainWindow *ui;
    // Member variables
    QString inputImagePath; // To store the file path of the loaded image
   // QImage originalImage;   // To store the original image
    QByteArray originalImageToRawData(const QImage &image);
    QImage rawDataToQImage(const QByteArray &data, int width, int height, QImage::Format format);
    ImageReadResult originalImage;
    ImageReadResult resultImage;
    ImageReadResult previousImage;
    ImageReadResult redoImage;

    FilterType activeFilter = FilterType::Box; // Declare activeFilter here
    void applyFilter(FilterType filterType);
};
#endif // MAINWINDOW_H
