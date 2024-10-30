Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Imports System.BitConverter
Imports System.Text

''' <summary>
''' シーケンサ情報テーブル表示
''' </summary>
''' <remarks></remarks>
Public Class frmToSeqmemTbl
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typToSeqmemTbl As TO_SEQMEM_TBL
	Private WithEvents m_timer1 As New Timer    ' 表示自動更新用タイマー
    Private WithEvents m_timer2 As New Timer    ' 共有メモリアクセス可・不可確認タイマー

    Private Enum EM_IN_START_NO
        ST_END = 83                            	' dgvの最後行No
    End Enum

    Private Enum EM_OUT_START_NO
        ST_END = 91	                            ' dgvの最後行No
    End Enum

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmToSeqmemTbl_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' 表示自動更新タイマー周期設定
		m_timer1.Interval = 100			' 100msec

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

        ' dgvの初期化
        InitDgv(dgvSeqRecv, EM_IN_START_NO.ST_END)
        InitDgv(dgvSeqSend, EM_OUT_START_NO.ST_END)


        ' 表示内容クリア
        DispInit()

		' 共有メモリの全体を読み込む
		If ReadFromShmem() Then
			' 共有メモリの内容を表示
			DispShmem()
		End If

		' 自動更新を有効にする・・・必要ない場合はコメントにする
		chkUpdate.Checked = True

        ' 共有メモリサイズ情報表示
        Dim str As String = ""
        str &= String.Format("TO_SEQMEM_TBL Size = {0}", Marshal.SizeOf(GetType(TO_SEQMEM_TBL))) & vbCrLf
        txtInfo.Text = str

        txtTitle.Select()
    End Sub

	''' <summary>
	''' 自動更新処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_timer1_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer1.Tick
		' 共有メモリの全体を読み込む
		If ReadFromShmem() Then
			' 共有メモリの内容を表示
			DispShmem()
		End If
	End Sub

	''' <summary>
	''' 共有メモリアクセス可能かチェック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_timer2_tick(ByVal sender As Object, ByVal e As EventArgs) Handles m_timer2.Tick
		' 対象の共有メモリを定期的に読み込みしてみて、アクセスできるかどうか確認する。
		' あまり大きな共有メモリを読み込むと負荷になるので注意
		Dim nRetc As ValueType = 0
		Dim tmp As TO_SEQMEM_TBL = CType(ksMemRead(TBL_TO_SEQMEM_NAME, 0, GetType(TO_SEQMEM_TBL), nRetc), TO_SEQMEM_TBL)
		If CBool(nRetc) Then
			' 共有メモリにアクセス可能
			DspShmemStatus(True)
			' 自動更新がOFFになっている場合は 読み込み・書き込みボタンを有効にする
			If Not chkUpdate.Checked Then
				btnRead.Enabled = True
				btnWrite.Enabled = True
			End If
		Else
			' 共有メモリにアクセスできない
			DspShmemStatus(False)
			' 読み込み・書き込みボタンを無効にする
			btnRead.Enabled = False
			btnWrite.Enabled = False
		End If
	End Sub

	''' <summary>
	''' 共有メモリが読み込める・読み込めないをタイトルの背景カラーで表す
	''' </summary>
	''' <param name="flg">true:読込可 False:読込不可</param>
	''' <remarks>読込可:LimeGreen 読込不可:Red</remarks>
	Private Sub DspShmemStatus(ByVal flg As Boolean)
		If flg Then
			' 読み込み可能
			txtTitle.BackColor = Color.LimeGreen
		Else
			txtTitle.BackColor = Color.Red
		End If
	End Sub

	''' <summary>
	''' 自動更新のチェックボックスが変化した時の処理
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub chkUpdate_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkUpdate.CheckedChanged
		Dim chk As CheckBox = CType(sender, CheckBox)

		If chk.Checked Then
			' チェックされた時
			chk.BackColor = Color.Cyan
			m_timer1.Start()

			' 読み込み・書き込みボタンを無効にする
			btnRead.Enabled = False
			btnWrite.Enabled = False

		Else
			' チェックが解除された時
			chk.BackColor = SystemColors.Control
			m_timer1.Stop()

			If txtTitle.BackColor <> Color.Red Then
				' 共有メモリアクセス可能な場合
				btnRead.Enabled = True
				btnWrite.Enabled = True
			End If
		End If
	End Sub

	''' <summary>
	''' 読み込みボタン
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
		' 共有メモリの全体を読み込む
		If ReadFromShmem() Then
			' 共有メモリの内容を表示
			DispShmem()
		End If

	End Sub

	''' <summary>
	''' 書き込みボタン
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click

		' 共有メモリの全体を読み込む
		If ReadFromShmem() Then
			' dgvからデータを読み込む
			ReadFromDgv()

			' 共有メモリに書き込む
			WriteToShmem()

			' 共有メモリの全体を読み込む
			If ReadFromShmem() Then
				' 共有メモリの内容を表示
				DispShmem()
			Else
				' 初期状態を表示
				' DispInit()
			End If

		End If

	End Sub

	''' <summary>
	''' 共有メモリから読み込み
	''' </summary>
	''' <returns>true:読み込み成功 False:読み込み失敗</returns>
	''' <remarks>共有メモリ⇒m_typToCoilTbl</remarks>
	Private Function ReadFromShmem() As Boolean
		Dim bRet As Boolean = False
		Dim nRetc As ValueType = 0

		Try
			m_typToSeqmemTbl = CType(ksMemRead(TBL_TO_SEQMEM_NAME, 0, GetType(TO_SEQMEM_TBL), nRetc), TO_SEQMEM_TBL)
			If CBool(nRetc) Then
				' 共有メモリの読み込みに失敗している場合は rec が nothing になる
				bRet = True
				DspShmemStatus(True)
			Else
				bRet = False
				DspShmemStatus(False)
			End If
		Catch ex As Exception
			' 共有メモリ読み込み失敗
			DspShmemStatus(False)
			Dim str As String = txtInfo.Text
			str &= String.Format("ReadFromShmem Error [{0}]", ex.Message)
			txtInfo.Text = str
		End Try

		Return bRet

	End Function


	''' <summary>
	''' 共有メモリに書き込み
	''' </summary>
	''' <returns>true:書き込み成功 false:書き込み失敗</returns>
	''' <remarks>m_typToCoilTbl⇒共有メモリ</remarks>
	Private Function WriteToShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			If Not ksMemWrite(TBL_TO_SEQMEM_NAME, m_typToSeqmemTbl) Then
				bRet = False
				DspShmemStatus(False)
			Else
				bRet = True
				DspShmemStatus(True)
			End If
		Catch ex As Exception
			bRet = False
			DspShmemStatus(False)
			Dim str As String = txtInfo.Text
			str &= String.Format("WriteToShmem Error [{0}]", ex.Message)
			txtInfo.Text = str
		End Try

		Return bRet

	End Function

	Private Enum em_DataColumn
		iNo = 0		' No
		iDevice		' デバイス
		iPos		' 相対位置
		iName		' 名称
		iType		' データ型
		iSize		' バイト数
		iValue		' 値

		iEnd		' 終了
	End Enum


    ''' <summary>
    ''' DataGridView初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="RowCount"></param>
    ''' <remarks></remarks>
    Private Sub InitDgv(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal RowCount As Integer)
        '基本設定
        dgv.dgvInitialize(dgv)

        With dgv
            'スクロールバーの表示
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '行設定
            .RowCount = RowCount
            .RowTemplate.Height = 20

            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue         ' 選択
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え

            '列設定
            .ColumnCount = em_DataColumn.iEnd

            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For ii As Integer = 0 To .ColumnCount - 1
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).HeaderText = ""
                Select Case ii
                    Case em_DataColumn.iNo
                        With .Columns(ii)
                            .Width = 30
                            .ReadOnly = True
                            .Frozen = True
                            .Visible = True
                            .HeaderText = "No"
                        End With
                    Case em_DataColumn.iDevice
                        With .Columns(ii)
                            .HeaderText = "Device"
                            .Width = 45
                            .ReadOnly = True
                            .Frozen = True
                            .Visible = True
                        End With
                    Case em_DataColumn.iPos
                        With .Columns(ii)
                            .HeaderText = "位置"
                            .Width = 35
                            .ReadOnly = True
                            .Frozen = True
                            .Visible = True
                        End With
                    Case em_DataColumn.iName
                        With .Columns(ii)
                            .HeaderText = "名称"
                            .Width = 250
                            .ReadOnly = False
                            .Frozen = True
                            .Visible = True

                            .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

                        End With
                    Case em_DataColumn.iType
                        With .Columns(ii)
                            .HeaderText = "型"
                            .Width = 40
                            .ReadOnly = True
                            .Frozen = True
                            .Visible = True
                        End With
                    Case em_DataColumn.iSize
                        With .Columns(ii)
                            .HeaderText = "byte"
                            .Width = 40
                            .ReadOnly = True
                            .Frozen = True
                            .Visible = True
                        End With
                    Case em_DataColumn.iValue
                        With .Columns(ii)
                            .HeaderText = "値"
                            .Width = 85
                            .ReadOnly = False
                            .Frozen = True
                            .Visible = True
                        End With
                End Select
            Next ii

        End With

    End Sub

    ''' <summary>
    ''' シーケンサ受信メモリ
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="init"></param>
    ''' <param name="flg"></param>
    ''' <remarks></remarks>
    Private Sub RecvMemory(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvSeqRecv
        Dim nPos As Integer = 0              ' 相対位置
        Dim bHanten As Boolean = False       ' バイト反転の有無

        For ii As Integer = 0 To dgv.RowCount - 1

            If init Then
                '' 初期化
                dgv(em_DataColumn.iSize, ii).Value = 0
                ' データメモリ上のデータ型及びバイト数を判別する
                ' 伝文バッファ以外の部分(※バッファ部分については、別の個所で設定)
                Select Case ii
                    Case 16 To 21, 26 To 41
                        dgv(em_DataColumn.iType, ii).Value = "DWORD"
                    Case Else
                        dgv(em_DataColumn.iType, ii).Value = "WORD"
                End Select

                dgv(em_DataColumn.iNo, ii).Value = ii.ToString
                'dgv(em_DataColumn.iDevice, ii).Value = (&HD10000 + ii * 2).ToString("X5")
                'dgv(em_DataColumn.iDevice, ii).Value = (&HD10000 + CInt(nPos / 2)).ToString("X5")
                dgv(em_DataColumn.iDevice, ii).Value = (100000 + CInt(nPos / 2)).ToString
                dgv(em_DataColumn.iPos, ii).Value = nPos.ToString


                Select Case ii
                    Case 0
                        dgv(em_DataColumn.iName, ii).Value = "制御シーケンサ ライブカウンタ"
                    Case 1
                        dgv(em_DataColumn.iName, ii).Value = "制御シーケンサ状態"
                    Case 2
                        dgv(em_DataColumn.iName, ii).Value = "制御シーケンサ稼動状態"
                    Case 8
                        dgv(em_DataColumn.iName, ii).Value = "検査PLG パルスレート"
                    Case 9
                        dgv(em_DataColumn.iName, ii).Value = "検査比較PLG パルスレート"
                    Case 10
                        dgv(em_DataColumn.iName, ii).Value = "表示PLG パルスレート"
                    Case 16
                        dgv(em_DataColumn.iName, ii).Value = "検査用PLG 測長距離"
                    Case 17
                        dgv(em_DataColumn.iName, ii).Value = "検査比較用PLG 測長距離"
                    Case 18
                        dgv(em_DataColumn.iName, ii).Value = "表示用PLG 測長距離"
                    Case 19
                        dgv(em_DataColumn.iName, ii).Value = "WPD信号ON時の検査用PLG 測長距離"
                    Case 20
                        dgv(em_DataColumn.iName, ii).Value = "WPD信号ON時の検査比較用PLG 測長距離"
                    Case 21
                        dgv(em_DataColumn.iName, ii).Value = "WPD信号ON時の表示用PLG 測長距離"
                    Case 26
                        dgv(em_DataColumn.iName, ii).Value = "検査用 フレームNo"
                    Case 27
                        dgv(em_DataColumn.iName, ii).Value = "検査用 PLG 差分"
                    Case 28
                        dgv(em_DataColumn.iName, ii).Value = "検査用 時間 差分"
                    Case 29
                        dgv(em_DataColumn.iName, ii).Value = "検査用 ExSync 積算値"
                    Case 30
                        dgv(em_DataColumn.iName, ii).Value = "検査用 PLG 積算値"
                    Case 31
                        dgv(em_DataColumn.iName, ii).Value = "検査用 時間 積算値"
                    Case 32
                        dgv(em_DataColumn.iName, ii).Value = "検査用 WPD信号ON時のフレームNo"
                    Case 33
                        dgv(em_DataColumn.iName, ii).Value = "検査用 WPD信号ON時のExSync 積算値"
                    Case 34
                        dgv(em_DataColumn.iName, ii).Value = "検査用 WPD信号ON時のフレーム内ExSync位置"
                    Case 38
                        dgv(em_DataColumn.iName, ii).Value = "表示用 PLG積算値"
                    Case 39
                        dgv(em_DataColumn.iName, ii).Value = "表示用 WPD信号ON時のPLG積算値"
                    Case 42
                        dgv(em_DataColumn.iName, ii).Value = "#4 WPDゲート信号(検査基点)"
                    Case 43
                        dgv(em_DataColumn.iName, ii).Value = "#6 WPDゲート信号(表示基点)"
                    Case 44
                        dgv(em_DataColumn.iName, ii).Value = "#4 WPD信号(検査基点)"
                    Case 45
                        dgv(em_DataColumn.iName, ii).Value = "#5 WPD信号"
                    Case 46
                        dgv(em_DataColumn.iName, ii).Value = "#6 WPD信号(表示基点)"
                    Case 47
                        dgv(em_DataColumn.iName, ii).Value = "出側ファーストカット信号"
                    Case 48
                        dgv(em_DataColumn.iName, ii).Value = "出側ラストカット信号"
                    Case 49
                        dgv(em_DataColumn.iName, ii).Value = "SPM 開放 信号"
                    Case 58
                        dgv(em_DataColumn.iName, ii).Value = "表面 チラー異常信号"
                    Case 59
                        dgv(em_DataColumn.iName, ii).Value = "裏面 チラー異常信号"
                    Case 60
                        dgv(em_DataColumn.iName, ii).Value = "BK チラー異常信号"
                    Case 61
                        dgv(em_DataColumn.iName, ii).Value = "表面 チラー運転信号"
                    Case 62
                        dgv(em_DataColumn.iName, ii).Value = "裏面 チラー運転信号"
                    Case 63
                        dgv(em_DataColumn.iName, ii).Value = "BK チラー運転信号"
                    Case 64
                        dgv(em_DataColumn.iName, ii).Value = "表面 φω台車 オンライン"
                    Case 65
                        dgv(em_DataColumn.iName, ii).Value = "表面 φω台車 オフライン"
                    Case 66
                        dgv(em_DataColumn.iName, ii).Value = "裏面 φω台車 オンライン"
                    Case 67
                        dgv(em_DataColumn.iName, ii).Value = "裏面 φω台車 オフライン"
                    Case 68
                        dgv(em_DataColumn.iName, ii).Value = "φω電気室 扉 開閉状態"
                    Case 69
                        dgv(em_DataColumn.iName, ii).Value = "φω電気室 鍵 開閉状態"
                    Case 70
                        dgv(em_DataColumn.iName, ii).Value = "パトライトリセットPB"
                    Case 71
                        dgv(em_DataColumn.iName, ii).Value = "エアドライヤー 運転信号"
                    Case 72
                        dgv(em_DataColumn.iName, ii).Value = "エアドライヤー 異常停止信号"
                    Case 82
                        dgv(em_DataColumn.iName, ii).Value = "塗油スプレー装置 吹付状態"
                    Case Else
                        dgv(em_DataColumn.iName, ii).Value = "予備"
                End Select

                ' サイズがまだ決まっていないならば、型に応じてサイズを決める
                If CInt(dgv(em_DataColumn.iSize, ii).Value) = 0 Then
                    Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                        Case "WORD", "Short"
                            dgv(em_DataColumn.iSize, ii).Value = 2
                        Case "DWORD", "Int"
                            dgv(em_DataColumn.iSize, ii).Value = 4
                    End Select
                End If
            End If

            If IsNothing(m_typToSeqmemTbl.fb_dm) Then   ' 共有メモリを読み込めていないならば相対位置だけ進めて次のループへ。
                nPos += CInt(dgv(em_DataColumn.iSize, ii).Value)   ' 相対位置を進める
                Continue For
            End If

            ' バイト反転の有無を番号毎に判別する
            bHanten = False


            If flg Then
                ' 共有メモリからdgvに読み込み
                Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                    Case "Short"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToInt16(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXInt16(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        End If
                    Case "WORD"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToUInt16(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXUInt16(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        End If
                    Case "DWORD"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToUInt32(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXUInt32(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        End If
                    Case "Char"
                        dgv(em_DataColumn.iValue, ii).Value = Encoding.GetEncoding("shift_jis").GetString(
                            m_typToSeqmemTbl.fb_dm(0).Data, nPos, CInt(dgv(em_DataColumn.iSize, ii).Value))
                    Case "Int"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToInt32(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXInt32(m_typToSeqmemTbl.fb_dm(0).Data, nPos).ToString
                        End If
                End Select
            Else
                ' dgvから共有メモリへ書込み
                Dim tmpByte As Byte()

                Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                    Case "Short"
                        tmpByte = GetBytes(CShort(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(2 - 1 - jj)
                            Next
                        End If
                    Case "WORD"
                        tmpByte = GetBytes(CUShort(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(2 - 1 - jj)
                            Next
                        End If
                    Case "DWORD"
                        tmpByte = GetBytes(CUInt(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(4 - 1 - jj)
                            Next
                        End If
                    Case "Char"
                        ReDim tmpByte(CInt(dgv(em_DataColumn.iSize, ii).Value) - 1)

                        tMod_Common.StringToByte(tmpByte, CStr(dgv(em_DataColumn.iValue, ii).Value))
                        For jj As Integer = 0 To tmpByte.Length - 1
                            m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(jj)
                        Next
                    Case "Int"
                        tmpByte = GetBytes(CInt(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(0).Data(nPos + jj) = tmpByte(4 - 1 - jj)
                            Next
                        End If
                End Select
            End If

            nPos += CInt(dgv(em_DataColumn.iSize, ii).Value)   ' 相対位置を進める
        Next

    End Sub


    ''' <summary>
    ''' シーケンサ送信メモリ
    ''' </summary>
    ''' <param name="rec"></param>
    ''' <param name="init"></param>
    ''' <param name="flg"></param>
    ''' <remarks></remarks>
    Private Sub SendMemory(ByVal rec As Integer, ByVal init As Boolean, ByVal flg As Boolean)

        ' 対象のdgvをセット
        Dim dgv As tCnt_DataGridView = dgvSeqSend
        Dim nPos As Integer = 0              ' 相対位置
        Dim bHanten As Boolean = False       ' バイト反転の有無

        For ii As Integer = 0 To dgv.RowCount - 1
            If init Then
                ' 初期化
                dgv(em_DataColumn.iSize, ii).Value = 0

                ' データメモリ上のデータ型及びバイト数を判別する
                ' 伝文バッファ以外の部分(※バッファ部分については、別の個所で設定)
                Select Case ii
                    Case Else
                        dgv(em_DataColumn.iType, ii).Value = "WORD"
                End Select

                dgv(em_DataColumn.iNo, ii).Value = ii.ToString
                'dgv(em_DataColumn.iDevice, ii).Value = (&HD11000 + ii * 2).ToString("X5")
                'dgv(em_DataColumn.iDevice, ii).Value = (&HD11000 + CInt(nPos / 2)).ToString("X5")
                dgv(em_DataColumn.iDevice, ii).Value = (100000 + CInt(nPos / 2)).ToString
                dgv(em_DataColumn.iPos, ii).Value = nPos.ToString

                Select Case ii
                    Case 0
                        dgv(em_DataColumn.iName, ii).Value = "統括兼蓄積PC ライブカウンタ"
                    Case 1
                        dgv(em_DataColumn.iName, ii).Value = "φω 稼働状態"
                    Case 2
                        dgv(em_DataColumn.iName, ii).Value = "φω 機器状態_表"
                    Case 3
                        dgv(em_DataColumn.iName, ii).Value = "φω 機器状態_裏"
                    Case 4
                        dgv(em_DataColumn.iName, ii).Value = "φω 検査状態_表"
                    Case 5
                        dgv(em_DataColumn.iName, ii).Value = "φω 検査状態_裏"
                    Case 6
                        dgv(em_DataColumn.iName, ii).Value = "φω PLG選択区分"
                    Case 8
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件1「疵検異常です」"
                    Case 9
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件2「コイル情報が受信できません」"
                    Case 10
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件3「疵検査実績送信異常です」"
                    Case 11
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件4「照明装置異常です」"
                    Case 12
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件5「カメラ異常です」"
                    Case 13
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件6「温度異常です」"
                    Case 14
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件7「表」"
                    Case 15
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件8「裏」"
                    Case 16
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件9「ヘゲ接近」"
                    Case 17
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件10「スリバー接近」"
                    Case 18
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件11「連続疵接近」"
                    Case 19
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件12「重大疵接近」"
                    Case 20
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件13「周期疵注意」"
                    Case 21
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件14「周期疵注意 単発」"
                    Case 22
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件15「疵接近」"
                    Case 23
                        dgv(em_DataColumn.iName, ii).Value = "ボイス出力 条件16「表裏面疵接近」"
                    Case 32
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 0 軽疵  （※ DS側）"
                    Case 33
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 0 重疵"
                    Case 34
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 1 軽疵"
                    Case 35
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 1 重疵"
                    Case 36
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 2 軽疵"
                    Case 37
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 2 重疵"
                    Case 38
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 3 軽疵"
                    Case 39
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 3 重疵"
                    Case 40
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 4 軽疵"
                    Case 41
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 4 重疵"
                    Case 42
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 5 軽疵"
                    Case 43
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 5 重疵"
                    Case 44
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 6 軽疵"
                    Case 45
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 6 重疵"
                    Case 46
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 7 軽疵"
                    Case 47
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 表面 7 重疵  （※ WS側）"
                    Case 48
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 0 軽疵  （※ DS側）"
                    Case 49
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 0 重疵"
                    Case 50
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 1 軽疵"
                    Case 51
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 1 重疵"
                    Case 52
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 2 軽疵"
                    Case 53
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 2 重疵"
                    Case 54
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 3 軽疵"
                    Case 55
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 3 重疵"
                    Case 56
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 4 軽疵"
                    Case 57
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 4 重疵"
                    Case 58
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 5 軽疵"
                    Case 59
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 5 重疵"
                    Case 60
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 6 軽疵"
                    Case 61
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 6 重疵"
                    Case 62
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 7 軽疵"
                    Case 63
                        dgv(em_DataColumn.iName, ii).Value = "疵接近ランプ （表検査台） 裏面 7 重疵  （※ WS側）"
                    Case 64
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 0 軽疵  （※ DS側）"
                    Case 65
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 0 重疵"
                    Case 66
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 1 軽疵"
                    Case 67
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 1 重疵"
                    Case 68
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 2 軽疵"
                    Case 69
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 2 重疵"
                    Case 70
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 3 軽疵"
                    Case 71
                        dgv(em_DataColumn.iName, ii).Value = "疵接近アラーム （裏検査台） 裏面 3 重疵  （※ WS側）"
                    Case 72
                        dgv(em_DataColumn.iName, ii).Value = "パトライト （検査台） ランプ"
                    Case 73
                        dgv(em_DataColumn.iName, ii).Value = "パトライト （検査台） ブザー"
                    Case 74
                        dgv(em_DataColumn.iName, ii).Value = "パトライト （運転室） ランプ"
                    Case 75
                        dgv(em_DataColumn.iName, ii).Value = "パトライト （運転室） ブザー"
                    Case 88
                        dgv(em_DataColumn.iName, ii).Value = "制御シーケンサ 高機能カウンタ リセット要求"
                    Case 89
                        dgv(em_DataColumn.iName, ii).Value = "制御シーケンサ 高機能カウンタ 開始要求"
                    Case 90
                        dgv(em_DataColumn.iName, ii).Value = "テスト用 WPD信号 ON 要求  （※ ゲート信号もON）"
                    Case Else
                        dgv(em_DataColumn.iName, ii).Value = "予備"
                End Select

                ' サイズがまだ決まっていないならば、型に応じてサイズを決める
                If CInt(dgv(em_DataColumn.iSize, ii).Value) = 0 Then
                    Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                        Case "WORD", "Short"
                            dgv(em_DataColumn.iSize, ii).Value = 2
                        Case "DWORD", "Int"
                            dgv(em_DataColumn.iSize, ii).Value = 4
                    End Select
                End If
            End If

            If IsNothing(m_typToSeqmemTbl.fb_dm) Then   ' 共有メモリを読み込めていないならば相対位置だけ進めて次のループへ。
                nPos += CInt(dgv(em_DataColumn.iSize, ii).Value)   ' 相対位置を進める
                Continue For
            End If

            ' バイト反転の有無を番号毎に判別する
            bHanten = False

            If flg Then
                ' 共有メモリからdgvに読み込み
                Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                    Case "Short"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToInt16(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXInt16(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        End If
                    Case "WORD"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToUInt16(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXUInt16(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        End If
                    Case "DWORD"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToUInt32(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXUInt32(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        End If
                    Case "Char"
                        dgv(em_DataColumn.iValue, ii).Value = Encoding.GetEncoding("shift_jis").GetString(
                            m_typToSeqmemTbl.fb_dm(1).Data, nPos, CInt(dgv(em_DataColumn.iSize, ii).Value))
                    Case "Int"
                        If bHanten = False Then
                            dgv(em_DataColumn.iValue, ii).Value = ToInt32(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        Else
                            dgv(em_DataColumn.iValue, ii).Value = ToXInt32(m_typToSeqmemTbl.fb_dm(1).Data, nPos).ToString
                        End If
                End Select
            Else
                ' dgvから共有メモリへ書込み
                Dim tmpByte As Byte()

                Select Case CStr(dgv(em_DataColumn.iType, ii).Value)
                    Case "Short"
                        tmpByte = GetBytes(CShort(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(2 - 1 - jj)
                            Next
                        End If
                    Case "WORD"
                        tmpByte = GetBytes(CUShort(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 2 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(2 - 1 - jj)
                            Next
                        End If
                    Case "DWORD"
                        tmpByte = GetBytes(CUInt(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(4 - 1 - jj)
                            Next
                        End If
                    Case "Char"
                        ReDim tmpByte(CInt(dgv(em_DataColumn.iSize, ii).Value) - 1)

                        tMod_Common.StringToByte(tmpByte, CStr(dgv(em_DataColumn.iValue, ii).Value))
                        For jj As Integer = 0 To tmpByte.Length - 1
                            m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(jj)
                        Next
                    Case "Int"
                        tmpByte = GetBytes(CInt(dgv(em_DataColumn.iValue, ii).Value))
                        If bHanten = False Then
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(jj)
                            Next
                        Else
                            For jj As Integer = 0 To 4 - 1
                                m_typToSeqmemTbl.fb_dm(1).Data(nPos + jj) = tmpByte(4 - 1 - jj)
                            Next
                        End If
                End Select
            End If

            nPos += CInt(dgv(em_DataColumn.iSize, ii).Value)   ' 相対位置を進める
        Next
    End Sub

	''' <summary>
	''' 表示を初期化する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispInit()
		' シーケンサ受信メモリ
		RecvMemory(0, True, True)
        ' シーケンサ送信メモリ
        SendMemory(0, True, True)
    End Sub

	''' <summary>
	''' dgvからデータを読み込む
	''' </summary>
	''' <remarks></remarks>
	Private Sub ReadFromDgv()
		' シーケンサ受信メモリ
		RecvMemory(0, False, False)
        ' シーケンサ送信メモリ
        SendMemory(0, False, False)
    End Sub

    ''' <summary>
    ''' 共有メモリの内容を表示する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        ' シーケンサ受信メモリ
        RecvMemory(0, False, True)
        ' シーケンサ送信メモリ
        SendMemory(0, False, True)
    End Sub

    ''' <summary>
    ''' バイト配列から、バイト反転したShort型の値を取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ToXInt16(ByVal value() As Byte, ByVal startIndex As Integer) As Short
        Dim ary(2 - 1) As Byte

        Buffer.BlockCopy(value, startIndex, ary, 0, 2)
        Array.Reverse(ary)
        Return ToInt16(ary, 0)
    End Function

    ''' <summary>
    ''' バイト配列から、バイト反転したWORD型の値を取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ToXUInt16(ByVal value() As Byte, ByVal startIndex As Integer) As UInteger
        Dim ary(2 - 1) As Byte

        Buffer.BlockCopy(value, startIndex, ary, 0, 2)
        Array.Reverse(ary)
        Return ToUInt16(ary, 0)
    End Function

    ''' <summary>
    ''' バイト配列から、バイト反転したDWORD型の値を取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ToXUInt32(ByVal value() As Byte, ByVal startIndex As Integer) As UInteger
        Dim ary(4 - 1) As Byte

        Buffer.BlockCopy(value, startIndex, ary, 0, 4)
        Array.Reverse(ary)
        Return ToUInt32(ary, 0)
    End Function

    ''' <summary>
    ''' バイト配列から、バイト反転したInteger型の値を取得する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ToXInt32(ByVal value() As Byte, ByVal startIndex As Integer) As Integer
        Dim ary(4 - 1) As Byte

        Buffer.BlockCopy(value, startIndex, ary, 0, 4)
        Array.Reverse(ary)
        Return ToInt32(ary, 0)
    End Function
End Class