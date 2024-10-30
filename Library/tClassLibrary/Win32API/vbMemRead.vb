Option Strict On
Option Explicit On

Imports System.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles

Namespace tClass
    Friend Class SharedMemoryApi

        '********************************************************************
        'API定義
        '********************************************************************
        Public Const PAGE_READWRITE As Integer = &H4
        Public Const STANDARD_RIGHTS_REQUIRED As Integer = &HF0000
        Public Const SECTION_QUERY As Integer = &H1
        Public Const SECTION_MAP_WRITE As Integer = &H2
        Public Const SECTION_MAP_READ As Integer = &H4
        Public Const SECTION_MAP_EXECUTE As Integer = &H8
        Public Const SECTION_EXTEND_SIZE As Integer = &H10
        Public Const SECTION_ALL_ACCESS As Integer = STANDARD_RIGHTS_REQUIRED Or SECTION_QUERY Or SECTION_MAP_WRITE Or SECTION_MAP_READ Or SECTION_MAP_EXECUTE Or SECTION_EXTEND_SIZE
        Public Const FILE_MAP_ALL_ACCESS As Integer = SECTION_ALL_ACCESS    '' 通常はこれを使用する。


        '' ファイルマッピングオブジェクトを開く
        <DllImport("KERNEL32.DLL", EntryPoint:="OpenFileMapping", SetLastError:=True)> _
        Public Shared Function OpenFileMapping(ByVal dwDesiredAccess As Integer, ByVal bInheritHandle As Boolean, ByVal lpName As String) As SafeFileHandle
        End Function

        '' ファイルマッピングオブジェクトを作成
        <DllImport("KERNEL32.DLL", EntryPoint:="CreateFileMapping", SetLastError:=True)> _
        Public Shared Function CreateFileMapping(ByVal hFile As IntPtr, ByVal lpAttributes As IntPtr, ByVal flProtect As Integer, ByVal dwMaximumSizeHigh As Integer, ByVal dwMaximumSizeLow As Integer, ByVal lpName As String) As SafeFileHandle
        End Function

        '' 呼び出し側プロセスのアドレス空間に、ファイルのビューをマッピングする
        <DllImport("KERNEL32.DLL", EntryPoint:="MapViewOfFile", SetLastError:=True)> _
        Public Shared Function MapViewOfFile(ByVal hFMapp As SafeFileHandle, ByVal dwDesiredAccess As Integer, ByVal dwFileOffsetHigh As Integer, ByVal dwFileOffsetLow As Integer, ByVal dwNumberOfBytesToMap As Integer) As IntPtr
        End Function

        '' ファイルのビューを開放する
        <DllImport("KERNEL32.dll", EntryPoint:="UnmapViewOfFile", SetLastError:=True)> _
        Public Shared Function UnmapViewOfFile(ByVal pMapped As IntPtr) As Boolean
        End Function


        ''Mapのアドレス制約(dwAllocationGranularity)を取得するためのSystemInfo
        <StructLayout(LayoutKind.Sequential)> _
        Public Structure SystemInfo
            Dim dwOemId As Integer
            Dim dwPageSize As Integer
            Dim lpMinimumApplicationAddress As IntPtr
            Dim lpMaximumApplicationAddress As IntPtr
            Dim dwActiveProcessorMask As IntPtr
            Dim dwNumberOfProcessors As Integer
            Dim dwProcessorType As Integer
            Dim dwAllocationGranularity As Integer
            Dim wProcessorLevel As Short
            Dim wProcessorRevision As Short
        End Structure

        '' SystemInfoを取得する
        <DllImport("KERNEL32.dll", EntryPoint:="GetSystemInfo", SetLastError:=True)> _
        Private Shared Sub GetSystemInfo(ByRef pMapped As SystemInfo)
        End Sub

        Public Shared sysinfo As SystemInfo
        Shared Sub New()
            GetSystemInfo(sysinfo)
        End Sub

    End Class

    Public Class SharedMemory
        Implements IDisposable                          ' デストラクタ定義

        '********************************************************************
        'メンバ定義
        '********************************************************************
        Protected m_fMap As New SafeFileHandle(IntPtr.Zero, False)
        Protected m_pMap As IntPtr = IntPtr.Zero

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' 共有メモリ アクセス
        ''' </summary>
        ''' <param name="strMapName">共有メモリ名称</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function Open(ByVal strMapName As String) As Boolean
            '念のため
            If Not m_fMap.IsClosed Then m_fMap.Close()

            m_fMap = SharedMemoryApi.OpenFileMapping(SharedMemoryApi.FILE_MAP_ALL_ACCESS, False, strMapName)
            If m_fMap.IsInvalid Then
                Return False
            End If

            '' 呼び出し側プロセスのアドレス空間に、ファイルのビューをマッピングする
            m_pMap = SharedMemoryApi.MapViewOfFile(m_fMap, SharedMemoryApi.FILE_MAP_ALL_ACCESS, 0, 0, 0)
            Return IntPtr.Zero <> m_pMap
        End Function

        ''' <summary>
        ''' Dispose
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks></remarks>
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If m_pMap <> IntPtr.Zero Then
                SharedMemoryApi.UnmapViewOfFile(m_pMap)
                m_pMap = IntPtr.Zero
            End If
            m_fMap.Dispose()
        End Sub
#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region
        Public Overloads Function Address() As IntPtr
            Return m_pMap
        End Function

        Public Overloads Function Address(offset As Integer) As IntPtr
            If IntPtr.Size = 8 Then
                Return New IntPtr(m_pMap.ToInt64 + offset)
            End If
            Return New IntPtr(m_pMap.ToInt32 + offset)
        End Function
        Public Overloads Function Address(offset As Long) As IntPtr
            If IntPtr.Size = 8 Then
                Return New IntPtr(m_pMap.ToInt64 + offset)
            End If
            Return New IntPtr(m_pMap.ToInt32 + CInt(offset))
        End Function


        ''' <summary>
        ''' 共有メモリの指定オフセットから構造体を書き込む。
        ''' </summary>
        ''' <typeparam name="T"> 共有メモリ構造体の型 </typeparam>
        ''' <param name="nStartIndex">開始位置</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function Write(Of T)(ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                '' 構造体データを共有メモリにセット
                Marshal.StructureToPtr(objVal, Address(nStartIndex), False)
                Return True
            Catch
                Return False
            End Try
        End Function



        ''' <summary>
        ''' 共有メモリの指定オフセットから構造体を読込み返す。
        ''' </summary>
        ''' <typeparam name="T"> 共有メモリ構造体の型 </typeparam>
        ''' <param name="nStartIndex">開始位置</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function Read(Of T)(ByVal nStartIndex As Integer, ByRef objVal As T) As Boolean
            Try
                If GetType(T).BaseType Is GetType(System.ValueType) Then
                    '' 共有メモリのデータを指定構造体にセット
                    objVal = DirectCast(Marshal.PtrToStructure(Address(nStartIndex), GetType(T)), T)
                Else
                    'メモリからデータを取得し、クラスへセット
                    Marshal.PtrToStructure(Address(nStartIndex), objVal)
                End If
                Return True
            Catch
                Return False
            End Try
        End Function

    End Class



    ''' <summary>
    ''' 必要になる都度、共有メモリをマッピングして処理するクラス
    ''' </summary>
    ''' <remarks>アドレス・オフセット類＝絶対：共有メモリ全体の先頭からの。相対：いまマップしている共有メモリの頭から。仮想：仮想メモリのアドレス。
    ''' </remarks>
    Public Class SharedMemoryPartial
        Implements IDisposable                          ' デストラクタ定義

        '********************************************************************
        'メンバ定義
        '********************************************************************
        Protected m_fMap As SafeFileHandle
        Protected m_accessR As Integer = SharedMemoryApi.FILE_MAP_ALL_ACCESS
        'マップされたエリアの仮想アドレス。
        Protected m_pMap As IntPtr = IntPtr.Zero


        '最低でもこのくらいのサイズはマップしておきたいな、というサイズ。
        Protected ReadOnly m_CacheLenMin As Integer = 65536 * 4
        'いま現在マップしているメモリの領域(共有メモリ相対)= m_nCacheOffset ~ m_nCacheLen
        Protected m_nCacheOffset As IntPtr
        Protected m_nCacheLen As Integer = 0
        'Mapするときのアドレス制約。APIに渡すのは、nAllocationSizeの倍数でないとダメ。
        Protected Shared nAllocationSize As Integer = 0
        Protected Shared nAllocationMask As Long = 0

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' 共有メモリ アクセス
        ''' </summary>
        ''' <param name="strMapName">共有メモリ名称</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal strMapName As String, Optional ByVal accessR As Integer = SharedMemoryApi.FILE_MAP_ALL_ACCESS)
            m_accessR = accessR
            m_fMap = SharedMemoryApi.OpenFileMapping(m_accessR, False, strMapName)

            If nAllocationSize = 0 Then
                nAllocationSize = SharedMemoryApi.sysinfo.dwAllocationGranularity
                nAllocationMask = Not (CLng(nAllocationSize) - 1)
            End If
        End Sub


        Public ReadOnly Property IsInvalid() As Boolean
            Get
                Return m_fMap.IsInvalid
            End Get
        End Property

        ''' <summary>
        ''' Dispose
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks></remarks>
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            Unmap()
            m_fMap.Dispose()
        End Sub
#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region



        Sub Unmap()
            If m_pMap <> IntPtr.Zero Then
                SharedMemoryApi.UnmapViewOfFile(m_pMap)
                m_pMap = IntPtr.Zero
            End If
        End Sub


        ''' <summary>
        ''' 共有メモリの絶対オフセットを仮想メモリアドレスに変換する
        ''' </summary>
        ''' <param name="nAddr">共有メモリの絶対オフセット</param>
        ''' <returns>仮想メモリのアドレス</returns>
        Function ConvVirtualAddress(ByVal nAddr As IntPtr) As IntPtr
            If IntPtr.Size = 4 Then
                Return New IntPtr(m_pMap.ToInt32 + nAddr.ToInt32 - m_nCacheOffset.ToInt32)
            Else
                Return New IntPtr(m_pMap.ToInt64 + nAddr.ToInt64 - m_nCacheOffset.ToInt64)
            End If
        End Function

        ''' <summary>
        ''' 現在のマップ（キャッシュ）上に要求された絶対位置～サイズが乗っているか？
        ''' </summary>
        Function InCacheRange(ByVal nAddr As IntPtr, ByVal nLength As Integer) As Boolean
            Dim nROffset As Long = (nAddr.ToInt64 - m_nCacheOffset.ToInt64)
            If nROffset < 0 Then Return False
            Return nROffset + nLength < m_nCacheLen
        End Function

        <StructLayout(LayoutKind.Explicit)> _
        Private Structure LargeInteger
            <FieldOffset(0)> Dim QuadPart As Long
            <FieldOffset(0)> Dim LowPart As Integer
            <FieldOffset(4)> Dim HighPart As Integer
        End Structure

        ''' <summary>
        ''' 共有メモリの絶対オフセットを　仮想メモリ上にマップする
        ''' </summary>
        Function Map(ByVal nAddr As IntPtr, ByVal nLength As Integer) As IntPtr
            Unmap()

            'アドレスは、nAllocationSizeでアラインされている必要がある。適宜前へ戻す。
            Dim nAddrAlign As Long = nAddr.ToInt64() And nAllocationMask
            'アドレスが前に戻った分、サイズは大きくなる.　
            ' + サイズもnAllocationSizeでアラインされる必要がある。
            Dim nLenAlign As Integer = CInt((nLength + (nAddr.ToInt64() - nAddrAlign) + nAllocationSize - 1) And nAllocationMask)

            'longを、int*2に分解する。
            Dim li As New LargeInteger
            li.HighPart = 0
            li.LowPart = 0
            li.QuadPart = nAddrAlign

            'マップする。
            m_nCacheOffset = New IntPtr(nAddrAlign)
            m_nCacheLen = Math.Max(m_CacheLenMin, nLenAlign)
            m_pMap = SharedMemoryApi.MapViewOfFile(m_fMap, m_accessR, li.HighPart, li.LowPart, m_nCacheLen)
            If (IntPtr.Zero = m_pMap) Then
                Throw New System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error(), "共有メモリのマップに失敗")
            End If
            Return m_pMap
        End Function



        ''' <summary>
        ''' 共有メモリの指定オフセットから構造体を書き込む。
        ''' </summary>
        ''' <typeparam name="T"> 共有メモリ構造体の型 </typeparam>
        ''' <param name="nAddr">開始位置(絶対オフセット)</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        Public Function Write(Of T)(ByVal nAddr As IntPtr, ByRef objVal As T) As Boolean
            Try
                If Not InCacheRange(nAddr, Marshal.SizeOf(objVal)) Then
                    Map(nAddr, Marshal.SizeOf(objVal))
                End If

                '' 構造体データを共有メモリにセット
                Marshal.StructureToPtr(objVal, ConvVirtualAddress(nAddr), False)
                Return True
            Catch
                Return False
            End Try
        End Function

        '上のアドレス引数がInt版。
        Public Function Write(Of T)(ByVal nAddr As Integer, ByRef objVal As T) As Boolean
            Return Write(New IntPtr(nAddr), objVal)
        End Function


        ''' <summary>
        ''' 共有メモリの指定オフセットから構造体を読込み返す。
        ''' </summary>
        ''' <typeparam name="T"> 共有メモリ構造体の型 </typeparam>
        ''' <param name="nAddr">開始位置(絶対オフセット)</param>
        ''' <param name="objVal">共有メモリ構造体</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function Read(Of T)(ByVal nAddr As IntPtr, ByRef objVal As T) As Boolean
            Try
                If Not InCacheRange(nAddr, Marshal.SizeOf(objVal)) Then
                    Map(nAddr, Marshal.SizeOf(objVal))
                End If

                If GetType(T).BaseType Is GetType(System.ValueType) Then
                    '' 共有メモリのデータを指定構造体にセット
                    objVal = DirectCast(Marshal.PtrToStructure(ConvVirtualAddress(nAddr), GetType(T)), T)
                Else
                    'メモリからデータを取得し、クラスへセット
                    Marshal.PtrToStructure(ConvVirtualAddress(nAddr), objVal)
                End If
                Return True
            Catch
                Return False
            End Try
        End Function

        '上のアドレス引数がInt版。
        Public Function Read(Of T)(ByVal nAddr As Integer, ByRef objVal As T) As Boolean
            Return Read(New IntPtr(nAddr), objVal)
        End Function

    End Class

    Public Class SharedMemoryReaeWrite
        Inherits SharedMemory

        <DllImport("kernel32")> Shared Function WriteFile(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByVal lpBuffer As IntPtr, ByVal nWrite As Integer, ByRef nWritten As Integer, ByVal lpOverlapped As IntPtr) As Boolean
        End Function

        <DllImport("kernel32")> Shared Function ReadFile(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByVal lpBuffer As IntPtr, ByVal nRead As Integer, ByRef nWritten As Integer, ByVal lpOverlapped As IntPtr) As Boolean
        End Function

        <DllImport("kernel32")> Shared Function GetFileSizeEx(ByVal hFile As Microsoft.Win32.SafeHandles.SafeFileHandle, ByRef nFileSize As Long) As Boolean
        End Function

        <StructLayout(LayoutKind.Sequential)> _
        Structure MEMORY_BASIC_INFORMATION
            Public BaseAddress As IntPtr
            Public AllocationBase As IntPtr
            Public AllocationProtect As Integer
            Public RegionSize As IntPtr
            Public State As Integer
            Public Protect As Integer
            Public Type As Integer
        End Structure

        <DllImport("kernel32")> Shared Function VirtualQuery(ByVal lpAddress As IntPtr, ByRef memoryinf As MEMORY_BASIC_INFORMATION, ByVal dwLength As IntPtr) As Integer
        End Function


        ''' <summary>
        ''' 共有メモリから読み込んでファイルに出力する
        ''' </summary>
        ''' <param name="ShName">共有メモリ名称</param>
        ''' <param name="size">共有メモリサイズ（サイズ指定しない場合は、取得した共有メモリサイズ）</param>
        ''' <returns>0:成功 1:共有メモリオープンエラー 2:共有メモリサイズ=0 3:指定サイズ不正 -1以下 Win32APIエラー </returns>
        ''' <remarks></remarks>
        Public Function WriteToFile(ByVal ShName As String, Optional ByVal size As Integer = 0) As Integer
            Dim rc As Integer = 0

            ' 共有メモリをオープンする
            If Open(ShName) = False Then
                Return 1
            End If

            ' 共有メモリサイズを取得する ⇒ 割り当てサイズではなく、ページサイズの倍数？となる
            Dim mapinfo As New MEMORY_BASIC_INFORMATION
            VirtualQuery(m_pMap, mapinfo, New IntPtr(Marshal.SizeOf(mapinfo)))

            ' 取得した共有メモリサイズチェック
            If mapinfo.RegionSize.ToInt32 = 0 Then
                ' 共有メモリは割り当てられていない？
                Return 2
            End If

            ' 書き込みサイズを決定する。書き込みファイルサイズ指定の場合はその値を使用する
            Dim wSize As Integer = CInt(IIf(size = 0, mapinfo.RegionSize.ToInt32, size))

            If wSize > mapinfo.RegionSize.ToInt32 Then
                ' 指定サイズが共有メモリサイズより大きい場合はどうするか
                Return 3
            End If

            ' 出力ファイル名は "<共有メモリテーブル名>.bin" とする。
            Dim fname As String = IO.Path.Combine(".\", ShName & ".bin")

            ' 指定された共有メモリの内容をファイルに出力する
            Try
                Using fsw As New IO.FileStream(fname, IO.FileMode.Create)
                    Dim nWritten As Integer = 0
                    If WriteFile(fsw.SafeFileHandle, m_pMap, wSize, nWritten, IntPtr.Zero) = False Then
                        rc = Marshal.GetLastWin32Error()
                    End If
                End Using
            Catch ex As Exception
                ' 何らかの例外発生
                rc = 999
            End Try

            Return rc

        End Function

        ''' <summary>
        ''' ファイルから読み込んで共有メモリに出力する
        ''' </summary>
        ''' <param name="ShName">共有メモリ名称</param>
        ''' <param name="size">共有メモリサイズ（サイズ指定しない場合は、取得した共有メモリサイズ）</param>
        ''' <returns>0:成功 1:共有メモリオープンエラー 2:共有メモリサイズ=0 3:指定サイズ不正 -1以下 Win32APIエラー </returns>
        ''' <remarks></remarks>
        Public Function ReadFromFile(ByVal ShName As String, Optional ByVal size As Integer = 0) As Integer
            Dim rc As Integer = 0

            ' 共有メモリをオープンする
            If Open(ShName) = False Then
                Return 1
            End If

            ' 共有メモリサイズを取得する ⇒ 割り当てサイズではなく、ページサイズの倍数？となる
            Dim mapinfo As New MEMORY_BASIC_INFORMATION
            VirtualQuery(m_pMap, mapinfo, New IntPtr(Marshal.SizeOf(mapinfo)))

            ' 取得した共有メモリサイズチェック
            If mapinfo.RegionSize.ToInt32 = 0 Then
                ' 共有メモリは割り当てられていない？
                Return 2
            End If

            ' 書き込みサイズを決定する。サイズ指定の場合はその値を使用する
            Dim wSize As Integer = CInt(IIf(size = 0, mapinfo.RegionSize.ToInt32, size))

            If wSize > mapinfo.RegionSize.ToInt32 Then
                ' 指定サイズが共有メモリサイズより大きい場合はエラー
                Return 3
            End If

            ' 読み込みファイル名は "<共有メモリテーブル名>.bin" とする。
            Dim fname As String = IO.Path.Combine(".\", ShName & ".bin")

            If IO.File.Exists(fname) = False Then
                ' 指定されたファイルが存在しない
                Return 4
            End If

            Using fsr As New IO.FileStream(fname, IO.FileMode.Open)
                Dim nRead As Integer = 0
                If ReadFile(fsr.SafeFileHandle, m_pMap, wSize, nRead, IntPtr.Zero) = False Then
                    rc = Marshal.GetLastWin32Error()
                End If
            End Using

            Return rc

        End Function

    End Class
End Namespace
