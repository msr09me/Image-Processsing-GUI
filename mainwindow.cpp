#include <QFileDialog> // For QFileDialog
#include <QMessageBox> // For QMessageBox
#include <QImage>      // For QImage
#include <QPixmap>     // For displaying images in QLabel
#include <cstring>     // For std::memcpy (used in helper functions)
#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "imageprocessingbackend.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove and re-add menus in the desired order
    menuBar()->clear();
    menuBar()->addMenu(ui->menuFile);                     // Add File menu first
    menuBar()->addMenu(ui->menuIntensity_Transformation); // Add Intensity Transformation menu second
    menuBar()->addMenu(ui->menuSpatial_Transformation);     // Add Spatial Transformation menu second
    menuBar()->addMenu(ui->menuConverter);

    hideControlElements();



/*
 *
 * // Connect slots to signals
    connect(ui->actionNegative, &QAction::triggered, this, &MainWindow::on_actionNegative_triggered);
    connect(ui->actionLog, &QAction::triggered, this, &MainWindow::on_actionLog_triggered);
    connect(ui->actionGamma, &QAction::triggered, this, &MainWindow::on_actionGamma_triggered);
    connect(ui->LoadImagePushButton, &QPushButton::clicked, this, &MainWindow::on_LoadImagePushButton_clicked);
    connect(ui->NegativePushButton, &QPushButton::clicked, this, &MainWindow::on_NegativePushButton_clicked);
    connect(ui->LogPushButton, &QPushButton::clicked, this, &MainWindow::on_LogPushButton_clicked);
    connect(ui->GammaPushButton, &QPushButton::clicked, this, &MainWindow::on_GammaPushButton_clicked);
    connect(ui->GammaSlider, &QSlider::valueChanged, this, &MainWindow::on_GammaSlider_valueChanged); // Connect slider

*/

}

MainWindow::~MainWindow()
{
    delete ui;

}

// Functions to hide all the controls ---------------------------------------------------------------------

void MainWindow::hideControlElements() {
    hideConversionControls();
    hideGammaSlider();
    hideKernelSize();
}

void MainWindow::hideGammaSlider(){
    ui->GammaSlider->setVisible(false);
}

void MainWindow::hideKernelSize(){
    ui->kernelSize->setVisible(false);
    ui->kernelSizeSlider->setVisible(false);
    ui->kernelSizeSpinBox->setVisible(false);
}


// Hide the threshold controls (if needed elsewhere)
void MainWindow::hideConversionControls() {
    ui->ThresholdLabel->setVisible(false);
    ui->ThresholdSlider->setVisible(false);
    ui->ThresholdSpinBox->setVisible(false);
    ui->ImageConverterPushButton->setVisible(false);
}

// Function to show control elements ----------------------------------------------------------------------

// Show the threshold controls
void MainWindow::showConversionControls() {
    ui->ThresholdLabel->setVisible(true);
    ui->ThresholdSlider->setVisible(true);
    ui->ThresholdSpinBox->setVisible(true);
    ui->ImageConverterPushButton->setVisible(true);
}

// Show gamma slider

void MainWindow::showGammaSlider() {
    ui->GammaSlider->setVisible(true);
}

// Show kernel size elements
void MainWindow::showkernelSize() {
    ui->kernelSize->setVisible(true);
    ui->kernelSizeSlider->setVisible(true);
    ui->kernelSizeSpinBox->setVisible(true);
}

// Function to display image
void MainWindow::updateImageDisplay(const ImageReadResult &image, QLabel *label)
{
    if (!image.buffer) {
        qDebug() << "No image data to display.";
        return;
    }

    QImage displayImage(image.buffer->data(), image.meta.width, image.meta.height, QImage::Format_Grayscale8);
    QImage flippedImage = displayImage.mirrored(false, true); // Flip vertically
    label->setPixmap(QPixmap::fromImage(flippedImage.scaled(label->size(), Qt::KeepAspectRatio)));
}

// Load Image

void MainWindow::on_actionLoad_Image_triggered()
{
    hideControlElements();
    qDebug() << "on_LoadImagePushButton_clicked called"; // Debug statement

    inputImagePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("BMP Files (*.bmp)"));
    if (inputImagePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("No image selected!"));
        return;
    }

    // Load the image
    originalImage = readImage(inputImagePath.toStdString().c_str());
    if (!originalImage.buffer) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load the BMP image!"));
        return;
    }

    resultImage = originalImage;

    // Display the image in the original image QLabel
    updateImageDisplay(originalImage, ui->OriginalWindowLabel);
}


// Undo icon

void MainWindow::on_UndoPushButton_clicked()
{
    if (!previousImage.buffer) {
        QMessageBox::information(this, tr("Information"), tr("No previous state to undo to."));
        return;
    }

    redoImage = resultImage;
    resultImage = previousImage;

    updateImageDisplay(resultImage, ui->ResultWindowLabel);

    QMessageBox::information(this, tr("Undo"), tr("Reverted to the previous state."));

}

// Redo icon

void MainWindow::on_RedoPushushButton_clicked()
{
    if (!redoImage.buffer) {
        QMessageBox::information(this, tr("Information"), tr("No redo state available."));
        return;
    }

    previousImage = resultImage;
    resultImage = redoImage;

    updateImageDisplay(resultImage, ui->ResultWindowLabel);

    QMessageBox::information(this, tr("Redo"), tr("Redone the previous undo action."));


}




void MainWindow::on_actionNegative_triggered() {
    hideControlElements();
    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    qDebug() << "Applying Negative Transformation...";

    // Apply transformation
    applyNegativeB(&resultImage);

    qDebug() << "Negative Transformation Completed";

    // Update the result window
    updateImageDisplay(resultImage, ui->ResultWindowLabel);

}

void MainWindow::on_actionLog_triggered() {
    hideControlElements();

    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    double c = 255.0 / log(1 + 255.0);

    qDebug() << "Applying Log Transformation...";

    applyLogTransform(&resultImage, c);

    qDebug() << "Log Transformation Completed";

    updateImageDisplay(resultImage, ui->ResultWindowLabel);


}

void MainWindow::on_actionGamma_triggered() {
    hideControlElements();

    showGammaSlider();

    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    double gammaValue = ui->GammaSlider->value() / 10.0;

    qDebug() << "Applying Gamma Transformation...";

    applyGammaTransform(&resultImage, 1.0, gammaValue);

    qDebug() << "Gamma Transformation Completed";

    updateImageDisplay(resultImage, ui->ResultWindowLabel);

}

void MainWindow::on_GammaSlider_sliderReleased()
{
    // Convert slider value to gamma
    double gammaValue = ui->GammaSlider->value() / 100.0;

    // Update the GammaPushButton text to show the current gamma value
    ui->actionGamma->setText(tr("Gamma: %1").arg(gammaValue, 0, 'f', 1));

    // If the original image is loaded, dynamically update the result preview
    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    // Apply the gamma transformation directly to the image in the backend
    qDebug() << "Applying gamma transformation";
    applyGammaTransform(&resultImage, 1.0, gammaValue);

    updateImageDisplay(resultImage, ui->ResultWindowLabel);

}



void MainWindow::on_kernelSizeSlider_valueChanged(int value)
{
    if (value % 2 == 0) {
        value += 1; // Ensure the value remains odd
        ui->kernelSizeSlider->setValue(value);
    }

    // Temporarily block signals to avoid triggering spin box's valueChanged
    ui->kernelSizeSpinBox->blockSignals(true);
    ui->kernelSizeSpinBox->setValue(value);
    ui->kernelSizeSpinBox->blockSignals(false);

    qDebug() << "Slider value changed to:" << value;
}

void MainWindow::on_kernelSizeSlider_sliderReleased()
{
    qDebug() << "Slider released";
    applyFilter(activeFilter); // Perform filtering only when the slider is released
}


void MainWindow::on_kernelSizeSpinBox_valueChanged(int value)
{
    if (value % 2 == 0) {
        value += 1; // Ensure the value remains odd
        ui->kernelSizeSpinBox->setValue(value);
    }
    ui->kernelSizeSlider->setValue(value);

    qDebug() << "Applying filter using spin box value";

    applyFilter(activeFilter);

}



void MainWindow::applyFilter(FilterType filterType)
{
    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    int kernelSize = ui->kernelSizeSpinBox->value();
    qDebug() << "Applying filter with kernel size:" << kernelSize;

    try {
        switch (filterType) {
        case Box:
            boxFilter(previousImage, resultImage, kernelSize);
            break;
        case Gaussian:
            gaussianFilter(previousImage, resultImage, kernelSize, 1.0);
            break;
        case Median:
            medianFilter(previousImage, resultImage, kernelSize);
            break;
        }
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Error"), tr("Filter failed: %1").arg(e.what()));
        return;
    }

    updateImageDisplay(resultImage, ui->ResultWindowLabel);

}

void MainWindow::on_actionBox_Filter_triggered()
{
    hideControlElements();
    showkernelSize();
    activeFilter = FilterType::Box;
    applyFilter(activeFilter);

}

void MainWindow::on_actionGaussian_Filter_triggered()
{
    hideControlElements();
    showkernelSize();
    activeFilter = FilterType::Gaussian;
    applyFilter(activeFilter);
}


void MainWindow::on_actionMedian_Filter_triggered()
{
    hideControlElements();
    showkernelSize();
    activeFilter = FilterType::Median;
    applyFilter(activeFilter);
}


void MainWindow::on_actionBasic_Laplacian_triggered()
{
    hideControlElements();
    qDebug() << "Applying highpass filter with Basic Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 1);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Basic Laplacian completed";
}


void MainWindow::on_actionFull_Laplacian_triggered()
{
    hideControlElements();
    qDebug() << "Applying highpass filter with Full Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 2);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Full Laplacian completed";
}


void MainWindow::on_actionBasic_Inverted_Laplacian_triggered()
{
    hideControlElements();
    qDebug() << "Applying highpass filter with Basic Inverted Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 3);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Basic Inverted Laplacian completed";
}


void MainWindow::on_actionFull_Inverted_Laplacian_triggered()
{
    hideControlElements();
    qDebug() << "Applying highpass filter with Full Inverted Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 4);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Full Inverted Laplacian completed";
}


void MainWindow::on_actionSobel_triggered()
{
    hideControlElements();
    qDebug() << "Applying highpass filter with Sobel Operator";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 5);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Sobel completed";
}


void MainWindow::on_actionBasic_Laplacian_2_triggered()
{
    hideControlElements();
    qDebug() << "Applying image sharpening with basic laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 1);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with basic laplacian highpass filter";
}


void MainWindow::on_actionFull_Laplacian_2_triggered()
{
    hideControlElements();
    qDebug() << "Applying image sharpening with full laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 2);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with full laplacian highpass filter";
}


void MainWindow::on_actionBaisc_Inverted_Laplacian_triggered()
{
    hideControlElements();
    qDebug() << "Applying image sharpening with basic inverted laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 3);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with basic inverted laplacian highpass filter";
}


void MainWindow::on_actionFull_Inverted_Laplacian_2_triggered()
{
    hideControlElements();
    qDebug() << "Applying image sharpening with full inverted laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 4);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with full inverted laplacian highpass filter";
}


void MainWindow::on_actionSobel_2_triggered()
{
    hideControlElements();
    qDebug() << "Applying image sharpening with soble highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 5);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with sobel highpass filter";
}




void MainWindow::on_actionUnsharp_Maksing_Highboost_Filtering_triggered()
{
    hideControlElements();
    qDebug() << "Applying Unsharp Masking";
    previousImage = resultImage;



    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed Unsharp Masking";
}

// Image Converter -----------------------------------------------------------
void MainWindow::on_actionGrayscale_to_Binary_triggered()
{
    hideControlElements();
    showConversionControls(); // Show threshold controls
}

// Slot for slider value change
void MainWindow::on_ThresholdSlider_valueChanged(int value) {
    ui->ThresholdSpinBox->blockSignals(true);
    ui->ThresholdSpinBox->setValue(value); // Sync with spinbox
    ui->ThresholdSpinBox->blockSignals(false);
}

// Slot for spinbox value change
void MainWindow::on_ThresholdSpinBox_valueChanged(int value) {
    ui->ThresholdSlider->blockSignals(true);
    ui->ThresholdSlider->setValue(value); // Sync with slider
    ui->ThresholdSlider->blockSignals(false);
}

// Slot for Convert button click
void MainWindow::on_ImageConverterPushButton_clicked() {
    if (!resultImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    int threshold = ui->ThresholdSpinBox->value(); // Get threshold value

    previousImage = resultImage; // store the current result image as previous image

    // Apply grayscale to binary conversion using your algorithm
    grayscaleToBinary(previousImage, resultImage, threshold);

    // Update the Result Image display
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed conversion from grayscale to binary";
}



