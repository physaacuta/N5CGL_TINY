'*********************************************************************************
' LoadLibrary / GetProcAddressラッパー
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Runtime.InteropServices              'MarshalAsクラス等々

''' <summary>
''' アンマネージなDLLとその関数を扱う。
''' </summary>
''' <remarks>LoadLibraryとGetProcAddressを使用</remarks>
Public Class NativeDLL
    Implements IDisposable

    ''' <summary>
    ''' SafeHandle型のDllHandle。
    ''' </summary>
    Public Class SafeDllHandle
        Inherits Microsoft.Win32.SafeHandles.SafeHandleZeroOrMinusOneIsInvalid
        Public Sub New()
            MyBase.New(True)
        End Sub

        Protected Overrides Function ReleaseHandle() As Boolean
            return SafeNativeMethods.FreeLibrary(handle)
        End Function
    End Class

    ''' <summary>
    ''' ネイティブDLL呼び出し用クラス。
    ''' </summary>
    ''' <remarks></remarks>
    Friend Class SafeNativeMethods
        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Unicode)> _
        Friend Shared Function LoadLibrary(ByVal lpDllName As String) As SafeDllHandle
        End Function

        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
        Friend Shared Function GetProcAddress(ByVal hDll As SafeDllHandle, ByVal lpDllName As String) As IntPtr
        End Function

        <DllImport("kernel32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Friend Shared Function FreeLibrary(ByVal hDll As IntPtr) As Boolean
        End Function
    End Class
    ''' <summary>
    ''' DLLインスタンス
    ''' </summary>
    ''' <remarks></remarks>
    Private m_hDLL As SafeDllHandle

    ''' <summary>
    ''' 初期化
    ''' </summary>
    Sub New()
    End Sub

    ''' <summary>
    ''' DLLを読む初期化
    ''' </summary>
    ''' <remarks>読めない場合、例外DllNotFoundExceptionが飛ぶ</remarks>
    Sub New(ByVal dllname As String)
        m_hDLL = SafeNativeMethods.LoadLibrary(dllname)
        If Not IsLoad Then
            Throw New DllNotFoundException(dllname)
        End If
    End Sub

    ''' <summary>
    ''' DLLを読み込む
    ''' </summary>
    ''' <returns>読み込めればTrue</returns>
    Function Load(ByVal dllname As String) As Boolean
        If m_hDLL IsNot Nothing Then
            m_hDLL.Close()
        End If
        m_hDLL = SafeNativeMethods.LoadLibrary(dllname)
        Return IsLoad
    End Function

    ''' <summary>
    ''' DLLを読みこみ済みかを返す。
    ''' </summary>
    Public ReadOnly Property IsLoad() As Boolean
        Get
            Return Not m_hDLL.IsInvalid
        End Get
    End Property

    Public Sub Dispose() Implements IDisposable.Dispose
        m_hDLL.Dispose()
        m_hDLL = Nothing
        GC.SuppressFinalize(Me)
    End Sub

    Public Function GetAddress(ByVal ProcName As String) As IntPtr
        If m_hDLL.IsInvalid Then Return IntPtr.Zero
        Return SafeNativeMethods.GetProcAddress(m_hDLL, ProcName)
    End Function

    ''' <summary>
    ''' DLL内の関数を、Delegateの形で取得する
    ''' </summary>
    ''' <typeparam name="T">アンマネージなDelegate型</typeparam>
    ''' <param name="ProcName">関数名</param>
    ''' <returns>delegate</returns>
    Public Function GetDelegateAddress(Of T As Class)(ByVal ProcName As String) As T
        Dim pfProc As IntPtr = GetAddress(ProcName)
        If IntPtr.Zero = pfProc Then Return Nothing

        'いったんObjectを介さずに、直接Ctype( Marshal..., T ) だとなぜかエラーになる
        'たぶん、Tが汎用過ぎて、Delegateの継承がない場合のことだとは思う.
        'しかも、System.DelegateはTの制約に指定できないので、いったんobjectで受けるしかない。
        Dim wkObj As Object = Marshal.GetDelegateForFunctionPointer(pfProc, GetType(T))
        Return DirectCast(wkObj, T)
    End Function

    ''' <summary>
    ''' DLL内の関数を、Delegateの形で取得する
    ''' </summary>
    ''' <typeparam name="T">アンマネージなDelegate型</typeparam>
    ''' <param name="ProcName">関数名</param>
    ''' <returns>読み込めれば、True</returns>
    Public Function ReadDelegateAddress(Of T As Class)(ByVal ProcName As String, ByRef tdele As T) As Boolean
        tdele = GetDelegateAddress(Of T)(ProcName)
        Return tdele IsNot Nothing
    End Function

End Class
