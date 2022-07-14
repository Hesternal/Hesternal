using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Copium.BuildTool.Core;


// [JsonConverter(typeof(FileItemJsonConverter))]
public sealed class FileItem
{
    [JsonInclude, JsonPropertyName("File")]
    private readonly FileInfo m_internal;
    private readonly int m_firstExtensionIndex;
    private readonly int m_lastExtensionIndex;

    [JsonIgnore] public string FullPath => m_internal.FullName;
    /// <summary>
    /// File name without last extension
    /// </summary>
    public ReadOnlySpan<char> Name => m_internal.Name.AsSpan(0, m_lastExtensionIndex);
    /// <summary>
    /// File name without any extensions
    /// </summary>
    public ReadOnlySpan<char> NameWithoutExtensions => m_internal.Name.AsSpan(0, m_firstExtensionIndex);
    /// <summary>
    /// File name with extensions
    /// </summary>
    [JsonIgnore] public string FileName => m_internal.Name;
    [JsonIgnore] public bool Exists => m_internal.Exists;
    [JsonIgnore] public DirectoryItem Directory
    {
        get
        {
            DirectoryInfo? fileDir = m_internal.Directory;
            if (fileDir == null)
            {
                throw new NullReferenceException($"File doesn't have a parent dir: {FullPath}");
            }
            return new DirectoryItem(fileDir);
        }
    }


    public FileItem(string filePath)
        : this(new FileInfo(filePath))
    {
    }

    public FileItem(FileInfo file)
    {
        m_internal = file;

        m_firstExtensionIndex = file.Name.IndexOf('.');
        if (m_firstExtensionIndex == -1)
        {
            m_firstExtensionIndex = file.Name.Length;
            m_lastExtensionIndex = file.Name.Length;
        }
        else
        {
            m_lastExtensionIndex = file.Name.LastIndexOf('.');
        }
    }


    public string GetRelativePath(DirectoryItem relativeTo)
    {
        return Path.GetRelativePath(relativeTo.FullPath, FullPath);
    }


    public TValue JsonDeserialize<TValue>()
    {
        JsonSerializerOptions options = new() { IncludeFields = true };
        TValue? value;

        using (FileStream fileStream = OpenRead())
        {
            value = JsonSerializer.Deserialize<TValue>(fileStream, options);
        }

        if (value == null)
        {
            throw new ApplicationException("Couldn't load json file: " + FullPath);
        }

        return value;
    }

    public void JsonSerialize<TValue>(TValue value)
    {
        JsonSerializerOptions options = new()
        {
            IncludeFields = true,
            WriteIndented = true,
        };

        using (FileStream fileStream = OpenWrite())
        {
            JsonSerializer.Serialize(fileStream, value, options);
        }
    }


    #region FileInfo Methods

    public void Delete() => m_internal.Delete();
    public void Refresh() => m_internal.Refresh();

    public FileStream OpenRead() => m_internal.OpenRead();
    public FileStream OpenWrite() => m_internal.Open(FileMode.Create, FileAccess.Write, FileShare.None);
    public StreamReader OpenReadText() => m_internal.OpenText();
    public StreamWriter OpenWriteText() => m_internal.CreateText();

    #endregion

    #region File methods

    public IEnumerable<string> ReadLines() => File.ReadLines(FullPath);

    public string ReadAllText() => File.ReadAllText(FullPath);

    public void WriteAllBytes(byte[] bytes) => File.WriteAllBytes(FullPath, bytes);

    #endregion


    // [JsonConstructor]
    // public FileItem(FileInfo file, DateTime lastWriteTimeUtc, string relativePath)
    // {
    //     m_file = file;
    //     LastWriteTimeUtc = lastWriteTimeUtc;
    //     RelativePath = relativePath;
    // }

    // public FileItem(ref Utf8JsonReader reader)
    // {
    //     m_file = new FileInfo(reader.GetString()!);
    //     LastWriteTimeUtc = reader.GetDateTime();
    //     RelativePath = reader.GetString()!;
    // }
    // 
    // public void Write(Utf8JsonWriter writer)
    // {
    //     writer.WriteStringValue(m_file.FullName);
    //     writer.WriteStringValue(LastWriteTimeUtc);
    //     writer.WriteStringValue(RelativePath);
    // }
}


// internal sealed class FileItemJsonConverter : JsonConverter<FileItem>
// {
//     public override FileItem Read(ref Utf8JsonReader reader, Type typeToConvert, JsonSerializerOptions options)
//     {
//         return new FileItem(ref reader);
//     }
// 
//     public override void Write(Utf8JsonWriter writer, FileItem fileItemValue, JsonSerializerOptions options)
//     {
//         fileItemValue.Write(writer);
//     }
// }
