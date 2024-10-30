'*********************************************************************************
'iniファイルの読み書きを行う基本モジュール群
'	[Ver]
'		Ver.01    2005/12/24  vs2005 対応
'
'	[メモ]
'
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices          ' DllImport
Imports System

Namespace tMod
    ''' <summary>
    ''' iniファイルの読み書きを行う基本モジュール群
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_IniFile
        '********************************************************************
        'API定義
        '********************************************************************
        Friend Class SafeNativeMethods
            ''' <summary>
            ''' [API] iniファイルから文字列読み込み
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileStringA", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileString( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpApplicationName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpDefault As String, _
                <Out(), MarshalAs(UnmanagedType.LPStr)> ByVal lpReturnedString As Text.StringBuilder, _
                ByVal nSize As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
            As Integer
            End Function

            ''' <summary>
            ''' INIファイルから、セクション単位で読みこむAPI
            ''' </summary>
            ''' <returns>書き込んだ文字数</returns>
            ''' <remarks> null-separation(double-null-terminate)なので、returnstringはLPSTRでマーシャリングできない</remarks>
            <DllImport("KERNEL32.DLL", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileSectionA( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpApplicationName As String, _
                ByVal lpReturnedString As IntPtr, _
                ByVal nSize As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Integer
            End Function

            ''' <summary>
            ''' [API] iniファイルから数値読み込み
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileInt", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
            Friend Shared Function GetPrivateProfileInt( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                ByVal nDefault As Integer, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Integer
            End Function

            ''' <summary>
            ''' [API] iniファイルに文字列書き込み
            ''' </summary>
            <DllImport("KERNEL32.DLL", EntryPoint:="WritePrivateProfileStringA", SetLastError:=True, ExactSpelling:=True, CharSet:=CharSet.Ansi)> _
            Friend Shared Function WritePrivateProfileString( _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpString As String, _
                <[In](), MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
                As Boolean
            End Function
        End Class


        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' iniファイルから 文字列 を読み込む
        ''' </summary>
        ''' <param name="strSection">セクション</param>
        ''' <param name="strKey">キー</param>
        ''' <param name="strFileName">iniファイルのフルパス</param>
        ''' <param name="strDef">初期値(失敗した場合の戻り値)</param>
        ''' <param name="intSize">読み込む最大文字数</param>
        ''' <returns>取得した値</returns>
        ''' <remarks></remarks>
        Public Function ReadIniStr(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal strDef As String = "", Optional ByVal intSize As Integer = 1024) As String
            Dim sbBuff As New Text.StringBuilder(intSize)
            '''' 値取得
            Call SafeNativeMethods.GetPrivateProfileString(strSection, strKey, strDef, sbBuff, sbBuff.Capacity, strFileName)
            Return sbBuff.ToString
        End Function

        ''' <summary>
        ''' iniファイルから 数値 を読み込む
        ''' </summary>
        ''' <param name="strSection">セクション</param>
        ''' <param name="strKey">キー</param>
        ''' <param name="strFileName">iniファイルのフルパス</param>
        ''' <param name="intDef">初期値(失敗した場合の戻り値)</param>
        ''' <returns>取得した値</returns>
        ''' <remarks></remarks>
        Public Function ReadIniInt(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal intDef As Integer = 0) As Integer
            '''' 値取得
            Return SafeNativeMethods.GetPrivateProfileInt(strSection, strKey, intDef, strFileName)
        End Function


        Private Function ReadIniSecionApi(ByVal strsection As String, ByVal strfilename As String) As String
            '読み込みバッファサイズを段々おおきくしていく。
            For Each intsize As Integer In New Integer() {256, 64 * 1024, 256 * 1024, 1024 * 1024}
                Dim len As Integer
                Dim memory(intsize - 1) As Byte
                Using pp As New PinnedPtr(memory)
                    len = SafeNativeMethods.GetPrivateProfileSectionA(strsection, pp.Ptr, intsize, strfilename)
                End Using

                If len = 0 Then
                    Return String.Empty
                End If

                'If the buffer is not large enough..., the return value is equal to nSize minus two.
                If len < intsize - 2 Then
                    Dim encoding As System.Text.Encoding = System.Text.Encoding.Default
                    Return encoding.GetString(memory, 0, len - 1)
                End If
            Next

            'さすがにINIで1MBを超えるのはないだろうから
            '例外をだしたほうがいいかも。
            Return String.Empty
        End Function


        ''' <summary>
        ''' INIファイルのセクションをまるごと文字列配列でかえす。
        ''' </summary>
        ''' <returns> "key=value"形の文字列配列</returns>
        ''' <remarks> 前後のTRIMはAPIがやってくれる。</remarks>
        Public Function ReadIniSecion(ByVal strsection As String, ByVal strfilename As String) As String()
            Dim rstr As String = ReadIniSecionApi(strsection, strfilename)
            If rstr.Length < 2 Then
                Return New String() {}
            End If

            Return rstr.Split(vbNullChar(0))
        End Function

        ''' <summary>
        ''' INIファイルのセクションをまるごとDictionaryでかえす。
        ''' </summary>
        ''' <returns>Keyの大小文字を区別しない、Dictionary</returns>
        Public Function ReadIniSecionAsDictionary(ByVal strsection As String, ByVal strfilename As String) As Dictionary(Of String, String)
            Dim list() As String = ReadIniSecion(strsection, strfilename)
            Dim rv As New Dictionary(Of String, String)(StringComparer.InvariantCultureIgnoreCase)
            For Each Str As String In list
                Dim eqpos As Integer = Str.IndexOf("="c)
                If eqpos >= 0 Then
                    'APIで、前後のスペースは除去してくれてる。
                    Dim key As String = Str.Substring(0, eqpos)
                    Dim value As String = Str.Substring(eqpos + 1)
                    rv.Add(key, value)
                End If
            Next
            Return rv
        End Function



        ''' <summary>
        ''' iniファイルの 値 に文字列 を書込み
        ''' </summary>
        ''' <param name="strSection">セクション</param>
        ''' <param name="strKey">キー(該当キーが無ければキーも追加)</param>
        ''' <param name="strFileName">iniファイルのフルパス</param>
        ''' <param name="strVal">書き込む値</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function WriteIni(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, ByVal strVal As String) As Boolean
            Try
                '''' 値書き込み
                Return SafeNativeMethods.WritePrivateProfileString(strSection, strKey, strVal, strFileName)
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' iniファイルから指定したキーを削除する
        ''' </summary>
        ''' <param name="strSection">セクション</param>
        ''' <param name="strKey">削除するキー</param>
        ''' <param name="strFileName">iniファイルのフルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function DeleteKeyIni(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String) As Boolean
            return WriteIni(strSection, strKey, strFileName, Nothing)
        End Function

        ''' <summary>
        ''' iniファイルから指定したセクションを削除する
        ''' </summary>
        ''' <param name="strSection">削除するセクション</param>
        ''' <param name="strFileName">iniファイルのフルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function DeleteSectionIni(ByVal strSection As String, ByVal strFileName As String) As Boolean
            return WriteIni(strSection, Nothing, strFileName, Nothing)
        End Function
    End Module
End Namespace
