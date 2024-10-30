Imports tClassLibrary

Public Class frmMain
    ' 定数
    Private Const nWriteFileNum As Integer = 5          ' Insertするﾌｧｲﾙの数（1～）

    ' アイテム
    Private m_objFilePath As TextBox()                  ' バイナリデータへのパス
    Private m_objFileLabel As Label()                   ' バイナリデータのラベル
    Private m_objFileBtn As Button()                    ' ファイル選択ボタン

    ''' <summary>
    ''' ロード
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' バイナリデータ数が0以下なら終了
        If 1 > nWriteFileNum Then
            MsgBox("Error!!" & Environment.NewLine & " nWriteFileNumが0以下!!", MsgBoxStyle.Critical)
            End
        End If

        Const nYohaku As Integer = 5

        m_objFilePath = New TextBox(nWriteFileNum - 1) {}                   ' バイナリデータへのパス
        m_objFileLabel = New Label(nWriteFileNum - 1) {}                    ' バイナリデータのラベル
        m_objFileBtn = New Button(nWriteFileNum - 1) {}                     ' ファイル選択ボタン

        ' バイナリデータ選択用アイテムの追加
        Dim ii As Integer
        For ii = 0 To nWriteFileNum - 1

            ' //////////////
            '' ラベル （@F1,@F2,・・・）
            m_objFileLabel(ii) = New Label
            BinDataGroup.Controls.Add(m_objFileLabel(ii))
            With m_objFileLabel(ii)
                .Text = "@F" & (ii + 1)
                .Top = CInt((.Height + nYohaku) * ii + BinDataGroup.Font.Size + nYohaku)
                .Left = nYohaku
                .Width = CInt(.Text.Length * .Font.Size + nYohaku)
            End With

            ' //////////////
            '' […]ボタン 
            m_objFileBtn(ii) = New Button
            BinDataGroup.Controls.Add(m_objFileBtn(ii))
            With m_objFileBtn(ii)
                .Text = "…"
                .Top = m_objFileLabel(ii).Top
                .Left = m_objFileLabel(ii).Right + nYohaku
                .Width = CInt(.Text.Length * .Font.Size + nYohaku * 2)
                AddHandler .Click, AddressOf m_objFileBtn_Click                     ' ボタン押下時のイベント
            End With

            ' //////////////
            '' テキストボックス
            m_objFilePath(ii) = New TextBox
            BinDataGroup.Controls.Add(m_objFilePath(ii))
            With m_objFilePath(ii)
                .Text = ""
                .Top = m_objFileLabel(ii).Top
                .Left = m_objFileBtn(ii).Right + nYohaku
                .Width = BinDataGroup.Width - .Left - nYohaku
                .AllowDrop = True                                                   ' ドラッグ&ドロップを有効にするための識別子
                AddHandler .DragEnter, AddressOf m_objFilePath_DragEnter            ' ファイルドラッグ時のイベント
                AddHandler .DragDrop, AddressOf m_objFilePath_DragDrop              ' ファイルドロップ時のイベント
            End With
        Next

        ' 追加した分だけ他のアイテムをずらす
        Dim nWk As Integer
        nWk = BinDataGroup.Height
        BinDataGroup.Height = m_objFileLabel(nWriteFileNum - 1).Bottom + nYohaku    ' バイナリデータ読込
        nWk = BinDataGroup.Height - nWk
        SqlGroup.Top += nWk                                                         ' SQL操作
        Me.Height += nWk                                                            ' ダイアログ

        ' 相対パスのカレントディレクトリを実行ファイルのディレクトリに初期化
        System.Environment.CurrentDirectory = _
            System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ バイナリファイル 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' バイナリファイル入力
    ''' </summary>
    Private Sub m_objFileBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim idx As Integer = GetIndex(m_objFileBtn, sender)
        ''
        ' ファイル選択
        Dim dlg As New OpenFileDialog
        dlg.Title = "ファイル選択"
        dlg.Multiselect = False
        dlg.ShowHelp = False
        dlg.ShowReadOnly = False
        dlg.InitialDirectory = ""
        dlg.FileName = ""
        dlg.ShowDialog()

        If "" <> dlg.FileName Then
            ' バイナリファイルへのパス変更
            m_objFilePath(idx).Text = dlg.FileName
        End If
    End Sub

    ''' <summary>
    ''' テキストボックスにファイルドラッグ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_objFilePath_DragEnter(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs)
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            ' ファイルをドラッグ
            e.Effect = DragDropEffects.Copy
        Else
            ' それ以外をドラッグ
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' テキストボックスにファイルドロップ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_objFilePath_DragDrop(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs)
        Dim idx As Integer = GetIndex(m_objFilePath, sender)      ' 選択されたテキストボックスのインデックス
        ' idxが-1の時はReturn
        If -1 = idx Then Return
        ''
        ' ファイルまでのフルパス
        Dim strFName() As String = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        ''
        ' バイナリファイルへのパス変更
        m_objFilePath(idx).Text = strFName(0)
    End Sub

    ''' <summary>
    ''' バイナリファイルをローカル変数に格納
    ''' </summary>
    ''' <param name="strFileName">ファイルへのフルパス</param>
    ''' <param name="bData">バイナリデータ</param>
    ''' <remarks></remarks>
    Private Function SetBinData(ByVal strFileName As String, ByRef bData As Byte()) As Boolean
        If System.IO.File.Exists(strFileName) Then
            ' ファイル読込
            Using os As New IO.FileStream(strFileName, IO.FileMode.Open, IO.FileAccess.Read)
                Dim bWk(CInt(os.Length) - 1) As Byte
                os.Read(bWk, 0, CInt(os.Length))
                bData = bWk
            End Using
            Return True
        Else
            Return False
        End If
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ データベース 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 書込ボタンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnDbWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDbWrite.Click
        ' 確認
        If MsgBox("DBに書込みますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "DB書込確認") = MsgBoxResult.No Then Return

        '' 整合性チェック
        If 0 > CheckIntegrity() Then
            Return
        End If

        '' ループ機能の整合性チェック
        Dim nRet As Integer = ChkLoop()
        If 0 > nRet Then
            Return
        End If

        ' バイナリデータリスト作成
        Dim para As New List(Of SqlClient.SqlParameter)              ' バイナリデータ
        If False = PutSqlParameter(para) Then
            Return
        End If


        ' //////////////
        ' DB接続＆SQL文実行
        If 1 = nRet Then
            'If 0 <> ChkSql() Then
            '    lblStatus.Text = "ループ機能を使用するときはINSERTで実行してください"
            '    Return
            'End If
            Dim nStart As Integer = CInt(txtLoopStart.Text)
            Dim nEnd As Integer = CInt(txtLoopEnd.Text)
            ' SQLループ実行
            If 0 > nExecSql(para, nStart, nEnd) Then
                Return
            End If
        ElseIf 0 = nRet Then
            ' SQL実行
            If 0 > ExecSql(para) Then
                Return
            End If
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL文 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INSERT定型文出力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>INSERT INTO $t VALUES( *,*,* );</remarks>
    Private Sub btnInsert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInsert.Click
        ' 上書き確認
        If "" <> txtDbWriteSql.Text Then
            If MsgBox("SQL文をINSERT定型文に上書きしてもよろしいですか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "上書き確認") = MsgBoxResult.No Then Return
        End If

        ' SQL文
        Dim strWk As String = ""
        strWk &= "INSERT INTO " & txtTableName.Text & Environment.NewLine
        strWk &= "VALUES( "
        Dim ii As Integer
        For ii = 0 To m_objFilePath.Length - 1
            If System.IO.File.Exists(m_objFilePath(ii).Text) Then
                strWk &= m_objFileLabel(ii).Text & ", "
            End If
        Next
        strWk = strWk.TrimEnd(" "c, ","c)
        strWk &= " ); " & Environment.NewLine

        txtDbWriteSql.Text = strWk

    End Sub

    ''' <summary>
    ''' UPDATE定型文出力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>UPDATE $t SET [**]=***  WHERE *****</remarks>
    Private Sub btnUpdate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpdate.Click
        ' 上書き確認
        If "" <> txtDbWriteSql.Text Then
            If MsgBox("SQL文をUPDATE定型文に上書きしてもよろしいですか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "上書き確認") = MsgBoxResult.No Then Return
        End If

        ' SQL文
        Dim strWk As String = ""
        strWk &= "UPDATE " & txtTableName.Text & Environment.NewLine
        strWk &= "SET "
        Dim ii As Integer
        For ii = 0 To m_objFilePath.Length - 1
            If System.IO.File.Exists(m_objFilePath(ii).Text) Then
                strWk &= Environment.NewLine
                strWk &= "[] = " & m_objFileLabel(ii).Text & ","

            End If
        Next
        strWk = strWk.TrimEnd(","c)
        strWk &= Environment.NewLine
        strWk &= "WHERE ;"

        txtDbWriteSql.Text = String.Copy(strWk)
    End Sub

    ''' <summary>
    ''' SQL文内のコメントを取り除く
    ''' </summary>
    ''' <param name="strsql">トリム対象SQL文</param>
    ''' <returns>コメント文を取り除いたSQL文</returns>
    ''' <remarks>すべての"--"から改行まで or "/*"から"*/"までの文字列を削除する</remarks>
    Private Function TrimComments(ByVal strsql As String) As String
        Dim strWk As String = strsql
        Dim stcWk As Stack(Of Integer) = New Stack(Of Integer)
        Dim nStart As Integer = 0
        Dim nEnd As Integer = 0
        Dim com1 As String = "(/\*.*\*/)|(--)"        ' コメント文の有無を検知するための正規表現
        Dim com2 As String = "(/\*)|(\*/)|(--)"       ' コメント文を開始、終了を検知するための正規表現

        ' SQL文内の"/*","*/","--"を全取得
        Dim m As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
        ' SQL文内にコメント文が無くなるまでループ
        While System.Text.RegularExpressions.Regex.IsMatch(strWk, com1, System.Text.RegularExpressions.RegexOptions.Compiled Or System.Text.RegularExpressions.RegexOptions.Singleline)
            If "/*" = m.Value Then                  ' /*の場合
                stcWk.Push(m.Index)
                ' 次のコメント
                m = m.NextMatch
            ElseIf "*/" = m.Value Then              ' */の場合
                If 1 = stcWk.ToArray.Length Then    ' スタック内のアイテムが一つ
                    ' 開始、終了位置設定
                    nStart = stcWk.Pop()
                    nEnd = m.Index - nStart + 2
                    strWk = strWk.Remove(nStart, nEnd)
                    ' 次のコメント、再検索
                    m = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
                ElseIf 1 < stcWk.ToArray.Length Then ' スタック内に2つ以上のアイテム
                    stcWk.Pop()
                    ' 次のコメント
                    m = m.NextMatch
                Else
                    ' 次のコメント
                    m = m.NextMatch
                End If
            ElseIf "" = m.Value Then    ' 永久ループ回避用IF
                nStart = strWk.IndexOf("/*")
                strWk = strWk.Remove(nStart)
                Exit While
            Else                                    ' --の場合
                If 0 = stcWk.ToArray.Length Then    ' スタック内にアイテムあり
                    nStart = m.Index
                    nEnd = strWk.Substring(m.Index).IndexOf(Environment.NewLine)
                    If 0 >= nEnd Then
                        strWk = strWk.Remove(nStart)
                    Else
                        strWk = strWk.Remove(nStart, nEnd)
                    End If

                    ' 次のコメント、再検索
                    m = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
                Else
                    ' 次のコメント
                    m = m.NextMatch
                End If
            End If
        End While
        Return strWk
    End Function


    ''' <summary>
    ''' SQL文チェック
    ''' </summary>
    ''' <returns>チェック結果(0:INSERT,1:UPDATE,-1:それ以外)</returns>
    ''' <remarks>改行、空白以外の最初の文字列で判断</remarks>
    Private Function ChkSql() As Integer
        Dim strWk As String = TrimComments(txtDbWriteSql.Text)
        If strWk.Replace(Environment.NewLine, " ").Trim.StartsWith("INSERT ", StringComparison.CurrentCultureIgnoreCase) Then
            Return 0
        ElseIf strWk.Replace(Environment.NewLine, " ").Trim.StartsWith("UPDATE ", StringComparison.CurrentCultureIgnoreCase) Then
            Return 1
        Else
            Return -1
        End If
    End Function

    ''' <summary>
    ''' ループ機能の整合性チェック
    ''' </summary>
    ''' <returns>チェック結果(0:非ループ文,1:ループ文,-1:開始空白,-2:終了空白,-3:開始数字以外,-4:終了数字以外,-5:開始>終了)</returns>
    ''' <remarks></remarks>
    Private Function ChkLoop() As Integer
        Dim strWk As String = TrimComments(txtDbWriteSql.Text)

        If Not strWk.Contains("$n") Then
            ' SQL文内に$nの文字がない
            Return 0
        Else
            ' ////////////////////////////////
            ' 開始、終了に文字が入力されていない
            If "" = txtLoopStart.Text Then
                lblStatus.Text = "ループ開始値が設定されていません"
                Return -1
            End If
            If "" = txtLoopEnd.Text Then
                lblStatus.Text = "ループ終了値が設定されていません"
                Return -2
            End If

            ' ////////////////////////////////
            ' 開始、終了に数字以外の文字が入力されている
            If System.Text.RegularExpressions.Regex.IsMatch(txtLoopStart.Text, "[^0-9]+") Then
                lblStatus.Text = "ループ開始値に数字以外の文字が入力されています"
                Return -3
            End If
            If System.Text.RegularExpressions.Regex.IsMatch(txtLoopEnd.Text, "[^0-9]+") Then
                lblStatus.Text = "ループ終了値に数字以外の文字が入力されています"
                Return -4
            End If

            ' ////////////////////////////////
            ' ループ開始値よりもループ終了値の方が小さい
            If CInt(txtLoopStart.Text) > CInt(txtLoopEnd.Text) Then
                lblStatus.Text = "ループ開始値 > ループ終了値になってます"
                Return -5
            End If
            Return 1
        End If

    End Function
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' インデックス取得
    ''' </summary>
    ''' <param name="objBox">オブジェクト配列</param>
    ''' <param name="sender">選択オブジェクト</param>
    ''' <returns>インデックス番号(異常時:-1)</returns>
    ''' <remarks>バイナリデータ読込グループ専用</remarks>
    Private Function GetIndex(ByVal objBox() As Object, ByVal sender As System.Object) As Integer
        Dim index As Integer = -1           ' インデックス
        Dim ii As Integer                   ' ループ変数
        For ii = 0 To objBox.Length - 1
            If objBox(ii).Equals(sender) Then
                index = ii
                Exit For
            End If
        Next
        Return index
    End Function

    ''' <summary>
    ''' 整合性チェック
    ''' </summary>
    ''' <returns>チェック結果(0:正常,-1:アドレス未入力,-2:DB名未入力,-3:ユーザー未入力,-4:パス未入力,-5:UPDATE,INSERT以外)</returns>
    ''' <remarks></remarks>
    Private Function CheckIntegrity() As Integer
        ' 変数定義
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ｱﾄﾞﾚｽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' データベース名
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ユーザー
        Dim strPass As String = txtDbWritePass.Text.Trim                ' パス

        ' ////////////////////////////////
        '  整合性チェック
        '' //////////////////
        '' DB情報関連チェック
        '' ｱﾄﾞﾚｽ
        If "" = strAddless Then
            lblStatus.Text = "addressが未入力です"
            Return -1
        End If
        '' データベース名
        If "Database_" = strDbName Or "" = strDbName Then
            lblStatus.Text = "DB名が未入力です"
            Return -2
        End If
        '' ユーザー
        If "" = strUser Then
            lblStatus.Text = "User名が未入力です"
            Return -3
        End If
        '' パス
        If "" = strPass Then
            lblStatus.Text = "Passが未入力です"
            Return -4
        End If

        '' //////////////////
        '' SQL文関連チェック
        '' INSERTかUPDATEで始まっていない文は実行しない
        If 0 > ChkSql() Then
            lblStatus.Text = "UPDATE、INSERT文以外のSQL文を実行しようとしてます"
            Return -5
        End If

        Return 0
    End Function

    ''' <summary>
    ''' バイナリデータリスト作成
    ''' </summary>
    ''' <param name="para">パラメータ</param>
    ''' <returns>true:作成成功,false:作成失敗</returns>
    ''' <remarks></remarks>
    Private Function PutSqlParameter(ByRef para As List(Of SqlClient.SqlParameter)) As Boolean
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL文
        Dim bWk() As Byte = New Byte() {}
        Dim ii As Integer

        Dim nChk As Integer = ChkLoop()
        For ii = 0 To nWriteFileNum - 1
            If strSql.Replace(Environment.NewLine, " ").Trim.Contains(m_objFileLabel(ii).Text) Then
                bWk = New Byte() {}
                If False = SetBinData(m_objFilePath(ii).Text, bWk) Then
                    lblStatus.Text = m_objFileLabel(ii).Text & "のパスの先にファイルが存在しません"
                    Return False
                End If
                '' スカラ変数名は@F1,@F2,@F3,…
                para.Add(New SqlClient.SqlParameter(m_objFileLabel(ii).Text, bWk))
            ElseIf nChk = 1 And strSql.Replace(Environment.NewLine, " ").Trim.Contains("@F$n") Then
                If CInt(txtLoopStart.Text) <= ii + 1 And CInt(txtLoopEnd.Text) >= ii + 1 Then
                    bWk = New Byte() {}
                    If False = SetBinData(m_objFilePath(ii).Text, bWk) Then
                        lblStatus.Text = m_objFileLabel(ii).Text & "のパスの先にファイルが存在しません"
                        Return False
                    End If
                    '' スカラ変数名は@F1,@F2,@F3,…
                    para.Add(New SqlClient.SqlParameter(m_objFileLabel(ii).Text, bWk))
                End If
            End If
        Next
        Return True
    End Function

    ''' <summary>
    ''' パラメータ付SQL文実行
    ''' </summary>
    ''' <param name="para">パラメータ</param>
    ''' <returns>実行結果(0:正常,-1:SQL実行失敗,-2:影響範囲0行,-3:処理中断,-9:DB接続失敗)</returns>
    ''' <remarks></remarks>
    Private Function ExecSql(ByVal para As List(Of SqlClient.SqlParameter)) As Integer
        ' 変数定義
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ｱﾄﾞﾚｽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' データベース名
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ユーザー
        Dim strPass As String = txtDbWritePass.Text.Trim                ' パス
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL文

        '' DBクラス
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        Try
            '' DB接続
            If Not tcls_db.Open() Then
                lblStatus.Text = "DB接続に失敗しました"
                Return -9
            End If

            Dim ii As Integer
            ' SQL文（パラメータ付き）実行
            Dim nResult As Integer
            Dim strWk As String = strSql.Replace("$n", CStr(ii))

            '' //////////////////
            '' 実行後の影響範囲取得
            ' トランザクション開始
            tcls_db.TransactStart()
            ' SQL文実行
            nResult = tcls_db.DMLExecute(strWk, para.ToArray)
            ' ロールバック
            tcls_db.TransactRollback()
            ' 影響範囲チェック
            If -1 = nResult Then
                lblStatus.Text = "SQL文実行失敗"
                Return -1
            ElseIf 0 = nResult Then
                lblStatus.Text = "影響範囲が0行でした"
                Return -2
            ElseIf 1 < nResult Then
                ' 影響範囲が2行以上のときはコミットするか確認
                If MsgBox(nResult & "行に影響が出る可能性があります。よろしいでしょうか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "書込中断確認") = MsgBoxResult.No Then
                    lblStatus.Text = "処理を中止しました"
                    Return -3
                End If
            End If

            ' SQL文実行
            nResult = tcls_db.DMLExecute(strWk, para.ToArray)
            ' 影響範囲チェック
            If -1 = nResult Then
                lblStatus.Text = "SQL文実行失敗"
                Return -1
            ElseIf 0 = nResult Then
                lblStatus.Text = "影響範囲が0行でした"
                Return -2
            End If
            lblStatus.Text = "SQL文実行成功！" & nResult & "行処理されました"
            Return 0
        Catch ex As Exception
            lblStatus.Text = ex.Message
        Finally
            tcls_db.Close()
            tcls_db.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' パラメータ付SQL文ループ実行
    ''' </summary>
    ''' <param name="para">パラメータ</param>
    ''' <returns>実行結果(0:全て正常,-1:異常あり,-9:DB接続)</returns>
    ''' <remarks></remarks>
    Private Function nExecSql(ByVal para As List(Of SqlClient.SqlParameter), ByVal nStart As Integer, ByVal nEnd As Integer) As Integer
        ' 変数定義
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ｱﾄﾞﾚｽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' データベース名
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ユーザー
        Dim strPass As String = txtDbWritePass.Text.Trim                ' パス
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL文

        '' DBクラス
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        Try
            '' DB接続
            If Not tcls_db.Open() Then
                lblStatus.Text = "DB接続に失敗しました"
                Return -1
            End If

            Dim ii As Integer
            Dim nSuccess As Integer = 0
            Dim nFailure As Integer = 0
            Dim nChgNothing As Integer = 0
            Dim nBreak As Integer = 0
            For ii = nStart To nEnd
                ' SQL文（パラメータ付き）実行
                Dim nResult As Integer
                Dim strWk As String = strSql.Replace("$n", CStr(ii))

                '' //////////////////
                '' 実行後の影響範囲取得
                ' トランザクション開始
                tcls_db.TransactStart()
                ' SQL文実行
                nResult = tcls_db.DMLExecute(strWk, para.ToArray)
                ' ロールバック
                tcls_db.TransactRollback()
                ' 影響範囲チェック
                If -1 = nResult Then
                    nfailure += 1
                    Continue For
                ElseIf 0 = nResult Then
                    nChgNothing += 1
                    Continue For
                ElseIf 1 < nResult Then
                    ' 影響範囲が2行以上のときはコミットするか確認
                    If MsgBox(nResult & "行に影響が出る可能性があります。よろしいでしょうか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "書込中断確認") = MsgBoxResult.No Then
                        nBreak += 1
                        Continue For
                    End If
                End If

                ' SQL文実行
                nResult = tcls_db.DMLExecute(strWk, para.ToArray)
                ' 影響範囲チェック
                If -1 = nResult Then
                    nfailure += 1
                    Continue For
                ElseIf 0 = nResult Then
                    nChgNothing += 1
                    Continue For
                End If
                nSuccess += nResult
                Continue For
            Next
            lblStatus.Text = "全SQL文実行終了！（成功:" & nSuccess & "行,失敗：" & nfailure & "行,変更なし：" & nChgNothing & "行,中断：" & nBreak & "回）"
            If nfailure > 0 Or nChgNothing > 0 Or nBreak > 0 Then Return -1
            Return 0
        Catch ex As Exception
            lblStatus.Text = ex.Message
        Finally
            tcls_db.Close()
            tcls_db.Dispose()
        End Try
    End Function
End Class
