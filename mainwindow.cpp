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

    // Initialize previousImage
    previousImage.data = nullptr;
    previousImage.width = 0;
    previousImage.height = 0;
    previousImage.bitDepth = 0;

    /*

    // Connect slots to signals
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
    freeImage(&previousImage);
    freeImage(&originalImage);
    freeImage(&image);
}

void MainWindow::on_LoadImagePushButton_clicked() {

    qDebug() << "on_LoadImagePushButton_clicked called"; // Debug statement

    inputImagePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("BMP Files (*.bmp)"));
    if (inputImagePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("No image selected!"));
        return;
    }

    // Load the image
    originalImage = loadImage(inputImagePath.toStdString().c_str());
    if (!originalImage.data) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load the BMP image!"));
        return;
    }

    image = originalImage;


    // Display the image in the original image QLabel
    QImage displayImage(originalImage.data, originalImage.width, originalImage.height, QImage::Format_Grayscale8);
    ui->OriginalWindowLabel->setPixmap(QPixmap::fromImage(displayImage.scaled(ui->OriginalWindowLabel->size(), Qt::KeepAspectRatio)));
}



void MainWindow::on_NegativePushButton_clicked() {

    if (!originalImage.data) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    freeImage(&previousImage);
    previousImage.data = (uint8_t *)malloc(image.width * image.height * (image.bitDepth / 8));
    if (previousImage.data) {
        memcpy(previousImage.data, image.data, image.width * image.height * (image.bitDepth / 8));
        previousImage.width = image.width;
        previousImage.height = image.height;
        previousImage.bitDepth = image.bitDepth;
    }

    image = originalImage;

    qDebug() << "Applying Negative Transformation...";



    // Apply transformation
    applyNegativeB(&image);

    // Update the result window
    QImage resultImage(image.data, image.width, image.height, QImage::Format_Grayscale8);

    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(resultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
    ui->ResultWindowLabel->repaint();
}



void MainWindow::on_LogPushButton_clicked()
{
    if (!originalImage.data) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    freeImage(&previousImage);
    previousImage.data = (uint8_t *)malloc(image.width * image.height * (image.bitDepth / 8));
    if (previousImage.data) {
        memcpy(previousImage.data, image.data, image.width * image.height * (image.bitDepth / 8));
        previousImage.width = image.width;
        previousImage.height = image.height;
        previousImage.bitDepth = image.bitDepth;
    }

    image = originalImage;

    double c = 255.0 / log(1 + 255.0);
    applyLogTransform(&image, c);

    QImage resultImage(image.data, image.width, image.height, QImage::Format_Grayscale8);
    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(resultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
}

void MainWindow::on_GammaPushButton_clicked()
{
    if (!originalImage.data) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    freeImage(&previousImage);
    previousImage.data = (uint8_t *)malloc(image.width * image.height * (image.bitDepth / 8));
    if (previousImage.data) {
        memcpy(previousImage.data, image.data, image.width * image.height * (image.bitDepth / 8));
        previousImage.width = image.width;
        previousImage.height = image.height;
        previousImage.bitDepth = image.bitDepth;
    }

    image = originalImage;

    double gammaValue = ui->GammaSlider->value() / 10.0;
    applyGammaTransform(&image, 1.0, gammaValue);

    QImage resultImage(image.data, image.width, image.height, QImage::Format_Grayscale8);
    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(resultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
}

void MainWindow::on_GammaSlider_valueChanged(int value)
{
    // Convert slider value to gamma (assuming the slider range is 10–50, mapping to 1.0–5.0)
    double gammaValue = value / 10.0;

    // Update the GammaPushButton text to show the current gamma value
    ui->GammaPushButton->setText(tr("Gamma: %1").arg(gammaValue, 0, 'f', 1));

    // If the original image is loaded, dynamically update the result preview
    if (!originalImage.data) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    freeImage(&previousImage);
    previousImage.data = (uint8_t *)malloc(image.width * image.height * (image.bitDepth / 8));
    if (previousImage.data) {
        memcpy(previousImage.data, image.data, image.width * image.height * (image.bitDepth / 8));
        previousImage.width = image.width;
        previousImage.height = image.height;
        previousImage.bitDepth = image.bitDepth;
    }

    image = originalImage;

    // Apply the gamma transformation directly to the image in the backend
    applyGammaTransform(&image, 1.0, gammaValue);

    // Convert the processed backend image to QImage for display
    QImage resultImage(image.data, image.width, image.height, QImage::Format_Grayscale8);
    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(resultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));
}



void MainWindow::on_UndoPushButton_clicked()
{
    if (!previousImage.data) {
        QMessageBox::information(this, tr("Information"), tr("No previous state to undo to."));
        return;
    }

    // Restore the previous image state
    freeImage(&image); // Free the current image memory
    image.data = (uint8_t *)malloc(previousImage.width * previousImage.height * (previousImage.bitDepth / 8));
    if (image.data) {
        memcpy(image.data, previousImage.data, previousImage.width * previousImage.height * (previousImage.bitDepth / 8));
        image.width = previousImage.width;
        image.height = previousImage.height;
        image.bitDepth = previousImage.bitDepth;
    }

    // Update the result display
    QImage resultImage(image.data, image.width, image.height, QImage::Format_Grayscale8);
    ui->ResultWindowLabel->setPixmap(QPixmap::fromImage(resultImage.scaled(ui->ResultWindowLabel->size(), Qt::KeepAspectRatio)));

    QMessageBox::information(this, tr("Undo"), tr("Reverted to the previous state."));

}

