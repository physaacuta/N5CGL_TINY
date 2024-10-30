'*********************************************************************************
'	テキストファイルにシーケンシャル出力を行う基底クラス
'	[Ver]
'		Ver.01	2009/02/26	vs2005 対応
'
'	[メモ]
'		Logファイル書込みに特化したクラス
'		ファイルの大体の容量で切替
'		高速の書き込みはダメ (ロック書き込み)
'*********************************************************************************

Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.IO								' IO操作


Namespace tClass
    Public Class tClass_CycleFileManager


        '********************************************************************
        'メンバー変数定義
        '********************************************************************

        Private m_cNowFPath As String						' 現在出力中のファイルフルパス
        Private m_Encoding As System.Text.Encoding			' エンコーディング


        'ログファイル関連
        Private m_cBaseFolder As String						' ベースのフォルダ
        Private m_cBaseFile As String						' ベースのファイル名
        Private m_cType As String							' 拡張子 (.log)

        Private m_nMaxChangeSize As Integer					' 最大ファイル切替サイズ [KByte]
        Private m_nMaxCycleCnt As Integer					' 最大サイクリックファイル件数


        '********************************************************************
        ' 処理
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="filename">ファイル名 (通常タスク名)  (Nothing時ログ出力しない)</param>
        ''' <param name="folder">フォルダパス指定 (最後の\必須) (デフォルト指定の時は、NULLでOK)</param>	
        ''' <param name="sType">拡張子 (.は必須)</param>
        ''' <param name="ChangSize">ファイル切替サイズ(目安) [KByte]</param>
        ''' <param name="cycleCnt">繰り返しファイル数</param>
        Public Sub New(ByVal filename As String, Optional ByVal folder As String = "", Optional ByVal sType As String = ".log", Optional ByVal ChangSize As Integer = 2048, Optional ByVal cycleCnt As Integer = 3)
            ''フォルダ有無
            If folder = "" Then folder = ".\LOG\"
            If Not IO.Directory.Exists(folder) Then
                IO.Directory.CreateDirectory(folder)
            End If
            m_cBaseFolder = folder
            m_cBaseFile = filename
            m_cType = sType
            m_nMaxChangeSize = ChangSize
            m_nMaxCycleCnt = cycleCnt

            '' その他項目生成
            m_cNowFPath = String.Format("{0}{1}_0{2}", m_cBaseFolder, m_cBaseFile, m_cType)
            m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")

        End Sub


        ''' <summary>
        ''' ファイルに1行書き込み
        ''' </summary>
        ''' <param name="sVal">1行分のデータ (終端の改行コードは必要)</param>
        Public Function Write(ByVal sVal As String) As Boolean
            ' ファイルチェック
            If CheckFileSize() Then
                ' ファイルの移動
                ChangeFileCycle()
            End If

            ' だれか開いていたら諦める
            Try
                ' ファイルに書き込み
                IO.File.AppendAllText(m_cNowFPath, sVal, m_Encoding)
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function


        ''' <summary>
        ''' ファイルサイズチェック
        ''' </summary>
        ''' <returns>true:切り替え</returns>
        ''' <remarks></remarks>
        Private Function CheckFileSize() As Boolean
            Try
                Dim f As New IO.FileInfo(m_cNowFPath)
                If Not f.Exists Then Return False ' そもそもファイルが無いなら、切替不要

                Dim size As Long = f.Length
                If size / 1024 > m_nMaxChangeSize Then Return True ' 指定サイズ数より大きいのでファイル切替対象

                Return False
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' ファイルの移動
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ChangeFileCycle()
            Dim sPath1 As String
            Dim sPath2 As String

            ' 最古のNoを消す
            sPath1 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, m_nMaxCycleCnt, m_cType)
            IO.File.Delete(sPath1)

            ' 8→9に変更
            For ii As Integer = m_nMaxCycleCnt - 1 To 0 Step -1
                sPath1 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, ii, m_cType)
                sPath2 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, ii + 1, m_cType)

                ' 元ファイルがある？
                If IO.File.Exists(sPath1) Then
                    ' 元ファイルを移動
                    IO.File.Move(sPath1, sPath2)
                End If
            Next ii

        End Sub

    End Class
End Namespace

