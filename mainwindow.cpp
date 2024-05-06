#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <cmath>
#include <QVBoxLayout>
#include <iostream>
#include <QPainter>
#include <QLineEdit>
#include <QTextEdit>
#include <cstdlib>
#include <ctime>
#include <QCheckBox>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Images (*.bmp *.png)");
    if (!fileName.isEmpty()) {
        QDialog *imageDialog = new QDialog(this); // Создаем новый объект окна
        imageDialog->setWindowTitle("ORIGIN IMAGE");
        QLabel *newLabel = new QLabel(imageDialog); // Создаем новый QLabel на окне

        newLabel->setPixmap(QPixmap(fileName)); // Устанавливаем изображение в метку
        newLabel->resize(newLabel->pixmap()->size()); // Меняем размер метки под размер изображения

        _imageOrigin = newLabel->pixmap()->toImage();

        // Проверяем, является ли изображение квадратным
        if (_imageOrigin.width() != _imageOrigin.height()) {
            QMessageBox::critical(this, "Ошибка", "Выбранное изображение не является квадратным.");
            delete imageDialog; // Удаляем диалоговое окно
            _imageOrigin = QImage();
            return;
        }

        QVBoxLayout *layout = new QVBoxLayout(imageDialog); // Создаем вертикальный макет для окна
        layout->addWidget(newLabel); // Добавляем метку на макет
        imageDialog->setLayout(layout); // Устанавливаем макет в качестве макета окна

        imageDialog->show(); // Показываем окно
        _imageGray = convertToGray(_imageOrigin);

        _arrImageGray =  convertQImageToArrImage(_imageGray);
    }
}


void MainWindow::on_actionSave_triggered()
{
    if (_resultImage.isNull()) {
            return;
        }
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "BMP (*.bmp);;PNG (*.png)");

    if (!filePath.isEmpty()) {
        bool saved = _resultImage.save(filePath);

        if (saved) {
            qDebug() << "Изображение успешно сохранено в файл:" << filePath;
        } else {
            qDebug() << "Ошибка при сохранении изображения.";
        }
    }
}


QImage MainWindow::convertToGray(const QImage& image) {
    return image.convertToFormat(QImage::Format_Grayscale8);
}

ArrImage MainWindow::convertQImageToArrImage(const QImage& image) {
    int width = image.width();
    int height = image.height();
    ArrImage arrImage(height, vector<double>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = image.pixel(x, y);
            arrImage[y][x] = double(qRed(pixel));
        }
    }
    return arrImage;
}

//QImage MainWindow::convertArrImageToQImage(const ArrImage& arrImage) {
//    int height = arrImage.size();
//    int width = arrImage[0].size();

//    // Находим минимальное и максимальное значения в матрице
//    double minVal = std::numeric_limits<double>::max();
//    double maxVal = std::numeric_limits<double>::min();
//    for (const auto& row : arrImage) {
//        for (double val : row) {
//            if (val < minVal) minVal = val;
//            if (val > maxVal) maxVal = val;
//        }
//    }

//    // Создаем QImage с соответствующими размерами
//    QImage qImage(width, height, QImage::Format_RGB32);

//    // Нормализуем значения и устанавливаем пиксели в QImage
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            double value = arrImage[y][x];
//            int intValue = static_cast<int>((value - minVal) / (maxVal - minVal) * 255);
//            qImage.setPixel(x, y, qRgb(intValue, intValue, intValue));
//        }
//    }

//    return qImage;
//}


QImage MainWindow::convertArrImageToQImage(const ArrImage& arrImage) {
    int height = arrImage.size();
    int width = arrImage[0].size();
    QImage image(width, height, QImage::Format_RGB32);
    double minVal = numeric_limits<double>::max();
    double maxVal = numeric_limits<double>::min();

    for (const auto& row : arrImage) {
        for (const auto& pixel : row) {
            double val = abs(pixel);
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }
    }


   for (int y = 0; y < height; ++y) {
       for (int x = 0; x < width; ++x) {
           // Получаем комплексное значение из ArrImage
           double value = arrImage[y][x];
           // Определяем цвет пикселя на основе вещественной и мнимой частей комплексного значения
           double val = abs(value);
           int color = static_cast<int>((val - minVal) / (maxVal - minVal) * 255);
           // Устанавливаем цвет пикселя
           image.setPixel(x, y, qRgb(color, color, color));
       }
   }
    return image;
}

vector<double> MainWindow::DirectTransform(vector<double> a, double coef)
{
    if (a.size() == 1)
        return a;

    vector<double> part1;
    vector<double> part2;

    for (size_t j = 0; j < a.size() - 1; j += 2)
    {
//        part1.push_back((a[j] + a[j + 1]) / sqrt(2));
//        part2.push_back((a[j] - a[j + 1]) / sqrt(2));
        part1.push_back((a[j] + a[j + 1]) / coef);
        part2.push_back((a[j] - a[j + 1]) / coef);
    }

    part1.insert(part1.end(), part2.begin(), part2.end());
    return part1;
}


vector<double> MainWindow::InverseTransform(vector<double> a, double coef)
{
    if (a.size() == 1)
        return a;

    vector<double> part1;
    vector<double> part2;

    for (size_t i = a.size() / 2; i < a.size(); i += 1)
    {
        part2.push_back(a[i]);
    }

    for (size_t i = 0; i < a.size() / 2; i++)
    {
//        part1.push_back((part2[i] + a[i]) / sqrt(2));
//        part1.push_back((a[i] - part2[i]) / sqrt(2));
        part1.push_back((part2[i] + a[i]) / coef);
        part1.push_back((a[i] - part2[i]) / coef);
    }

    return part1;
}

ArrImage MainWindow::TransposeMatrix(const ArrImage& matrix) {
    int rows = matrix.size();
    int columns = matrix[0].size();

    ArrImage transposedMatrix(columns, vector<double>(rows));

    for (int j = 0; j < columns; j++) {
        for (int i = 0; i < rows; i++) {
            transposedMatrix[j][i] = matrix[i][j];
        }
    }

    return transposedMatrix;
}


ArrImage MainWindow::ApplyHaarTransform(ArrImage matrix)
{
    ArrImage transformedMatrix;

    for (vector<double>&row : matrix)
    {
        vector<double> transformedRow = DirectTransform(row, sqrt(2));
        transformedMatrix.push_back(transformedRow);
    }

    ArrImage transposedMatrix = TransposeMatrix(transformedMatrix);

    transformedMatrix.clear();
    for (vector<double>&column : transposedMatrix)
    {
        vector<double> transformedColumn = DirectTransform(column, -1.0 * sqrt(2));
        transformedMatrix.push_back(transformedColumn);
    }

    transformedMatrix = TransposeMatrix(transformedMatrix);
    return transformedMatrix;
}


ArrImage MainWindow::ApplyInverseHaarTransform(ArrImage matrix)
{
    ArrImage transposedMatrix = TransposeMatrix(matrix);
    ArrImage transformedMatrix;
    for (vector<double>&row : transposedMatrix)
    {
        vector<double> transformedRow = InverseTransform(row, -1.0 * sqrt(2));
        transformedMatrix.push_back(transformedRow);
    }

    transformedMatrix = TransposeMatrix(transformedMatrix);

    ArrImage finalTransformedMatrix;
    for (vector<double>&row : transformedMatrix)
    {
        vector<double> finalTransformedRow = InverseTransform(row, sqrt(2));
        finalTransformedMatrix.push_back(finalTransformedRow);
    }
    return finalTransformedMatrix;
}




void MainWindow::debugArrImage(ArrImage a)
{
    int size = a.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout << a[i][j] << " ";
        cout << endl;
    }
    cout << endl << endl;

}

void MainWindow::splitMatrix(const ArrImage& matrix, ArrImage& topLeft, ArrImage& topRight, ArrImage& bottomLeft, ArrImage& bottomRight) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    // Заполняем каждую часть матрицы
    for (int i = 0; i < rows / 2; ++i) {
        for (int j = 0; j < cols / 2; ++j) {
            topLeft[i][j] = matrix[i][j];
            topRight[i][j] = matrix[i][j + cols / 2];
            bottomLeft[i][j] = matrix[i + rows / 2][j];
            bottomRight[i][j] = matrix[i + rows / 2][j + cols / 2];
        }
    }
}

ArrImage MainWindow::mergeMatrices(const ArrImage& topLeft, const ArrImage& topRight, const ArrImage& bottomLeft, const ArrImage& bottomRight) {
    int rows = topLeft.size() + bottomLeft.size(); // Высота новой матрицы
    int cols = topLeft[0].size() + topRight[0].size(); // Ширина новой матрицы

    ArrImage mergedMatrix(rows, vector<double>(cols));

    // Заполняем новую матрицу значениями из четырех матриц
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i < rows / 2 && j < cols / 2) {
                mergedMatrix[i][j] = topLeft[i][j]; // Заполняем верхнюю левую четверть
            } else if (i < rows / 2) {
                mergedMatrix[i][j] = topRight[i][j - cols / 2]; // Заполняем верхнюю правую четверть
            } else if (j < cols / 2) {
                mergedMatrix[i][j] = bottomLeft[i - rows / 2][j]; // Заполняем нижнюю левую четверть
            } else {
                mergedMatrix[i][j] = bottomRight[i - rows / 2][j - cols / 2]; // Заполняем нижнюю правую четверть
            }
        }
    }

    return mergedMatrix;
}

void MainWindow::drawImageAfterTransform(QString title, const ArrImage& topLeft, const ArrImage& topRight, const ArrImage& bottomLeft, const ArrImage& bottomRight)
{
    // отрисовка прямого пробразования Хаара
    QDialog *dialog = new QDialog(this);
    QLabel *label = new QLabel(dialog);
    dialog->setWindowTitle(title);
    dialog->setMinimumWidth(_arrImageGray.size());
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    label->resize(topLeft.size() * 2, topLeft.size() * 2);

    // Создание QPixmap для отрисовки C11, D11, D12, D13
    QPixmap combinedPixmap(label->size());
    combinedPixmap.fill(Qt::white);

    QPainter painter(&combinedPixmap);
    int xOffset = 0;
    int yOffset = 0;

    // Отрисовка C11
    painter.drawImage(xOffset, yOffset, convertArrImageToQImage(topLeft));
    xOffset += topLeft.size();

    // Отрисовка D11
    painter.drawImage(xOffset, yOffset, convertArrImageToQImage(topRight));
    yOffset += topRight.size();
    xOffset = 0;

    // Отрисовка D12
    painter.drawImage(xOffset, yOffset, convertArrImageToQImage(bottomLeft));
    xOffset += bottomLeft.size();

    // Отрисовка D13
    painter.drawImage(xOffset, yOffset, convertArrImageToQImage(bottomRight));

    label->setPixmap(combinedPixmap);
    layout->addWidget(label);
    dialog->setLayout(layout);
    dialog->show();
}


void MainWindow::choseQuadrantForTransform() {
    if (c11.size() == 0) {
        return;
    }
    // Создаем новое диалоговое окно
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Chose quadrant");

    // Создаем метки
    QLabel *enterTextLabel = new QLabel("Введите следущий квадрант для разложения.", dialog);
    QLabel *dataLabel = new QLabel("c  | d1\n"
                                   "d2 | d3", dialog);

    // Создаем текстовое поле
    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setFixedHeight(50);

    // Создаем кнопки "ОК" и "Отмена"
    QPushButton *okButton = new QPushButton("OK", dialog);
    QPushButton *cancelButton = new QPushButton("Отмена", dialog);

    // Устанавливаем макет для диалогового окна
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(enterTextLabel);
    layout->addWidget(dataLabel);
    layout->addWidget(textEdit);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);
    dialog->setLayout(layout);

    // Подключаем сигналы и слоты для кнопок
    connect(okButton, &QPushButton::clicked, [=]() {
        dialog->close();
        ansChoseQuadrant =  textEdit->toPlainText();
    });

    connect(cancelButton, &QPushButton::clicked, [=]()
    {
        textEdit->setText("c");
        dialog->close();
        ansChoseQuadrant = textEdit->toPlainText();
    });


    // Показываем диалоговое окно
    dialog->exec();

}


void MainWindow::on_actionDo_triggered()
{
    if (_imageOrigin.isNull()) {
        return;
    }

//    debugArrImage(_arrImageGray);
//    on_action_2_triggered();

    i1_2 = ""; i2_3 = ""; i3_4 = "";
    ArrImage afterTransforme = ApplyHaarTransform(_arrImageGray);
//    debugArrImage(afterTransforme);

    c11.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d11.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d12.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d13.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));

    splitMatrix(afterTransforme, c11, d11, d12, d13);
    drawImageAfterTransform("WT ITER 1", c11, d11, d12, d13);

    choseQuadrantForTransform();
    i1_2 = ansChoseQuadrant;

    ArrImage afterTransforme2;
    if (i1_2.contains("d1"))
        afterTransforme2 = ApplyHaarTransform(d11);
    else if (i1_2.contains("d2"))
        afterTransforme2 = ApplyHaarTransform(d12);
    else if (i1_2.contains("d3"))
        afterTransforme2 = ApplyHaarTransform(d13);
    else
        afterTransforme2 = ApplyHaarTransform(c11);

    c22.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d21.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d22.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d23.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));

    splitMatrix(afterTransforme2, c22, d21, d22, d23);
    drawImageAfterTransform("WT ITER 2 by "  + i1_2, c22, d21, d22, d23);


    choseQuadrantForTransform();
    i2_3 = ansChoseQuadrant;

    ArrImage afterTransforme3;
    if (i2_3.contains("d1"))
        afterTransforme3 = ApplyHaarTransform(d21);
    else if (i2_3.contains("d2"))
        afterTransforme3 = ApplyHaarTransform(d22);
    else if (i2_3.contains("d3"))
        afterTransforme3 = ApplyHaarTransform(d23);
    else
        afterTransforme3 = ApplyHaarTransform(c22);


    c33.resize(afterTransforme3.size() / 2, vector<double>(afterTransforme3.size() / 2));
    d31.resize(afterTransforme3.size() / 2, vector<double>(afterTransforme3.size() / 2));
    d32.resize(afterTransforme3.size() / 2, vector<double>(afterTransforme3.size() / 2));
    d33.resize(afterTransforme3.size() / 2, vector<double>(afterTransforme3.size() / 2));

    splitMatrix(afterTransforme3, c33, d31, d32, d33);
    drawImageAfterTransform("WT ITER 3 by " + i2_3, c33, d31, d32, d33);


    choseQuadrantForTransform();
        i3_4 = ansChoseQuadrant;

        ArrImage afterTransforme4;
        if (i3_4.contains("d1"))
            afterTransforme4 = ApplyHaarTransform(d31);
        else if (i3_4.contains("d2"))
            afterTransforme4 = ApplyHaarTransform(d32);
        else if (i3_4.contains("d3"))
            afterTransforme4 = ApplyHaarTransform(d33);
        else
            afterTransforme4 = ApplyHaarTransform(c33);


    c44.resize(afterTransforme4.size() / 2, vector<double>(afterTransforme4.size() / 2));
    d41.resize(afterTransforme4.size() / 2, vector<double>(afterTransforme4.size() / 2));
    d42.resize(afterTransforme4.size() / 2, vector<double>(afterTransforme4.size() / 2));
    d43.resize(afterTransforme4.size() / 2, vector<double>(afterTransforme4.size() / 2));

    splitMatrix(afterTransforme4, c44, d41, d42, d43);
    drawImageAfterTransform("WT ITER 4 by " + i3_4, c44, d41, d42, d43);

}

void MainWindow::on_actionDo2_triggered()
{
    if (c11.size() == 0) {
        return;
    }
    // после обнулений, сбоорка в общую матрицу
    if (i3_4.contains("d1"))
        d31 = ApplyInverseHaarTransform(mergeMatrices(c44, d41, d42, d43));
    else if (i3_4.contains("d2"))
        d32 = ApplyInverseHaarTransform(mergeMatrices(c44, d41, d42, d43));
    else if (i3_4.contains("d3"))
        d33 = ApplyInverseHaarTransform(mergeMatrices(c44, d41, d42, d43));
    else
        c33 = ApplyInverseHaarTransform(mergeMatrices(c44, d41, d42, d43));

    if (i2_3.contains("d1"))
        d21 = ApplyInverseHaarTransform(mergeMatrices(c33, d31, d32, d33));
    else if (i2_3.contains("d2"))
        d22 = ApplyInverseHaarTransform(mergeMatrices(c33, d31, d32, d33));
    else if (i2_3.contains("d3"))
        d23 = ApplyInverseHaarTransform(mergeMatrices(c33, d31, d32, d33));
    else
        c22 = ApplyInverseHaarTransform(mergeMatrices(c33, d31, d32, d33));

    if (i1_2.contains("d1"))
        d11 = ApplyInverseHaarTransform(mergeMatrices(c22, d21, d22, d23));
    else if (i1_2.contains("d2"))
        d12 = ApplyInverseHaarTransform(mergeMatrices(c22, d21, d22, d23));
    else if (i1_2.contains("d3"))
        d13 = ApplyInverseHaarTransform(mergeMatrices(c22, d21, d22, d23));
    else
        c11 = ApplyInverseHaarTransform(mergeMatrices(c22, d21, d22, d23));

    ArrImage afterTransforme2 = mergeMatrices(c11, d11, d12, d13);


    ArrImage afterInvTransforme = ApplyInverseHaarTransform(afterTransforme2);
    QImage afterInvTransformeImage = convertArrImageToQImage(afterInvTransforme);
    _resultImage = afterInvTransformeImage;

    QDialog *imageDialog = new QDialog(this);
    imageDialog->setWindowTitle("IWT");
    QLabel *newLabel = new QLabel(imageDialog); // Создаем QLabel для отображения изображения
    newLabel->setPixmap(QPixmap::fromImage(afterInvTransformeImage)); // Устанавливаем изображение в QLabel

    // Устанавливаем макет для диалогового окна и добавляем QLabel
    QVBoxLayout *layout = new QVBoxLayout(imageDialog);
    layout->addWidget(newLabel);
    imageDialog->setLayout(layout);

    // Показываем диалоговое окно
    imageDialog->show();
}


void MainWindow::resetToZero(ArrImage& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j < arr[0].size(); ++j) {
            arr[i][j] = 0.0;
        }
    }
}

void MainWindow::on_actionDo3_triggered()
{
    if (c11.size() == 0) {
        return;
    }
    // Создаем новое диалоговое окно
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter");

    // Создаем метки
    QLabel *enterTextLabel = new QLabel("Введите какие представления обнулить (через пробел).", dialog);
    QLabel *dataLabel = new QLabel("c11 | d11    c22 | d21    c33 | d31    c44 | d41\n"
                                   "d12 | d13   d22 | d23    d32 | d33    d42 | d43", dialog);

    // Создаем текстовое поле
    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setFixedHeight(50);

    // Создаем кнопки "ОК" и "Отмена"
    QPushButton *okButton = new QPushButton("OK", dialog);
    QPushButton *cancelButton = new QPushButton("Отмена", dialog);

    // Устанавливаем макет для диалогового окна
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(enterTextLabel);
    layout->addWidget(dataLabel);
    layout->addWidget(textEdit);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);
    dialog->setLayout(layout);

    // Подключаем сигналы и слоты для кнопок
    connect(okButton, &QPushButton::clicked, [=]() {
        QString text = textEdit->toPlainText();
        if (text.contains("c11")) {
            resetToZero(c11);
        }
        if (text.contains("d11")) {
            resetToZero(d11);
        }
        if (text.contains("d12")) {
            resetToZero(d12);
        }
        if (text.contains("d13")) {
            resetToZero(d13);
        }
        if (text.contains("c22")) {
            resetToZero(c22);
        }
        if (text.contains("d21")) {
            resetToZero(d21);
        }
        if (text.contains("d22")) {
            resetToZero(d22);
        }
        if (text.contains("d23")) {
            resetToZero(d23);
        }
        if (text.contains("c33")) {
            resetToZero(c33);
        }
        if (text.contains("d31")) {
            resetToZero(d31);
        }
        if (text.contains("d32")) {
            resetToZero(d32);
        }
        if (text.contains("d33")) {
            resetToZero(d33);
        }
        if (text.contains("c44")) {
            resetToZero(c44);
        }
        if (text.contains("d41")) {
            resetToZero(d41);
        }
        if (text.contains("d42")) {
            resetToZero(d42);
        }
        if (text.contains("d43")) {
            resetToZero(d43);
        }
        drawImageAfterTransform("FILTER", c11, d11, d12, d13);
        drawImageAfterTransform("FILTER 2", c22, d21, d22, d23);
        drawImageAfterTransform("FILTER 3", c33, d31, d32, d33);
        drawImageAfterTransform("FILTER 4", c44, d41, d42, d43);
        dialog->close();
    });

    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::close);

    // Показываем диалоговое окно
    dialog->exec();
}



void MainWindow::on_action_2_triggered()
{
    if (_imageOrigin.isNull()) {
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Noises");

    // Создаем элементы управления для выбора параметров
    QLineEdit *minIntensityEdit = new QLineEdit(QString::number(0), dialog);
    QLineEdit *maxIntensityEdit = new QLineEdit(QString::number(180), dialog);
    QLineEdit *lineCountEdit = new QLineEdit(QString::number(10), dialog);
    QCheckBox *verticalLinesCheckBox = new QCheckBox("Vertical Lines", dialog);
    QCheckBox *horizontalLinesCheckBox = new QCheckBox("Horizontal Lines", dialog);

    // Создаем кнопки "Применить" и "Отмена"
    QPushButton *applyButton = new QPushButton("Apply", dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", dialog);

    // Создаем макет для размещения элементов управления
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(new QLabel("Minimum Intensity:", dialog));
    layout->addWidget(minIntensityEdit);
    layout->addWidget(new QLabel("Maximum Intensity:", dialog));
    layout->addWidget(maxIntensityEdit);
    layout->addWidget(new QLabel("Line Count:", dialog));
    layout->addWidget(lineCountEdit);
    layout->addWidget(verticalLinesCheckBox);
    layout->addWidget(horizontalLinesCheckBox);
    layout->addWidget(applyButton);
    layout->addWidget(cancelButton);
    dialog->setLayout(layout);

    // Соединяем кнопки с слотами для их обработки
    connect(applyButton, &QPushButton::clicked, [=]() {
        double minIntensity = minIntensityEdit->text().toDouble();
        double maxIntensity = maxIntensityEdit->text().toDouble();
        int lineCount = lineCountEdit->text().toInt();
        bool verticalLines = verticalLinesCheckBox->isChecked();
        bool horizontalLines = horizontalLinesCheckBox->isChecked();

        if (minIntensity > maxIntensity)
        {
            double tmp = minIntensity;
            minIntensity = maxIntensity;
            maxIntensity = tmp;
        }

        if (maxIntensity > 255)
            maxIntensity = 255;
        if (minIntensity < 0)
            minIntensity = 0;

        if (horizontalLines == true or verticalLines == true)
        {
            srand(time(nullptr)); // Инициализация генератора случайных чисел
            _arrImageGray =  convertQImageToArrImage(_imageGray);

            int size = _arrImageGray.size();

            int step = size / lineCount; // Вычисляем шаг между линиями

            for (int x = 0; x < size; x += step) {
                double intensity = minIntensity + (maxIntensity - minIntensity) * (rand() / (double)RAND_MAX);
                // Задаем яркость для каждого пикселя в вертикальной линии
                for (int y = 0; y < size; ++y) {
                    if (verticalLines == true)
                        _arrImageGray[y][x] = intensity;
                    else
                        _arrImageGray[x][y] = intensity;
                }
            }


            QImage imageWithLines = convertArrImageToQImage(_arrImageGray);

            QDialog *imageDialog = new QDialog(this);
            imageDialog->setWindowTitle("IMAGE WITH LINES");
            QLabel *newLabel = new QLabel(imageDialog); // Создаем QLabel для отображения изображения
            newLabel->setPixmap(QPixmap::fromImage(imageWithLines)); // Устанавливаем изображение в QLabel

            // Устанавливаем макет для диалогового окна и добавляем QLabel
            QVBoxLayout *layout = new QVBoxLayout(imageDialog);
            layout->addWidget(newLabel);
            imageDialog->setLayout(layout);

            // Показываем диалоговое окно
            imageDialog->show();
        }

        dialog->close(); // Закрываем диалоговое окно после применения параметров
    });

    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::close);

    dialog->exec(); // Отображаем диалоговое окно

}
