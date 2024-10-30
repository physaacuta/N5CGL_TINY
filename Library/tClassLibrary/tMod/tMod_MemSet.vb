'*********************************************************************************
'メモリ操作を行うモジュール群
'	[Ver]
'		Ver.01    2005/12/25  vs2005 対応
'
'	[メモ]
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
Imports System.IO
Imports System.Runtime.InteropServices


Namespace tMod
    ''' <summary>
    ''' メモリ操作を行うモジュール群
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_MemSet

        ''' <summary>
        ''' 構造体・クラスのサイズ取得
        ''' </summary>
        ''' <param name="objVal">配列で無いインスタンス</param>
        ''' <returns>サイズ</returns>
        ''' <remarks></remarks>
        Public Function SizeOf(ByVal objVal As Object) As Integer
            Try
                Return Marshal.SizeOf(objVal)
            Catch ex As Exception
                Return 0
            End Try
        End Function

        ''' <param name="objVal">配列のクラス・構造体・変数 (ただし、全て同じ型である事)</param>
        ''' <returns>サイズ</returns>
        Public Function SizeOf(ByVal objVal As System.Array) As Integer
            Try
                Return objVal.Length * SizeOf(objVal.GetValue(0))
            Catch ex As Exception
                Return 0
            End Try
        End Function

        ''' <summary>
        ''' 構造体をメモリから読み出す
        ''' </summary>
        ''' <typeparam name="T">構造体型</typeparam>
        ''' <param name="addr_base">メモリ</param>
        ''' <param name="offset">メモリオフセット</param>
        ''' <returns>メモリから読んだ構造体</returns>
        Public Function ReadStructure(Of T)(ByVal addr_base As IntPtr, Optional ByVal offset As Integer = 0) As T
            Dim addr As New IntPtr(addr_base.ToInt64 + offset)
            Return DirectCast(Marshal.PtrToStructure(addr, GetType(T)), T)
        End Function

        ''' <summary>
        ''' 構造体をメモリに書き込む
        ''' </summary>
        ''' <typeparam name="T">型（自動推測）</typeparam>
        ''' <param name="addr_base">メモリ</param>
        ''' <param name="offset">メモリオフセット</param>
        ''' <param name="obj">構造体</param>
        Public Sub WriteStructure(Of T)(ByVal addr_base As IntPtr, ByVal offset As Integer, ByRef obj As T)
            Dim addr As New IntPtr(addr_base.ToInt64 + offset)
            Marshal.StructureToPtr(obj, addr, False)
        End Sub

        ''' <summary>
        ''' 構造体をメモリに書き込む
        ''' </summary>
        ''' <typeparam name="T">型（自動推測）</typeparam>
        ''' <param name="addr_base">メモリ</param>
        ''' <param name="obj">構造体</param>
        Public Sub WriteStructure(Of T)(ByVal addr_base As IntPtr, ByRef obj As T)
            Marshal.StructureToPtr(obj, addr_base, False)
        End Sub


        ''' <summary>
        ''' バイト配列から構造体へコピー
        ''' </summary>
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー元の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="typTo">コピー先データ領域 (構造体)</param>
        ''' <param name="length">コピーサイズ</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef typTo As ValueType, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(length)                        'メモリ取得
                Marshal.Copy(from, startIndex, p, length)               'メモリ上にデータをセット
                typTo = DirectCast(Marshal.PtrToStructure(p, typTo.GetType), ValueType)       'メモリからデータを取得し、構造体へセット
                Return True
            Catch ex As Exception
                typTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function

        ''' <summary>
        ''' バイト配列からオブジェクトへコピー
        ''' </summary>
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー元の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="objTo">コピー先データ領域 (オブジェクト)</param>
        ''' <param name="length">コピーサイズ</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef objTo As Object, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(length)                        'メモリ取得
                Marshal.Copy(from, startIndex, p, length)               'メモリ上にデータをセット
                objTo = Marshal.PtrToStructure(p, objTo.GetType)        'メモリからデータを取得し、構造体へセット
                Return True
            Catch ex As Exception
                objTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function

        ''' <summary>
        ''' 構造体からバイト配列へコピー
        ''' </summary>
        ''' <param name="from">コピー元構造体</param>
        ''' <param name="aryTo">コピー先データ配列</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="length">コピーサイズ</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from As ValueType, ByRef aryTo() As Byte, ByVal startIndex As Integer, ByVal length As Integer) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(SizeOf(from))                  'メモリ取得
                Marshal.StructureToPtr(from, p, True)                   'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, length)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function

        ''' <summary>
        ''' 構造体から構造体へコピー
        ''' </summary>
        ''' <param name="from">コピー元データ</param>
        ''' <param name="typTo">コピー先データ領域 (構造体)</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from As ValueType, ByRef typTo As ValueType) As Boolean
            Dim p As IntPtr
            Try
                p = Marshal.AllocHGlobal(SizeOf(from))                  'メモリ取得
                Marshal.StructureToPtr(from, p, True)                   'メモリ上にデータをセット
                typTo = DirectCast(Marshal.PtrToStructure(p, typTo.GetType), ValueType)       'メモリからデータを取得し、構造体へセット
                Return True
            Catch ex As Exception
                typTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function

#Region "byte配列から配列へコピー"
        ''' <summary>
        ''' byte配列から配列へコピー
        ''' </summary>
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Char, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Single, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Double, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Short, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Integer, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
        ''' <param name="from">コピー元データ</param>
        ''' <param name="startIndex">コピー先の開始配列位置 (0ｵﾘｼﾞﾝ)</param>
        ''' <param name="aryTo">コピー先データ領域</param>
        ''' <param name="count">コピーする要素数</param>
        ''' <returns>復帰情報</returns>
        Public Function MemSet(ByVal from() As Byte, ByVal startIndex As Integer, ByRef aryTo() As Long, ByVal count As Integer) As Boolean
            Dim p As IntPtr
            Try
                Dim allsize As Integer
                allsize = SizeOf(aryTo(startIndex)) * count
                p = Marshal.AllocHGlobal(allsize)                       'メモリ取得
                Marshal.Copy(from, startIndex, p, allsize)              'メモリ上にデータをセット
                Marshal.Copy(p, aryTo, startIndex, count)
                Return True
            Catch ex As Exception
                aryTo = Nothing
                Return False
            Finally
                Marshal.FreeHGlobal(p)                                  'メモリ開放
            End Try
        End Function
#End Region
    End Module
End Namespace


''' <summary>
''' 配列データをピン止めして、メモリポインタを取得する。
''' </summary>
''' <remarks></remarks>
Public Class PinnedPtr
    Implements IDisposable
    Private g1 As GCHandle

    Public Sub New(ByVal obj() As Byte)
        g1 = GCHandle.Alloc(obj, GCHandleType.Pinned)
    End Sub

    Public ReadOnly Property Ptr As IntPtr
        Get
            Return g1.AddrOfPinnedObject
        End Get
    End Property

    Public Sub Dispose() Implements IDisposable.Dispose
        g1.Free()
        GC.SuppressFinalize(Me)
    End Sub

End Class


Public Class VirtualAllocMemory
    Implements IDisposable

    Const MEM_COMMIT As Integer = &H1000
    Const MEM_TOPDOWN As Integer = &H100000
    Const PAGE_READWRITE As Integer = 4
    Const MEM_RELEASE As Integer = &H8000

    <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> Shared Function VirtualAlloc(lpAddress As IntPtr, dwSize As IntPtr, flAllocationType As Integer, flProtect As Integer) As IntPtr
    End Function
    <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> Shared Function VirtualFree(lpAddress As IntPtr, dwSize As IntPtr, dwFreeType As Integer) As Boolean
    End Function

    Private m_ptr As IntPtr
    Private m_size As Long

    ''' <summary>
    ''' メモリ所有権の譲り渡し。Usingしたけど解放したくなくなったときに。
    ''' </summary>
    ''' <returns></returns>
    Public Function MoveTo() As VirtualAllocMemory
        Dim vm As New VirtualAllocMemory
        vm.m_ptr = Me.m_ptr
        vm.m_size = Me.Size
        m_ptr = IntPtr.Zero
        m_size = 0
        Return vm
    End Function

    Public ReadOnly Property Valid As Boolean
        Get
            Return m_ptr <> IntPtr.Zero
        End Get
    End Property

    Public ReadOnly Property Ptr As IntPtr
        Get
            Return m_ptr
        End Get
    End Property

    Public ReadOnly Property Size As Long
        Get
            Return m_size
        End Get
    End Property


    Public Sub New(size As Long)
        m_ptr = VirtualAlloc(IntPtr.Zero, New IntPtr(size), MEM_COMMIT Or MEM_TOPDOWN, PAGE_READWRITE)
        m_size = size
        GC.AddMemoryPressure(size)
    End Sub
    Private Sub New()
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose
        Free()
        GC.SuppressFinalize(Me)
    End Sub

    ''' <summary>
    ''' ファイナライザ（通常はDisposeで解放せよ）
    ''' </summary>
    Protected Overrides Sub Finalize()
        Free()
    End Sub

    Private Sub Free()
        If m_ptr <> IntPtr.Zero Then
            VirtualFree(m_ptr, IntPtr.Zero, MEM_RELEASE)
            GC.RemoveMemoryPressure(m_size)
        End If
        m_ptr = IntPtr.Zero
        m_size = 0
    End Sub


End Class