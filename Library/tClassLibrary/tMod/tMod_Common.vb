'*********************************************************************************
'共通処理のモジュール群
'	[Ver]
'		Ver.01    2005/12/25  vs2005 対応
'
'	[メモ]
'
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices          ' DllImport

Public Module GlobalCommon
    ''' <summary>
    ''' Generic対応IIf。キャストが不要
    ''' </summary>
    ''' <typeparam name="T">式の型</typeparam>
    ''' <param name="b">True･False</param>
    ''' <param name="tTrue">True時の値</param>
    ''' <param name="tFalse">False時の値</param>
    Public Function IIf(Of T)(ByVal b As Boolean, ByVal tTrue As T, ByVal tFalse As T) As T
        If b Then
            Return tTrue
        Else
            Return tFalse
        End If
    End Function

    ''' <summary>
    ''' 数値vを範囲内(min~max)におさめる。(Saturate)
    ''' </summary>
    Public Sub LimitValue(ByRef v As Integer, ByVal min As Integer, ByVal max As Integer)
        If v > max Then v = max
        If v < min Then v = min
        Return
    End Sub

    ''' <summary>
    ''' 数値vを範囲内(min~max)におさめる。(Saturate)
    ''' </summary>
    Public Sub LimitValue(ByRef v As Double, ByVal min As Double, ByVal max As Double)
        If v > max Then v = max
        If v < min Then v = min
        Return
    End Sub

    ''' <summary>
    ''' 数値vを範囲内(min~max)におさめた値をかえす
    ''' </summary>
    Public Function Saturate(ByVal v As Integer, ByVal min As Integer, ByVal max As Integer) As Integer
        If v > max Then v = max
        If v < min Then v = min
        Return v
    End Function

    ''' <summary>
    ''' 数値vを範囲内(min~max)におさめた値をかえす
    ''' </summary>
    Public Function Saturate(ByVal v As Double, ByVal min As Double, ByVal max As Double) As Double
        If v > max Then v = max
        If v < min Then v = min
        Return v
    End Function
End Module

Namespace tMod
    ''' <summary>
    ''' 共通処理のモジュール群
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_Common



        '********************************************************************
        'メニューバー の ×ボタン無効
        '********************************************************************
        ' メニューバー の ×ボタン無効処理で使用
        Private Declare Function GetSystemMenu Lib "user32.dll" Alias "GetSystemMenu" (ByVal hwnd As IntPtr, ByVal bRevert As Boolean) As IntPtr
        Private Declare Function RemoveMenu Lib "user32.dll" Alias "RemoveMenu" (ByVal hMenu As IntPtr, ByVal nPosition As Integer, ByVal wFlags As Integer) As Integer
        Private Const SC_CLOSE As Long = &HF060
        Private Const MF_BYCOMMAND As Long = &H0


        ''' <summary>
        ''' メニューバー の ×ボタン無効
        ''' </summary>
        ''' <param name="hForm">フォームハンドル (Me.Handle)</param>
        ''' <remarks></remarks>
        Public Sub subFormEndButton_Desebled(ByVal hForm As IntPtr)
            'ウィンドウハンドルもともにメニューを無効化
            Dim ptr As IntPtr
            ptr = GetSystemMenu(hForm, False)
            RemoveMenu(ptr, SC_CLOSE, MF_BYCOMMAND)
        End Sub

        '********************************************************************
        'コンピューターの設定を変更する
        '********************************************************************
        Private Declare Function SetComputerName Lib "kernel32" Alias "SetComputerNameA" (ByVal strName As String) As Integer
        'Private Declare Function GetComputerName Lib "kernel32" Alias "GetComputerNameA" (ByVal strName As String) As Integer

        ''' <summary>
        ''' コンピュータ名を変更する
        ''' </summary>
        ''' <param name="strNewNetBiosComputerName"></param>
        ''' <returns></returns>
        Public Function SetNetBiosComputerName(ByVal strNewNetBiosComputerName As String) As Boolean
            '//// コンピューター名を変更する
            '// ※ ただし、NetBIOSの名前ね。 (システムのプロパティ / コンピュータ名 / 変更 / 詳細 に出て来る名前)
            Dim retc As Integer = SetComputerName(strNewNetBiosComputerName)
            Return CBool(IIf(1 = retc, True, False))
        End Function

        ''' <summary>
        ''' コンピューター名を取得する
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function GetNetBiosComputerName() As String
            Return System.Environment.MachineName
        End Function


        ''' <summary>
        ''' IPアドレスの変更を行う
        ''' </summary>
        ''' <param name="Ip">変更を行うIPアドレス (127.0.0.1形式)</param>
        ''' <param name="AdapterName">変更対象のアダプタ名称 (ネットワーク接続プロパティで表示されている名称を指定)</param>
        ''' <param name="SubNetMask">変更を行うサブネットマスク</param>
        Public Sub SetIpAddress(ByVal Ip As String, Optional ByVal AdapterName As String = "ローカル エリア接続", Optional ByVal SubNetMask As String = "255.255.255.0")
            Dim strCmd As String = ""        ' 生成するDOSコマンド

            '''' コマンド生成
            strCmd = "netsh interface ip set "
            strCmd &= "address name = """ & AdapterName & """ "         '対象の奴
            strCmd &= "source=static addr=" & Ip & " "
            strCmd &= "mask=" & SubNetMask & ""

            '''' コマンド実行
            Shell(strCmd, AppWinStyle.Hide)
        End Sub




        '********************************************************************
        '文字列関連
        '********************************************************************
        ''' <summary>
        ''' 文字列のバイト数を返す
        ''' </summary>
        ''' <param name="strVal">文字列</param>
        ''' <returns>バイト数</returns>
        ''' <remarks></remarks>
        Public Function LenB(ByVal strVal As String) As Integer
            Return System.Text.Encoding.GetEncoding("shift_jis").GetByteCount(strVal)
        End Function

        ''' <summary>
        ''' バイト配列から文字列に変換
        ''' </summary>
        ''' <param name="val">バイト配列</param>
        ''' <returns>文字列</returns>
        ''' <remarks></remarks>
        Public Function ByteToString(ByVal val() As Byte) As String
            Dim nEnd As Integer = 0
            Dim bJis As Boolean = False

            'For ii As Integer = 0 To val.Length - 1
            '	If 0 = val(ii) Then
            '		Exit For
            '	End If
            '	If val(ii) > &H7F Then bJis = True
            '	nEnd += 1
            'Next

            Dim wk() As Byte
            ReDim wk(val.Length - 1)
            For ii As Integer = 0 To val.Length - 1
                If 0 = val(ii) Then
                    Exit For
                End If
                If val(ii) > &H7F Then bJis = True
                wk(ii) = val(ii)
                nEnd += 1
            Next

            ' たぶんこんなめんどくさいことをしなくても、全部shift-jisにしても問題ないと思うが、既存影響を調査がないため、暫定処置
            If bJis Then
                Return System.Text.Encoding.GetEncoding("shift_jis").GetString(wk)
            Else
                Return System.Text.Encoding.ASCII.GetString(wk)
            End If
        End Function
        ''' <summary>
        ''' 文字列からバイト配列に変換
        ''' </summary>
        ''' <param name="val">文字列</param>
        ''' <returns>バイト配列</returns>
        ''' <remarks></remarks>
        Public Function StringToByte(ByVal val As String) As Byte()
            Return System.Text.Encoding.ASCII.GetBytes(val)
        End Function
        ''' <summary>
        ''' メールスロット等で使用するときに、バイト配列領域は変えたくない場合に使用する。
        ''' </summary>
        ''' <param name="ans">バイト配列</param>
        ''' <param name="val">文字列</param>
        ''' <param name="size">サイズ</param>
        ''' <remarks></remarks>
        Public Sub StringToByte(ByVal ans As Byte(), ByVal val As String, ByVal size As Integer)
            '変換サイズ決定
            Dim iwk As Integer = val.Length
            If iwk > size Then iwk = size
            '変換
            System.Text.Encoding.ASCII.GetBytes(val, 0, iwk, ans, 0)
        End Sub
        Public Sub StringToByte(ByVal ans As Byte(), ByVal val As String)
            StringToByte(ans, val, ans.Length)
        End Sub

        ''' <summary>
        ''' 渡された文字列の半角全角をチェック
        ''' </summary>
        ''' <param name="val"></param>
        ''' <returns>-1:いろいろ混在 0:半角のみ 1:全角のみ</returns>
        Public Function CheckStringType(ByVal val As String) As Integer
            Dim nCntStr As Integer = val.Length
            Dim nCntByte As Integer = System.Text.Encoding.GetEncoding("shift_jis").GetByteCount(val)

            ' 各サイズチェック
            If nCntStr * 2 = nCntByte Then
                Return 1
            ElseIf nCntStr = nCntByte Then
                Return 0
            Else
                Return -1
            End If
        End Function

        '********************************************************************
        'ファイル/フォルダ関連
        '********************************************************************

        ''' <summary>
        ''' フォルダ毎のコピー (Frameworkでは、準備されていないから自作する)
        ''' </summary>
        ''' <param name="strPathFrom">コピー元パス (最後の\は無し)</param>
        ''' <param name="strPathTo">コピー先パス (最後の\は無し)</param>
        ''' <param name="bOverWrite">同一ファイルがある場合上書きする:true</param>
        ''' <param name="bSubFolder">サブフォルダがある場合コピーする:true</param>
        Public Sub CopyDirectory(ByVal strPathFrom As String, ByVal strPathTo As String, ByVal bOverWrite As Boolean, ByVal bSubFolder As Boolean)
            '''' コピー先のディレクトリを作成
            If System.IO.Directory.Exists(strPathTo) = False Then
                IO.Directory.CreateDirectory(strPathTo)
                IO.File.SetAttributes(strPathTo, IO.File.GetAttributes(strPathFrom))
                'bOverWrite = True
            End If

            '''' コピー元のディレクトリにあるすべてのファイルをコピー
            For Each strCopyFrom As String In System.IO.Directory.GetFiles(strPathFrom)
                Dim strCopyTo As String = System.IO.Path.Combine(strPathTo, System.IO.Path.GetFileName(strCopyFrom))

                If bOverWrite Then
                    System.IO.File.Copy(strCopyFrom, strCopyTo, bOverWrite) '上書き許可
                Else
                    If Not IO.File.Exists(strCopyTo) Then
                        System.IO.File.Copy(strCopyFrom, strCopyTo, bOverWrite) '上書き不許可
                    End If
                End If


            Next strCopyFrom

            '''' コピー元のディレクトリ内にあるサブフォルダ
            If bSubFolder Then
                For Each strCopyFrom As String In System.IO.Directory.GetDirectories(strPathFrom)
                    Dim stCopyTo As String = System.IO.Path.Combine(strPathTo, System.IO.Path.GetFileName(strCopyFrom))

                    CopyDirectory(strCopyFrom, stCopyTo, bOverWrite, bSubFolder)
                Next strCopyFrom
            End If
        End Sub

        ''' <summary>
        ''' 指定フォルダが無い場合は生成する
        ''' </summary>
        ''' <param name="strFolder">チェック対象のフォルダパス</param>
        ''' <remarks></remarks>
        Public Sub CheckFolder(ByVal strFolder As String)
            '''' 指定フォルダが有るかチェック
            If IO.Directory.Exists(strFolder) Then Return
            '''' 指定フォルダ生成
            Try
                IO.Directory.CreateDirectory(strFolder)
            Catch ex As Exception
                ' ネットワークフォルダの場合などでそもそもつながっていない場合
            End Try
        End Sub


        '********************************************************************
        '個々のモジュール
        '********************************************************************

        ''' <summary>
        ''' 普通(RGB)の色コードから Colorクラスに変換
        ''' </summary>
        ''' <param name="intRBG">RGB形式</param>
        ''' <returns>Colorクラス</returns>
        ''' <remarks></remarks>
        Public Function GetIntToColor(ByVal intRBG As Integer) As System.Drawing.Color
            Return System.Drawing.Color.FromArgb(System.Drawing.Color.FromArgb(intRBG).R, _
                      System.Drawing.Color.FromArgb(intRBG).G, _
                      System.Drawing.Color.FromArgb(intRBG).B)
        End Function

        ''' <summary>
        ''' Colorクラス から 普通(RGB)の色コードに変換
        ''' </summary>
        ''' <param name="clr">Colorクラス</param>
        ''' <returns>RGB形式</returns>
        ''' <remarks></remarks>
        Public Function GetColorToInt(ByVal clr As System.Drawing.Color) As Integer
            Return System.Drawing.Color.FromArgb(clr.R, clr.G, clr.B).ToArgb
        End Function

        ''' <summary>
        ''' VB6(BGR)の色コードから Colorクラスに変換
        ''' </summary>
        ''' <param name="intGBR">RGB形式(VB6の色コード)</param>
        ''' <returns>Colorクラス</returns>
        ''' <remarks></remarks>
        Public Function GetIntToColor_VB6(ByVal intGBR As Integer) As System.Drawing.Color
            Return System.Drawing.Color.FromArgb(System.Drawing.Color.FromArgb(intGBR).B, _
             System.Drawing.Color.FromArgb(intGBR).G, _
             System.Drawing.Color.FromArgb(intGBR).R)
        End Function

        ''' <summary>
        ''' Colorクラス から VB6(BGR)の色コードに変換
        ''' </summary>
        ''' <param name="clr">Colorクラス</param>
        ''' <returns>BGR形式</returns>
        ''' <remarks></remarks>
        Public Function GetColorToInt_VB6(ByVal clr As System.Drawing.Color) As Integer
            Return System.Drawing.Color.FromArgb(clr.B, clr.G, clr.R).ToArgb
        End Function


        ''' <summary>
        ''' ダンプ出力関数
        ''' </summary>
        ''' <param name="strFileName">ダンプファイル名</param>
        ''' <param name="strFolder">出力フォルダ(最後の\Dumpはいらない)</param>
        ''' <param name="len">出力データサイズ</param>
        ''' <param name="data">データ</param>
        ''' <param name="addTime">ダンプファイル名に日付を付与するしない</param>
        ''' <remarks>データ行でない部分は 先頭に # をつける事。(疵検ダンプファイル専用フォーマット)</remarks>
        Public Sub DumpData(ByVal strFileName As String, ByVal strFolder As String, ByVal len As Integer, ByVal data As Byte(), ByVal addTime As Boolean)
            Dim strPath As String

            '' フォルダ有無
            If "" = strFolder Then
                strFolder = ".\Dump"
            Else
                strFolder = strFolder & "\Dump"
            End If
            '' ログフォルダ生成
            CheckFolder(strFolder)

            '' ファイルオープン
            strFolder &= "\"
            If addTime Then             ' 日付付与
                strPath = strFolder & Now.ToString("yyyyMMdd_hhmmss_fff_") & strFileName & ".log"
            Else
                strPath = strFolder & strFileName & ".log"
            End If
            Dim os As New IO.StreamWriter(strPath, False, System.Text.Encoding.GetEncoding("shift-jis"))

            '' ダンプ出力
            Dim wk As String
            Dim ii As Integer
            Dim rec As Integer = 0

            os.WriteLine("# {0}[byte]送受信", len)
            os.WriteLine("# (Hex)ADDR(Dec)  :  0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F | [ASCII]")

            While True
                If rec = len Then Exit While ' 最後まで到達

                os.Write("{0:X7} ({1:D7}) :", rec, rec)

                wk = " | "
                For ii = 0 To 15
                    If ii = 8 Then os.Write(" -")
                    os.Write(" {0:X2}", data(rec))

                    If data(rec) < &H20 Or data(rec) > &H7F Then
                        wk &= "･"
                    Else
                        wk &= Chr(data(rec))
                    End If

                    ' 次
                    rec += 1
                    If rec = len Then   ' 最終データ
                        '余白分設定
                        For jj As Integer = ii + 1 To 15
                            If jj = 8 Then os.Write(" -")
                            os.Write("   ")
                        Next
                        os.WriteLine(wk)
                        Exit For
                    End If
                    If ii = 15 Then     ' 列最後
                        os.WriteLine(wk)
                        Exit For
                    End If
                Next ii
            End While
            os.WriteLine()
            os.Flush()
            os.Close()
            os.Dispose()
        End Sub


        ''' <summary>
        ''' 指定モニターNo左上の絶対位置
        ''' </summary>
        ''' <param name="nMonitorNo">モニタ番号 (1オリジン)</param>
        ''' <returns>絶対位置 [pixel]</returns>
        ''' <remarks></remarks>
        Public Function ScreenToLocation(ByVal nMonitorNo As Integer) As System.Drawing.Point

            '' 表示したいモニタの情報にあわせてフォームサイズを変更
            Dim monitor() As System.Windows.Forms.Screen
            monitor = System.Windows.Forms.Screen.AllScreens()

            nMonitorNo -= 1 'なぜか昔から1オリジンになっていた為、互換性の為にそのままとする

            '' 番号チェック
            If 0 > nMonitorNo Or nMonitorNo >= monitor.Length Then
                nMonitorNo = 0
            End If

            Return monitor(nMonitorNo).Bounds.Location
        End Function

        '********************************************************************
        ' C++の使いやすいモジュールの展開
        '********************************************************************
        ''' <summary>
        ''' 欠陥優先度比較 Ex (各変数配列に情報をセット)
        ''' </summary>
        ''' <param name="pNow">比較する新情報を順番にセット(配列の先頭が最優先) </param>
        ''' <param name="pPre">比較する旧情報を順番にセット(配列の先頭が最優先) </param>
        ''' <param name="bMode">比較条件 (true:新旧比較したときに大きい方が強い。 false:新旧比較したときに小さい方が強い)</param>
        ''' <returns>true:新欠陥の方が強い  false:旧欠陥の方が強い</returns>
        Public Function SelectPriorityDetect(ByVal pNow() As Double, ByVal pPre() As Double, ByVal bMode() As Boolean) As Boolean
            For ii As Integer = 0 To pNow.Length - 1
                If pNow(ii) = pPre(ii) Then Continue For

                If bMode(ii) Then
                    '// 大きい方が強い
                    If pNow(ii) > pPre(ii) Then
                        Return True
                    Else
                        Return False
                    End If
                Else
                    '// 小さい方が強い
                    If pNow(ii) < pPre(ii) Then
                        Return True
                    Else
                        Return False
                    End If
                End If
            Next
            Return False    '// 全部一緒なら 旧欠陥を優先
        End Function


        ''' <summary>
        ''' nValサイズをnPitchの倍数に揃える(必ず、nValのままか、ちょっと大きくなる方向)
        ''' </summary>
        ''' <param name="nVal">元サイズ</param>
        ''' <param name="nPitch">倍数</param>
        ''' <returns>結果</returns>
        Public Function GET_SIZE_JUST(ByVal nVal As Integer, ByVal nPitch As Integer) As Integer
            Return ((nVal - 1) \ nPitch + 1) * nPitch
        End Function
        ''' <summary>
        ''' 割り切り処理
        ''' </summary>
        ''' <param name="nVal">値</param>
        ''' <param name="nPitch">ピッチ</param>
        ''' <returns>5,5 の場合は、0となる</returns>
        Public Function JUST_SIZE(ByVal nVal As Integer, ByVal nPitch As Integer) As Integer
            Return (nVal - 1) \ nPitch
        End Function

    End Module
End Namespace