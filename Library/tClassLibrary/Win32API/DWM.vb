Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices              'MarshalAsクラス等々

Namespace Win32API
    Public Class DWM
        <DllImport("dwmapi", CharSet:=CharSet.Unicode, PreserveSig:=False)> Private Shared Function DwmIsCompositionEnabled() As Boolean
        End Function


        Shared dwmapi_dll As New NativeDLL
        Shared Sub New()
            dwmapi_dll.Load("dwmapi.dll")
        End Sub

        Public Shared ReadOnly Property IsCompositionEnabled As Boolean
            Get
                If Not dwmapi_dll.IsLoad Then Return False
                Return DwmIsCompositionEnabled()
            End Get
        End Property



    End Class

End Namespace