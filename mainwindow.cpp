// Includes {
#include <QFileDialog> // For QFileDialog
#include <QMessageBox> // For QMessageBox
#include <QImage>      // For QImage
#include <QPixmap>     // For displaying images in QLabel
#include <cstring>     // For std::memcpy (used in helper functions)
#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "imageprocessingbackend.h"

// }

// Constructors and Desctructors {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove and re-add menus in the desired order
    menuBar()->clear();
    menuBar()->addMenu(ui->menuFile);                     // Add File menu first
    menuBar()->addMenu(ui->menuIntensity_Transformation); // Add Intensity Transformation menu second
    menuBar()->addMenu(ui->menuSpatial_Transformation);     // Add Spatial Transformation menu third
    menuBar()->addMenu(ui->menuConverter);
    menuBar()->addMenu(ui->menuMorphological);
    menuBar()->addMenu(ui->menuEdge_Detection);

    switchToPage(0);

    //hideControlElements();



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

// }

// Helper functions {

// QStack
void MainWindow::switchToPage(int pageIndex)  //Helper function to switch between pages
{
    ui->stackedWidget->setCurrentIndex(pageIndex);
    qDebug() << "Switching to page:" << pageIndex;
}


// Functions to hide all the controls ---------------------------------------------------------------------

void MainWindow::hideControlElements() {
    hideConversionControls();
    hideGammaSlider();
    hideKernelSize();
    hideMorphologicalControls();
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

void MainWindow::hideMorphologicalControls()
{
    ui->mKernelRow->setVisible(false);
    ui->mKernelColumn->setVisible(false);
    ui->mKernelSlider->setVisible(false);
    ui->mkernelSpinBox->setVisible(false);
    ui->mKernelSlider_2->setVisible(false);
    ui->mKernelSpinBox2->setVisible(false);
    ui->applyPushButton->setVisible(false);
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

void MainWindow::showMorphologicalControls()
{
    ui->mKernelRow->setVisible(true);
    ui->mKernelColumn->setVisible(true);
    ui->mKernelSlider->setVisible(true);
    ui->mkernelSpinBox->setVisible(true);
    ui->mKernelSlider_2->setVisible(true);
    ui->mKernelSpinBox2->setVisible(true);
    ui->applyPushButton->setVisible(true);
}

// }

// Load and display image {


// --------------------------------------------------------------------------------------------------------------

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

// }


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


// Intensity transformation handlers {

void MainWindow::on_actionNegative_triggered() {
    //hideControlElements();
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
    //hideControlElements();

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
    //hideControlElements();
    switchToPage(3);

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

// }

// Spatial transformation handlers {

// Low pass filter handlers {



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
    //hideControlElements();
    switchToPage(4);
    showkernelSize();
    activeFilter = FilterType::Box;
    applyFilter(activeFilter);

}

void MainWindow::on_actionGaussian_Filter_triggered()
{
    //hideControlElements();
    switchToPage(4);
    showkernelSize();
    activeFilter = FilterType::Gaussian;
    applyFilter(activeFilter);
}


void MainWindow::on_actionMedian_Filter_triggered()
{
    //hideControlElements();
    switchToPage(4);
    showkernelSize();
    activeFilter = FilterType::Median;
    applyFilter(activeFilter);
}

// }

// High pass filter handlers {


void MainWindow::on_actionBasic_Laplacian_triggered()
{
    //hideControlElements();
    qDebug() << "Applying highpass filter with Basic Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 1);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Basic Laplacian completed";
}


void MainWindow::on_actionFull_Laplacian_triggered()
{
    //hideControlElements();
    qDebug() << "Applying highpass filter with Full Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 2);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Full Laplacian completed";
}


void MainWindow::on_actionBasic_Inverted_Laplacian_triggered()
{
    //hideControlElements();
    qDebug() << "Applying highpass filter with Basic Inverted Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 3);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Basic Inverted Laplacian completed";
}


void MainWindow::on_actionFull_Inverted_Laplacian_triggered()
{
    //hideControlElements();
    qDebug() << "Applying highpass filter with Full Inverted Laplacian";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 4);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Full Inverted Laplacian completed";
}


void MainWindow::on_actionSobel_triggered()
{
    //hideControlElements();
    qDebug() << "Applying highpass filter with Sobel Operator";
    previousImage = resultImage;
    highpassFilter(previousImage, resultImage, 5);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Highpass filter with Sobel completed";
}

// }

// Image sharpening handlers {


void MainWindow::on_actionBasic_Laplacian_2_triggered()
{
    //hideControlElements();
    qDebug() << "Applying image sharpening with basic laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 1);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with basic laplacian highpass filter";
}


void MainWindow::on_actionFull_Laplacian_2_triggered()
{
    //hideControlElements();
    qDebug() << "Applying image sharpening with full laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 2);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with full laplacian highpass filter";
}


void MainWindow::on_actionBaisc_Inverted_Laplacian_triggered()
{
    //hideControlElements();
    qDebug() << "Applying image sharpening with basic inverted laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 3);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with basic inverted laplacian highpass filter";
}


void MainWindow::on_actionFull_Inverted_Laplacian_2_triggered()
{
    //hideControlElements();
    qDebug() << "Applying image sharpening with full inverted laplacian highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 4);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with full inverted laplacian highpass filter";
}


void MainWindow::on_actionSobel_2_triggered()
{
    //hideControlElements();
    qDebug() << "Applying image sharpening with soble highpass filter";
    previousImage = resultImage;
    imageSharpening(previousImage, resultImage, 5);
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed image sharpening with sobel highpass filter";
}

// }



void MainWindow::on_actionUnsharp_Maksing_Highboost_Filtering_triggered()
{
    //hideControlElements();
    qDebug() << "Applying Unsharp Masking";
    previousImage = resultImage;



    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Completed Unsharp Masking";
}

// }

// Image converter handlers {

// Image Converter -----------------------------------------------------------

// Grayscale to Binary handlers {
void MainWindow::on_actionGrayscale_to_Binary_triggered()
{
    //hideControlElements();
    showConversionControls(); // Show threshold controls
    switchToPage(1);
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

// }

//}

// Morphological handlers {


// Morphology ---------------------------------------------------------------------------

void MainWindow::on_actionErosion_triggered()
{
    //hideControlElements();
    switchToPage(2);
    showMorphologicalControls();
    currentMorphologicalOperation = MorphologicalOperation::Erosion;

}


void MainWindow::on_actionDilation_triggered()
{
    //hideControlElements();
    switchToPage(2);
    showMorphologicalControls();
    currentMorphologicalOperation = MorphologicalOperation::Dilation;
}


void MainWindow::on_actionOpening_triggered()
{
    //hideControlElements();
    switchToPage(2);
    showMorphologicalControls();
    currentMorphologicalOperation = MorphologicalOperation::Opening;
}


void MainWindow::on_actionClosing_triggered()
{
    //hideControlElements();
    switchToPage(2);
    showMorphologicalControls();
    currentMorphologicalOperation = MorphologicalOperation::Closing;
}

void MainWindow::on_actionBoundary_Extraction_triggered()
{
    switchToPage(2);
    showMorphologicalControls();
    currentMorphologicalOperation = MorphologicalOperation::BoundaryExtraction;
}



void MainWindow::on_mKernelSlider_valueChanged(int value)
{
    ui->mkernelSpinBox->blockSignals(true);
    ui->mkernelSpinBox->setValue(value);
    ui->mkernelSpinBox->blockSignals(false);
}


void MainWindow::on_mkernelSpinBox_valueChanged(int value)
{
    ui->mKernelSlider->blockSignals(true);
    ui->mKernelSlider->setValue(value);
    ui->mKernelSlider->blockSignals(false);
}


void MainWindow::on_mKernelSlider_2_valueChanged(int value)
{
    ui->mKernelSpinBox2->blockSignals(true);
    ui->mKernelSpinBox2->setValue(value);
    ui->mKernelSpinBox2->blockSignals(false);
}


void MainWindow::on_mKernelSpinBox2_valueChanged(int value)
{
    ui->mKernelSlider_2->blockSignals(true);
    ui->mKernelSlider_2->setValue(value);
    ui->mKernelSlider_2->blockSignals(false);
}


void MainWindow::on_applyPushButton_clicked()
{
    if (!resultImage.buffer) {
        QMessageBox::warning(this, tr("Warning"), tr("Load an image first!"));
        return;
    }

    int kernelRows = ui->mkernelSpinBox->value();
    int kernelColumns = ui->mKernelSpinBox2->value();

    previousImage = resultImage; // store the current result image as previous image

    switch (currentMorphologicalOperation) {
        case MorphologicalOperation::Erosion:
            qDebug() << "Applying Erosion...";
            erosion(previousImage, resultImage, kernelColumns, kernelRows);
            break;
        case MorphologicalOperation::Dilation:
            qDebug() << "Applying Dilation...";
            dilation(previousImage, resultImage, kernelColumns, kernelRows);
            break;
        case MorphologicalOperation::Opening:
            qDebug() << "Applying Opening...";
            opening(previousImage, resultImage, kernelColumns, kernelRows);
            break;
        case MorphologicalOperation::Closing:
            qDebug() << "Applying Closing...";
            closing(previousImage, resultImage, kernelColumns, kernelRows);
            break;
        case MorphologicalOperation::BoundaryExtraction:
            qDebug() << "Applying Boundary Extraction...";
            boundaryExtraction(previousImage, resultImage, kernelColumns, kernelRows);
            break;
        default:
            break;
    }

    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Operation completed.";
}




// Edge Detection ---------------------------------------------------------------------------------

// Gradient Based -------------------------------------------------------------

void MainWindow::on_actionGradient_Based_triggered()
{
    switchToPage(5);
}


void MainWindow::on_applyPushButton_2_clicked()
{
    if (!resultImage.buffer) {
        QMessageBox::warning(this, "Warning", "Please load an image first!");
        return;
    }

    // 1. Determine which kernel is selected
    KernelChoice kernelChoice;
    if (ui->sobelRadioButton->isChecked()) {
        kernelChoice = KernelChoice::SOBEL;
    } else if (ui->prewittRadioButton->isChecked()) {
        kernelChoice = KernelChoice::PREWITT;
    } else if (ui->robertsRadioButton->isChecked()) {
        kernelChoice = KernelChoice::ROBERTS;
    } else {
        QMessageBox::warning(this, "Warning", "Select an edge detection method!");
        return;
    }

    // 2. Determine whether to apply thresholding (binary or grayscale output)
    bool applyThreshold = ui->binaryRadioButton->isChecked();
    double thresholdValue = ui->thresholdSpinBox->value(); // Read from UI

    // 3. Determine padding choice
    PaddingChoice paddingChoice;
    if (ui->noPaddingRadioButton->isChecked()) {
        paddingChoice = PaddingChoice::NONE;
    } else if (ui->zeroPaddingRadioButton->isChecked()) {
        paddingChoice = PaddingChoice::ZERO;
    } else if (ui->replicateRadioButton->isChecked()) {
        paddingChoice = PaddingChoice::REPLICATE;
    } else if (ui->reflectRadioButton->isChecked()) {
        paddingChoice = PaddingChoice::REFLECT;
    } else {
        QMessageBox::warning(this, "Warning", "Select a padding method!");
        return;
    }

    // 4. Store the current state for undo
    previousImage = resultImage;

    // 5. Apply edge detection

    gradientEdgeDetection(previousImage, resultImage, kernelChoice, paddingChoice, applyThreshold, thresholdValue);

    // 6. Update the result display
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Operation completed.";

}


void MainWindow::on_thresholdSlider_valueChanged(int value)
{
    ui->thresholdSpinBox->blockSignals(true);
    ui->thresholdSpinBox->setValue(value);
    ui->thresholdSpinBox->blockSignals(false);
}


void MainWindow::on_thresholdSpinBox_valueChanged(int value)
{
    ui->thresholdSlider->blockSignals(true);
    ui->thresholdSlider->setValue(value);
    ui->thresholdSlider->blockSignals(false);
}


// Edge Detection: Laplacian --------------------------------------------------

void MainWindow::on_actionLaplacian_triggered()
{
    switchToPage(6);
}


void MainWindow::on_applyPB_clicked()
{
    if (!resultImage.buffer) {
        QMessageBox::warning(this, "Warning", "Please load an image first!");
        return;
    }

    double kernelSize = ui->kernelSpinBox->value();
    double sigma = ui->sigmaDoubleSpinBox->value();
    previousImage = resultImage;

    if(ui->gaussianRadioButton->isChecked()) {
        qDebug() << "Applying Gaussian filter...";
        gaussianFilter(previousImage, resultImage, kernelSize, sigma);
        qDebug() << "Gaussian Filter Applied with Kernel Size: " << kernelSize << "and sigma: " << sigma;
    }

    if (ui->basicLaplacianRadioButton->isChecked()) {
        qDebug() << "Applying Basic Laplacian...";
        highpassFilter(previousImage, resultImage, 1);
        qDebug() << "Basic Laplacian Applied";
    } else if (ui->fullLaplacianRadioButton->isChecked()) {
        qDebug() << "Applying Full Laplacian...";
        highpassFilter(previousImage, resultImage, 2);
        qDebug() << "Full Laplacian Applied";
    } else if (ui->basicInvertedLaplacianRadioButton->isChecked()) {
        qDebug() << "Applying Basic Inverted Laplacian...";
        highpassFilter(previousImage, resultImage, 3);
        qDebug() << "Basic Inverted Laplacian Applied";
    } else if (ui->fullInvertedLaplacianRadioButton->isChecked()) {
        qDebug() << "Applying Full Inverted Laplacian...";
        highpassFilter(previousImage, resultImage, 4);
        qDebug() << "Full Inverted Laplacian Applied";
    }

    updateImageDisplay(resultImage, ui->ResultWindowLabel);
    qDebug() << "Operation completed.";

}



void MainWindow::on_kernelHSlider_valueChanged(int value)
{
    ui->kernelSpinBox->blockSignals(true);
    ui->kernelSpinBox->setValue(value);
    ui->kernelSpinBox->blockSignals(false);
}

void MainWindow::on_kernelSpinBox_valueChanged(int value)
{
    ui->kernelHSlider->blockSignals(true);
    ui->kernelHSlider->setValue(value);
    ui->kernelHSlider->blockSignals(false);
}

// Edge Detection: Canny



void MainWindow::on_actionCanny_triggered()
{
    switchToPage(7);
}


void MainWindow::on_applyPBCanny_clicked()
{
    if (!resultImage.buffer) {
        QMessageBox::warning(this, "Warning", "Please load an image first!");
        return;
    }

    int kernelSize = ui->kernelSizeSpinBox_2->value();
    double sigma = ui->sigmaSpinBox->value();
    double lowThreshold = ui->lowThresholdSpinBox->value();
    double highThreshold = ui->highThresholdSpinBox->value();

    PaddingChoice paddingChoice;
    QString selectedPadding = ui->comboBoxPadding->currentText();

    if (selectedPadding == "None") {
        paddingChoice = PaddingChoice::NONE;
    } else if (selectedPadding == "Zero") {
        paddingChoice = PaddingChoice::ZERO;
    } else if (selectedPadding == "Reflect") {
        paddingChoice = PaddingChoice::REFLECT;
    } else if (selectedPadding == "Replicate") {
        paddingChoice = PaddingChoice::REPLICATE;
    } else {
        QMessageBox::warning(this, "Warning", "Invalid padding selection!");
        return;
    }

    // Store current image for undo
    previousImage = resultImage;

    qDebug() << "Performing Canny Edge Detection...";
    cannyEdgeDetection(previousImage, resultImage, lowThreshold, highThreshold, kernelSize, sigma, paddingChoice);
    qDebug() << "Canny Edge Detection Completed";
    updateImageDisplay(resultImage, ui->ResultWindowLabel);
}

