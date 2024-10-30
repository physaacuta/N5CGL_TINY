Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.IO                       ' ファイルアクセス

Namespace tClass

    ''' <summary>
    ''' BinaryReader + alpha(ReadString)
    ''' </summary>
    Public Class BinaryReaderSP
        Inherits BinaryReader
        Public Sub New(ByVal stream As Stream)
            MyBase.New(stream)
        End Sub
        Public Sub New(ByVal arr() As Byte)
            MyBase.New(New MemoryStream(arr))
        End Sub

        Public Property Position As Long
            Get
                Return BaseStream.Position

            End Get
            Set(value As Long)
                BaseStream.Position = value
            End Set
        End Property

        ''' <summary>
        ''' バイナリファイルからString値取込
        ''' </summary>
        ''' <param name="size">取得バイト数</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shadows Function ReadString(ByVal size As Integer) As String
            Dim wk() As Byte = ReadBytes(size)
            Return tMod.tMod_Common.ByteToString(wk).TrimEnd(Chr(0))
        End Function

    End Class


    ''' <summary>
    ''' BinaryReader + alpha(ReadString)
    ''' </summary>
    Public Class BinaryWriterSP
        Inherits BinaryWriter


        Public Sub New(ByVal stream As Stream)
            MyBase.New(stream)
        End Sub
        Public Sub New(ByVal arr() As Byte)
            MyBase.New(New MemoryStream(arr))
        End Sub
        Public Property Position As Long
            Get
                Return BaseStream.Position

            End Get
            Set(value As Long)
                BaseStream.Position = value
            End Set
        End Property

        Public encoder As System.Text.Encoding = System.Text.Encoding.GetEncoding(932)

        ''' <summary>
        ''' バイナリファイルへのString書き込み
        ''' </summary>
        ''' <param name="size">バイト数</param>
        ''' <remarks></remarks>
        Public Sub WriteString(str As String, ByVal size As Integer)
            If Not String.IsNullOrEmpty(str) Then
                Dim enc() As Byte = encoder.GetBytes(str)
                Dim copy_len As Integer = Math.Min(enc.Length, size)
                MyBase.Write(enc, 0, copy_len)
                size -= enc.Length
            End If
            Dim write_mem(size - 1) As Byte
            MyBase.Write(write_mem)
        End Sub

    End Class
End Namespace