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

    ui->GammaSlider->setVisible(false);


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

// Load Image

void MainWindow::on_actionLoad_Image_triggered()
{
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
    QImage displayImage(originalImage.buffer->data(), originalImage.meta.width, originalImage.meta.height, QImage::Format_Grayscale8);
    ui->OriginalWindowLabel->setPixmap(QPixmap::fromImage(displayImage.scaled(ui->OriginalWindowLabel->size(), Qt::KeepAspectRatio)));

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

    // Update the result window
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));

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

    // Update the result window
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));

    QMessageBox::information(this, tr("Redo"), tr("Redone the previous undo action."));


}




void MainWindow::on_actionNegative_triggered() {
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
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
    ui->ResultWindowLabel->repaint();
}

void MainWindow::on_actionLog_triggered() {

    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    double c = 255.0 / log(1 + 255.0);

    qDebug() << "Applying Log Transformation...";

    applyLogTransform(&resultImage, c);

    qDebug() << "Log Transformation Completed";

    // Update the result window
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));

}

void MainWindow::on_actionGamma_triggered() {

    ui->GammaSlider->setVisible(true);

    if (!originalImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    previousImage = resultImage;

    double gammaValue = ui->GammaSlider->value() / 10.0;

    qDebug() << "Applying Gamma Transformation...";

    applyGammaTransform(&resultImage, 1.0, gammaValue);

    qDebug() << "Gamma Transformation Completed";

    // Update the result window
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
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

    // Update the result window
    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
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

    QImage qResultImage(resultImage.buffer->data(), resultImage.meta.width, resultImage.meta.height, QImage::Format_Grayscale8);
    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(qResultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
    ui->ResultWindowLabel->update();
}

void MainWindow::on_actionBox_Filter_triggered()
{
    activeFilter = FilterType::Box;
    applyFilter(activeFilter);

}

void MainWindow::on_actionGaussian_Filter_triggered()
{
    activeFilter = FilterType::Gaussian;
    applyFilter(activeFilter);
}


void MainWindow::on_actionMedian_Filter_triggered()
{
    activeFilter = FilterType::Median;
    applyFilter(activeFilter);
}

