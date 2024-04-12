using System;
using System.Collections.Generic;


static List<Double> DirectTransform(List<Double> SourceList)
{
  if (SourceList.Count == 1)
    return SourceList;

  List<Double> Part1 = new List<Double>();
  List<Double> Part2 = new List<Double>();

  for (int j = 0; j < SourceList.Count - 1; j += 2)
  {
    Part1.Add((SourceList[j] + SourceList[j + 1]) / Math.Sqrt(2.0));
    Part2.Add((SourceList[j] - SourceList[j + 1]) / Math.Sqrt(2.0));
  }

  Part1.AddRange(Part2);

  return Part1;
}

static List<Double> InverseTransform(List<Double> SourceList)
{
  if (SourceList.Count == 1)
    return SourceList;

  List<Double> Part1 = new List<Double>();
  List<Double> Part2 = new List<Double>();

  for (int i = SourceList.Count / 2; i < SourceList.Count; i++)
    Part2.Add(SourceList[i]);

  List<Double> SecondPart = Part2;

  for (int i = 0; i < SourceList.Count / 2; i++)
  {
    Part1.Add((SecondPart[i] + SourceList[i]) / Math.Sqrt(2.0));
    Part1.Add((SourceList[i] - SecondPart[i]) / Math.Sqrt(2.0));
  }

  return Part1;
}


static List<List<double>> ApplyHaarTransform(List<List<double>> matrix)
{
  List<List<double>> transformedMatrix = new List<List<double>>();

  // Применение к строкам
  foreach (var row in matrix)
  {
    List<double> transformedRow = DirectTransform(row);
    transformedMatrix.Add(transformedRow);
  }

  // Транспонирование матрицы
  List<List<double>> transposedMatrix = TransposeMatrix(transformedMatrix);

  // Применение к столбцам
  transformedMatrix.Clear();
  foreach (var column in transposedMatrix)
  {
    List<double> transformedColumn = DirectTransform(column);
    transformedMatrix.Add(transformedColumn);
  }

  // Транспонирование обратно для получения итоговой матрицы
  transformedMatrix = TransposeMatrix(transformedMatrix);

  return transformedMatrix;
}


static List<List<double>> ApplyInverseHaarTransform(List<List<double>> matrix)
{
  // Транспонирование матрицы
  List<List<double>> transposedMatrix = TransposeMatrix(matrix);

  // Применение InverseTransform к каждой строке после первого транспонирования
  List<List<double>> transformedMatrix = new List<List<double>>();
  foreach (var row in transposedMatrix)
  {
    List<double> transformedRow = InverseTransform(row);
    transformedMatrix.Add(transformedRow);
  }

  // Повторное транспонирование
  transformedMatrix = TransposeMatrix(transformedMatrix);

  // Применение InverseTransform к каждой строке после повторного транспонирования
  List<List<double>> finalTransformedMatrix = new List<List<double>>();
  foreach (var row in transformedMatrix)
  {
    List<double> finalTransformedRow = InverseTransform(row);
    finalTransformedMatrix.Add(finalTransformedRow);
  }

  return finalTransformedMatrix;
}

static List<List<double>> TransposeMatrix(List<List<double>> matrix)
{
  int rows = matrix.Count;
  int columns = matrix[0].Count;

  List<List<double>> transposedMatrix = new List<List<double>>();

  for (int j = 0; j < columns; j++)
  {
    List<double> transposedRow = new List<double>();
    for (int i = 0; i < rows; i++)
    {
      transposedRow.Add(matrix[i][j]);
    }
    transposedMatrix.Add(transposedRow);
  }

  return transposedMatrix;
}

static void PrintMatrix(List<List<double>> matrix)
{
  foreach (var row in matrix)
  {
    List<string> roundedRow = row.ConvertAll(value => Math.Round(value, 2).ToString());
    Console.WriteLine(string.Join(" ", roundedRow));
  }
}

static List<List<double>> ApplyHaarTransformOnlyRow(List<List<double>> matrix)
{
  List<List<double>> transformedMatrix = new List<List<double>>();

  foreach (var row in matrix)
  {
    List<double> transformedRow = DirectTransform(row);
    transformedMatrix.Add(transformedRow);
  }

  return transformedMatrix;
}

static List<List<double>> ApplyInverseHaarTransformOnlyColomn (List<List<double>> matrix)
{
  // Транспонирование матрицы
  List<List<double>> transposedMatrix = TransposeMatrix(matrix);

  // Применение InverseTransform к каждой строке
  List<List<double>> transformedMatrix = new List<List<double>>();
  foreach (var row in transposedMatrix)
  {
    List<double> transformedRow = InverseTransform(row);
    transformedMatrix.Add(transformedRow);
  }

  // Повторное транспонирование
  transformedMatrix = TransposeMatrix(transformedMatrix);

  return transformedMatrix;
}



List<double> list = new List<double> { 3, 7, 8, 10 };
Console.WriteLine(string.Join(" ", list));

List<double> ret = DirectTransform(list);
Console.WriteLine(string.Join(" ", ret));

List<double> retInv = InverseTransform(ret);
Console.WriteLine(string.Join(" ", retInv));


List<List<double>> matrix = new List<List<double>>
        {
            new List<double> {1, 2, 3, 4},
            new List<double> {4, 5, 6, 7},
            new List<double> {8, 9, 1, 2},
            new List<double> {3, 4, 5, 6}
        };

matrix = new List<List<double>>
        {
            new List<double> {1, 2, 3, 4},
            new List<double> {4, 3, 7, 8},
            new List<double> {6, 2, 1, 8},
            new List<double> {2, 5, 4, 7}
        };

Console.WriteLine();
Console.WriteLine("Исходная матрица");
Console.WriteLine();
PrintMatrix(matrix);

Console.WriteLine();
Console.WriteLine("Применение прямого вейвлет преобразования Хаара к строкам");
Console.WriteLine();
List<List<double>> matrix2_1 = ApplyHaarTransformOnlyRow(matrix);
PrintMatrix(matrix2_1);

Console.WriteLine();
Console.WriteLine("Применение прямого вейвлет преобразования Хаара к строкам и столбцам");
Console.WriteLine();
List<List<double>> matrix2 = ApplyHaarTransform(matrix);
PrintMatrix(matrix2);


Console.WriteLine();
Console.WriteLine("Применение обратного вейвлет преобразования Хаара к столбцам преобразованной матрицы");
Console.WriteLine();
List<List<double>> matrix3_1 = ApplyInverseHaarTransformOnlyColomn(matrix2);
PrintMatrix(matrix3_1);

Console.WriteLine();
Console.WriteLine("Применение обратного вейвлет преобразования Хаара к столбцам и строкам преобразованной матрицы");
Console.WriteLine();
List<List<double>> matrix3 = ApplyInverseHaarTransform(matrix2);
PrintMatrix(matrix3);