#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <complex>
#include <vector>
#include <string>


using namespace std;


namespace Ui {
class MainWindow;
}

typedef vector<vector<double> > ArrImage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionDo_triggered();

    void on_actionDo2_triggered();

    void on_actionDo3_triggered();

    void on_actionSave_triggered();

    void on_action_2_triggered();

private:
    Ui::MainWindow *ui;
    QImage _imageOrigin;
    QImage _imageGray;
    QImage _resultImage;

    QString ansChoseQuadrant;
    QString i1_2;
    QString i2_3;
    QString i3_4;

    ArrImage _arrImageGray;
    ArrImage c11;
    ArrImage d11;
    ArrImage d12;
    ArrImage d13;

    ArrImage c22;
    ArrImage d21;
    ArrImage d22;
    ArrImage d23;

    ArrImage c33;
    ArrImage d31;
    ArrImage d32;
    ArrImage d33;

    ArrImage c44;
    ArrImage d41;
    ArrImage d42;
    ArrImage d43;

    void debugArrImage(ArrImage a);
    void splitMatrix(const ArrImage& matrix, ArrImage& topLeft, ArrImage& topRight, ArrImage& bottomLeft, ArrImage& bottomRight);
    ArrImage mergeMatrices(const ArrImage& topLeft, const ArrImage& topRight, const ArrImage& bottomLeft, const ArrImage& bottomRight);
    void resetToZero(ArrImage& arr);
    void drawImageAfterTransform(QString title, const ArrImage& topLeft, const ArrImage& topRight, const ArrImage& bottomLeft, const ArrImage& bottomRight);

    ArrImage ApplyInverseHaarTransform(ArrImage matrix);
    ArrImage ApplyHaarTransform(ArrImage matrix);
    ArrImage TransposeMatrix(const ArrImage& matrix);
    vector<double> InverseTransform(vector<double> a, double coef);
    vector<double> DirectTransform(vector<double> a, double coef);

    QImage convertToGray(const QImage& image);
    ArrImage convertQImageToArrImage(const QImage& image);
    QImage convertArrImageToQImage(const ArrImage& arrImage);
    void choseQuadrantForTransform();



};

#endif // MAINWINDOW_H
