using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Hesternal.MathTool.Cpp;

namespace Hesternal.MathTool;

/*
internal sealed class MathTool
{
    private readonly DirectoryInfo m_privateDir;
    private readonly DirectoryInfo m_publicDir;

    private readonly List<VectorType> m_vectorTypes = new();
    private readonly List<MatrixType> m_matrixTypes = new();


    private MathTool(DirectoryInfo privateDir, DirectoryInfo publicDir)
    {
        m_privateDir = privateDir;
        m_publicDir = publicDir;
    }


    public static void Generate(DirectoryInfo privateDir, DirectoryInfo publicDir)
    {
        MathTool mathTool = new(privateDir, publicDir);
        mathTool._Generate();
    }


    private void _Generate()
    {
        _AddVector(CppSystemType.int32, 2);
        _AddVector(CppSystemType.float32, 2);
        _AddVector(CppSystemType.float32, 3);
        _AddVector(CppSystemType.float32, 4);

        _AddMatrix(CppSystemType.float32, 3, 3);
        _AddMatrix(CppSystemType.float32, 4, 4);

        _GenerateTypes();
        _GenerateAdditionalModules();
        _GenerateMathModule();
    }

    private void _AddVector(CppSystemType cppSystemType, int dimension)
    {
        if (dimension < 1 || dimension > 4)
        {
            throw new NotSupportedException();
        }

        // TODO(v.matushkin): Column or row vector?
        VectorType vectorType = VectorType.CreateColumnVector(cppSystemType, dimension);
        m_vectorTypes.Add(vectorType);
    }

    private void _AddMatrix(CppSystemType cppSystemType, int rows, int columns)
    {
        if (rows < 2 || rows > 4 || columns < 2 || columns > 4)
        {
            throw new NotSupportedException();
        }

        MatrixType matrixType = MatrixType.Create(cppSystemType, rows, columns);
        m_matrixTypes.Add(matrixType);
    }

    private void _GenerateTypes()
    {
        foreach (VectorType vectorType in m_vectorTypes)
        {
            string filePath = _MakeInternalPartitionFilePath(m_privateDir, vectorType.Name);
            string moduleName = _MakeInternalPartitionName(vectorType.Name);

            VectorGenerator.Generate(filePath, moduleName, vectorType);
        }

        foreach (MatrixType matrixType in m_matrixTypes)
        {
            string filePath = _MakeInternalPartitionFilePath(m_privateDir, matrixType.Name);
            string moduleName = _MakeInternalPartitionName(matrixType.Name);

            MatrixGenerator.Generate(filePath, moduleName, matrixType);
        }
    }

    private void _GenerateAdditionalModules()
    {
        {
            string filePath = _MakeInternalPartitionFilePath(m_privateDir, VectorFunctionsGenerator.InternalPartitionName);
            string moduleName = _MakeInternalPartitionName(VectorFunctionsGenerator.InternalPartitionName);
            VectorFunctionsGenerator.Generate(filePath, moduleName, m_vectorTypes);
        }
        {
            string filePath = _MakeInternalPartitionFilePath(m_privateDir, MatrixMulGenerator.InternalPartitionName);
            string moduleName = _MakeInternalPartitionName(MatrixMulGenerator.InternalPartitionName);
            MatrixMulGenerator.Generate(filePath, moduleName, m_vectorTypes.Concat(m_matrixTypes.Cast<MathType>()));
        }
    }

    private void _GenerateMathModule()
    {
        List<string> internalPartitions = new(MathToolSettings.HandWrittenMathInternalPartitions);
        internalPartitions.AddRange(m_vectorTypes.Select(vectorType => vectorType.Name));
        internalPartitions.AddRange(m_matrixTypes.Select(matrixType => matrixType.Name));
        internalPartitions.Sort();

        StringBuilder code = new();

        code.AppendLine(MathToolSettings.AutoGenComment)
            .AppendLine("export module " + MathToolSettings.MathModuleName + ";")
            .AppendLine();

        foreach (string internalPartition in internalPartitions)
        {
            code.Append("export import :").Append(internalPartition).AppendLine(";");
        }

        File.WriteAllText(_MakeModuleFilePath(m_publicDir, "Math"), code.ToString());
    }


    private static string _MakeInternalPartitionName(string internalPartition)
    {
        return $"{MathToolSettings.MathModuleName}:{internalPartition}";
    }

    private static string _MakeInternalPartitionFilePath(DirectoryInfo outputDir, string internalPartition)
    {
        return Path.Join(outputDir.FullName, "Math-" + internalPartition + ".ixx");
    }

    private static string _MakeModuleFilePath(DirectoryInfo outputDir, string module)
    {
        return Path.Join(outputDir.FullName, module + ".ixx");
    }
}
*/
