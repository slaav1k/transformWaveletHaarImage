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


void MainWindow::on_actionDo_triggered()
{
    if (_imageOrigin.isNull()) {
        return;
    }
    _imageGray = convertToGray(_imageOrigin);
//    _imageGray = _imageOrigin;

    _arrImageGray =  convertQImageToArrImage(_imageGray);
//    debugArrImage(_arrImageGray);


    ArrImage afterTransforme = ApplyHaarTransform(_arrImageGray);
//    debugArrImage(afterTransforme);

    c11.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d11.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d12.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));
    d13.resize(afterTransforme.size() / 2, vector<double>(afterTransforme.size() / 2));

    splitMatrix(afterTransforme, c11, d11, d12, d13);
    drawImageAfterTransform("WT ITER 1", c11, d11, d12, d13);

    ArrImage afterTransforme2 = ApplyHaarTransform(c11);

    c22.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d21.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d22.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));
    d23.resize(afterTransforme2.size() / 2, vector<double>(afterTransforme2.size() / 2));

    splitMatrix(afterTransforme2, c22, d21, d22, d23);
    drawImageAfterTransform("WT ITER 2", c22, d21, d22, d23);

}

void MainWindow::on_actionDo2_triggered()
{
    if (c11.size() == 0) {
        return;
    }
    // после обнулений, сбоорка в общую матрицу
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
    QLabel *dataLabel = new QLabel("c11 | d11    c22 | d21\n"
                                   "d12 | d13   d22 | d23", dialog);

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
        drawImageAfterTransform("FILTER", c11, d11, d12, d13);
        drawImageAfterTransform("FILTER 2", c22, d21, d22, d23);
        dialog->close();
    });

    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::close);

    // Показываем диалоговое окно
    dialog->exec();
}


