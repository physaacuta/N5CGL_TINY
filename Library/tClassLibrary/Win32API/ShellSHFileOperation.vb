Option Explicit On
Option Strict On

Imports System.Runtime.InteropServices

Namespace SHAPI
    Public Enum FO
        Move = 1
        Copy = 2
        Delete = 3
        Rename = 4
    End Enum



    <StructLayout(LayoutKind.Sequential)> _
    Public Class SHFileOpStruct
        Public hWnd As IntPtr
        Public fOP As FO
        <MarshalAs(UnmanagedType.LPWStr)> Protected zzFrom As String
        <MarshalAs(UnmanagedType.LPWStr)> Protected zzTo As String
        Public flags As FileOperationFlags
        Public bAnyOperationAbort As Boolean
        Public hNameMappings As IntPtr
        <MarshalAs(UnmanagedType.LPWStr)> _
        Public ProgressTitle As String


        ''' <summary>
        ''' Double Numm Terminated Stringで文字列配列を表現する
        ''' </summary>
        ''' <param name="strs"></param>
        ''' <returns></returns>
        Private Function ZZStrFromArray(ByVal strs() As String) As String
            Return String.Join(vbNullChar, strs) + vbNullChar
        End Function


        Public Sub SetFromFile(ByVal ParamArray files() As String)
            zzFrom = ZZStrFromArray(files)
        End Sub
        Public Sub SetToFile(ByVal ParamArray files() As String)
            zzTo = ZZStrFromArray(files)
        End Sub

    End Class

    Public Module SHFileOperationAPI
        <DllImport("Shell32", CallingConvention:=CallingConvention.Winapi, CharSet:=CharSet.Unicode, SetLastError:=True)> _
        Function SHFileOperation(<[In]()> ByVal op As SHFileOpStruct) As Integer
        End Function
    End Module
End Namespace