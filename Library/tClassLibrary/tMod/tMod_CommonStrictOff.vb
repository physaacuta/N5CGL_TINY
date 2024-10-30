'*********************************************************************************
'共通処理のモジュール群
'	[Ver]
'		Ver.01    2005/12/25  vs2005 対応
'
'	[メモ]
'		Option Strict Off 専用
'*********************************************************************************
Option Strict Off								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices			' DllImport


Namespace tMod
    ''' <summary>
    ''' 共通処理のモジュール群
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_CommonStrictOff
        ''' <summary>
        ''' ショートカット作成
        ''' </summary>
        ''' <param name="strCreateLinkName">ショートカットの生成ファイル名 (フルパス) (拡張子は.lnk)</param>
        ''' <param name="strTagetPath">ショートカット先のパス</param>
        ''' <param name="strTagetExe">ショートカット先のEXEファイル名</param>
        ''' <param name="strcomment">ショートカットのコメント</param>
        ''' <remarks></remarks>
        Public Function CreateShortCut(ByVal strCreateLinkName As String, ByVal strTagetPath As String, ByVal strTagetExe As String, Optional ByVal strcomment As String = "") As Boolean
            'Dim objShell As Object = CreateObject("WScript.Shell")
            'Dim objLink As Object = objShell.CreateShortcut("C:\test.lnk") 'ショートカットのファイル名

            'With objLink
            '	.targetpath = "c:\install\KS_INSTAL.exe"
            '	.description = "testtest"
            '	.iconlocation = "c:\install\KS_INSTAL.exe"
            '	.workingdirectory = "c:\install"
            '	.save()
            'End With

            Try
                Dim objShell As Object = CreateObject("WScript.Shell")
                Dim objLink As Object = objShell.CreateShortcut(strCreateLinkName) 'ショートカットのファイル名

                With objLink
                    .targetpath = IO.Path.Combine(strTagetPath, strTagetExe)
                    .iconlocation = IO.Path.Combine(strTagetPath, strTagetExe)
                    .description = strcomment
                    .workingdirectory = strTagetPath
                    .save()
                End With
                Return True

            Catch ex As Exception
                Return False
            End Try
        End Function
    End Module
End Namespace