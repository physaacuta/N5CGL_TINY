'*********************************************************************************
'共有メモリ のアクセスを行う基本クラス
'	[Ver]
'		Ver.01    2005/12/30  vs2005 対応
'
'	[メモ]
'		疵検では、つかっちゃダメ？
'
'		構造体を使用する場合は、以下のサンプルの様に構造体を指定する
'
    ' '' 構造体サンプル
    '<StructLayout(LayoutKind.Sequential, Size:=1200, pack:=1)> _
    'Structure SAMPLE
    '	<VBFixedArray(99), MarshalAs(UnmanagedType.ByValArray, SizeConst:=100)> _
    '	Dim dataL() As Integer

    '	<VBFixedString(400), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=400)> _
    '	Dim dataS As String

    '	<VBFixedArray(399), MarshalAs(UnmanagedType.ByValArray, SizeConst:=400)> _
    '	Dim dataB() As Byte

    '	Public Sub Inisialize()
    '		ReDim dataL(99)
    '		ReDim dataB(399)
    '	End Sub
    'End Structure
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices				'MarshalAsクラス等々
Imports Microsoft.Win32.SafeHandles

Namespace tClass

    Public Class tClass_GlovalMemory

        '********************************************************************
        'API定義
        '********************************************************************
        Private Const PAGE_READWRITE As Integer = &H4
        Private Const STANDARD_RIGHTS_REQUIRED As Integer = &HF0000
        Private Const SECTION_QUERY As Integer = &H1
        Private Const SECTION_MAP_WRITE As Integer = &H2
        Private Const SECTION_MAP_READ As Integer = &H4
        Private Const SECTION_MAP_EXECUTE As Integer = &H8
        Private Const SECTION_EXTEND_SIZE As Integer = &H10
        Private Const SECTION_ALL_ACCESS As Integer = STANDARD_RIGHTS_REQUIRED Or _
                  SECTION_QUERY Or _
                  SECTION_MAP_WRITE Or _
                  SECTION_MAP_READ Or _
                  SECTION_MAP_EXECUTE Or _
                  SECTION_EXTEND_SIZE
        Private Const FILE_MAP_ALL_ACCESS As Integer = SECTION_ALL_ACCESS '' 通常はこれを使用する。


        '' ファイルマッピングオブジェクトを開く
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
        Private Shared Function OpenFileMapping( _
           ByVal dwDesiredAccess As Integer, _
           ByVal bInheritHandle As Boolean, _
           ByVal lpName As String) _
           As SafeFileHandle
        End Function

        '' ファイルマッピングオブジェクトを作成
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
        Private Shared Function CreateFileMapping( _
           ByVal hFile As IntPtr, _
           ByVal lpAttributes As IntPtr, _
           ByVal flProtect As Integer, _
           ByVal dwMaximumSizeHigh As Integer, _
           ByVal dwMaximumSizeLow As Integer, _
           ByVal lpName As String) _
           As SafeFileHandle
        End Function

        '' 呼び出し側プロセスのアドレス空間に、ファイルのビューをマッピングする
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Private Shared Function MapViewOfFile( _
            ByVal hFMapp As SafeFileHandle, _
            ByVal dwDesiredAccess As Integer, _
            ByVal dwFileOffsetHigh As Integer, _
            ByVal dwFileOffsetLow As Integer, _
            ByVal dwNumberOfBytesToMap As Integer) _
            As IntPtr
        End Function

        '' ファイルのビューを開放する
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Private Shared Function UnmapViewOfFile( _
           ByVal hMapped As IntPtr) _
           As Boolean
        End Function



        '********************************************************************
        'メソッド定義
        '********************************************************************
        Private Shared m_fMapBase As New Dictionary(Of String, SafeFileHandle)      '共有メモリ アドレス。

        Private Shared m_mtxObj As New Threading.Mutex(False, "tClass_GlovalMemoryMtx")     ' タスク間排他制御


        ''' <summary>
        ''' 共有メモリ生成
        ''' </summary>
        ''' <param name="intMapSize">共有メモリサイズ</param>
        ''' <param name="strMapName">共有メモリ名称</param>
        ''' <returns>ハンドル</returns>
        ''' <remarks>CreateFileMapping, MapViewOfFileを使用
        ''' 		ただし、すでに共有メモリが生成されている場合は、OpenFileMapping</remarks>
        Public Shared Function AllocMapped(ByVal intMapSize As Integer, ByVal strMapName As String) As IntPtr
            Dim INVALID_HANDLE_VALUE As New IntPtr(-1L)


            '''' ファイルマップがすでに作成されていれば、OpenFileMapping
            '' 無ければ、CreateFileMapping を行う
            Dim fMap As SafeFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, False, strMapName)
            If fMap.IsInvalid Then
                '' 名前付きまたは名前なしのファイルマッピングオブジェクトを作成
                fMap = CreateFileMapping(INVALID_HANDLE_VALUE, IntPtr.Zero, PAGE_READWRITE, 0, intMapSize, strMapName)

                m_fMapBase.Add(strMapName, fMap)
            End If


            '' 呼び出し側プロセスのアドレス空間に、ファイルのビューをマッピングする
            Return MapViewOfFile(fMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)
        End Function
        ''' <summary>
        ''' 共有メモリ アクセス
        ''' </summary>
        ''' <param name="strMapName">共有メモリ名称</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function OpenMapped(ByVal strMapName As String) As IntPtr
            Dim INVALID_HANDLE_VALUE As Integer = -1


            '''' ファイルマップがすでに作成されていれば、OpenFileMapping
            ' 無ければ失敗
            Dim fMap As SafeFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, False, strMapName)
            If fMap.IsInvalid Then
                Return IntPtr.Zero
            End If

            '' 呼び出し側プロセスのアドレス空間に、ファイルのビューをマッピングする
            Return MapViewOfFile(fMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)
        End Function

        '''' <summary>
        '''' 共有メモリ開放
        '''' </summary>
        '''' <param name="hMap">共有メモリハンドル</param>
        '''' <remarks></remarks>
        'Public Shared Sub FreeMapped(ByRef hMap As IntPtr)
        '	'' ハンドルチェック
        '	If hMap.Equals(IntPtr.Zero) Then Exit Sub

        '	'' 共有メモリ開放
        '	UnmapViewOfFile(hMap)
        '	CloseHandle(hMap.ToInt32)
        '	hMap = IntPtr.Zero
        'End Sub
        ''' <summary>
        ''' 共有メモリ開放
        ''' </summary>
        ''' <param name="hMap">共有メモリハンドル</param>
        ''' <param name="strMapName">共有メモリ名称</param>
        ''' <remarks></remarks>
        Public Shared Sub FreeMapped(ByRef hMap As IntPtr, ByVal strMapName As String)
            '' ハンドルチェック
            If hMap.Equals(IntPtr.Zero) Then Exit Sub

            '' 共有メモリ開放
            UnmapViewOfFile(hMap)

            '' 本体開放
            Dim fWk As SafeFileHandle = Nothing
            If m_fMapBase.TryGetValue(strMapName, fWk) Then
                fWk.Close()
                m_fMapBase.Remove(strMapName)
            End If
            hMap = IntPtr.Zero
        End Sub


        ''' <summary>
        ''' 共有メモリの構造体を書込む
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMapped(ByVal hMap As IntPtr, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' 構造体データを共有メモリにセット
                'System.Runtime.InteropServices.Marshal.StructureToPtr(objVal, hMap, True)
                'Marshal.StructureToPtr(objVal, hMap, True)
                Marshal.StructureToPtr(objVal, hMap, False)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' 共有メモリの構造体を書込む
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMapped(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()
                '' 構造体データを共有メモリにセット
                Dim nOffset As New IntPtr(hMap.ToInt64 + nStartIndex)
                'Marshal.StructureToPtr(objVal, CType(nOffset, IntPtr), True)
                Marshal.StructureToPtr(objVal, nOffset, False)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function


        ''' <summary>
        ''' 共有メモリの構造体を読込み
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="objType">共有メモリ構造体の型</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMapped(ByVal hMap As IntPtr, ByVal objType As System.Type, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' 共有メモリのデータを指定構造体にセット
                objVal = DirectCast(Marshal.PtrToStructure(hMap, objType), ValueType)          'メモリからデータを取得し、構造体へセット

                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' 共有メモリの構造体を読込み
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="nStartIndex">開始位置</param>
        ''' <param name="objType">共有メモリ構造体の型</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMapped(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByVal objType As System.Type, ByRef objVal As ValueType) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' 共有メモリのデータを指定構造体にセット
                Dim nOffset As New IntPtr(hMap.ToInt64 + nStartIndex)
                objVal = DirectCast(Marshal.PtrToStructure(nOffset, objType), ValueType)                     'メモリからデータを取得し、構造体へセット


                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' 共有メモリの構造体を書込む
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function SetMappedT(Of T)(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                m_mtxObj.WaitOne()
                '' 構造体データを共有メモリにセット
                tMod.WriteStructure(hMap, nStartIndex, objVal)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

        ''' <summary>
        ''' 共有メモリの構造体を読込み(型つき)
        ''' </summary>
        ''' <param name="hMap">共有メモリ構造体のポインタ</param>
        ''' <param name="nStartIndex">開始位置</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function GetMappedT(Of T)(ByVal hMap As IntPtr, ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                m_mtxObj.WaitOne()

                '' 共有メモリのデータを指定構造体にセット
                objVal = tMod.ReadStructure(Of T)(hMap, nStartIndex)
                Return True
            Catch
                Return False
            Finally
                m_mtxObj.ReleaseMutex()
            End Try
        End Function

    End Class
End Namespace