#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage>
#include <QLabel>

#include "imageprocessingbackend.h"
#include "ImageFilter.h"

enum FilterType { Box, Gaussian, Median };
enum KernelType {BasicLaplacian, FullLaplacian, BasicInvertedLaplacian, FullInvertedLaplacian};
enum class MorphologicalOperation { Erosion, Dilation, Opening, Closing };

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

    void on_actionBasic_Laplacian_triggered();

    void on_actionFull_Laplacian_triggered();

    void on_actionBasic_Inverted_Laplacian_triggered();

    void on_actionFull_Inverted_Laplacian_triggered();

    void on_actionSobel_triggered();

    void on_actionBasic_Laplacian_2_triggered();

    void on_actionFull_Laplacian_2_triggered();

    void on_actionBaisc_Inverted_Laplacian_triggered();

    void on_actionFull_Inverted_Laplacian_2_triggered();

    void on_actionSobel_2_triggered();

    void on_actionUnsharp_Maksing_Highboost_Filtering_triggered();

    // Image converter

    void on_actionGrayscale_to_Binary_triggered(); // Slot for menu action
    void on_ThresholdSlider_valueChanged(int value); // Slot for slider
    void on_ThresholdSpinBox_valueChanged(int value); // Slot for spinbox
    void on_ImageConverterPushButton_clicked(); // Slot for convert button


    // Morphological

    void on_actionErosion_triggered();

    void on_actionDilation_triggered();

    void on_actionOpening_triggered();

    void on_actionClosing_triggered();

    void on_mKernelSlider_valueChanged(int value);

    void on_mkernelSpinBox_valueChanged(int arg1);

    void on_mKernelSlider_2_valueChanged(int value);

    void on_mKernelSpinBox2_valueChanged(int arg1);

    void on_applyPushButton_clicked();

private:
    Ui::MainWindow *ui;

    // Helper functions to hide the control elements on the ui
    void hideControlElements();          // Hide all the elements
    void hideGammaSlider();             // Hide the gamma slider
    void hideConversionControls();      // Hide conversion elemetns
    void hideKernelSize();              // Hide the kernel size elements
    void hideMorphologicalControls();   // Hide the morphological control elements

    // Helper function to show control elements on the ui
    void showGammaSlider();              // Show the gamma slider
    void showkernelSize();               // Show kernel size elements
    void showConversionControls();       // Show control elements for image conversion
    void showMorphologicalControls();       // Show control elements for morphological operations
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
    void updateImageDisplay(const ImageReadResult &image, QLabel *label);

    MorphologicalOperation currentMorphologicalOperation;

    // For QStack
    void switchToPage(int pageIndex);


};
#endif // MAINWINDOW_H
