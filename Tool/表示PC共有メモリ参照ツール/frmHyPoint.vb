Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

Public Class frmHyPoint
	'///////////////////////////////////////////////////////
	'メンバー変数
	'///////////////////////////////////////////////////////
	Private m_typHyPoint As TBL_HY_POINT

	Private WithEvents m_timer1 As New Timer	' 表示自動更新用タイマー
	Private WithEvents m_timer2 As New Timer	' 共有メモリアクセス可・不可確認タイマー

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmHyPoint_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' 表示自動更新タイマー周期設定
		m_timer1.Interval = 100			' 100msec

		' 共有メモリにアクセス可能かチェックタイマー
		m_timer2.Interval = 100			' 100msec
		m_timer2.Start()

		'レコードコンボボックス初期化
		cmbRecord.Items.Clear()
		For ii As Integer = 0 To TBL_HY_REC_MAX - 1
			cmbRecord.Items.Add(CStr(ii + 1))
		Next
		cmbRecord.SelectedIndex = 0

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
        str = String.Format("TBL_HY_POINT Size = {0}", Marshal.SizeOf(GetType(TBL_HY_POINT))) & vbCrLf
        'str &= String.Format("POINT_INF_NOW Size = {0}", Marshal.SizeOf(GetType(POINT_INF_NOW))) & vbCrLf
        str &= String.Format("POINT_TBL SIZE Size = {0}", Marshal.SizeOf(GetType(POINT_TBL))) & vbCrLf

		txtInfo.Text = str

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
		Dim tmp As TBL_HY_POINT = CType(ksMemRead(TBL_HY_POINT_NAME, 0, GetType(TBL_HY_POINT), nRetc), TBL_HY_POINT)
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
	''' レコード コンボボックス変更時
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub cmbRecord_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbRecord.SelectedIndexChanged
		' 読み込み処理
		btnRead.PerformClick()
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
		If ReadFromShmemAll() Then
			' dgvからデータを読み込む dgv->m_typDefectInfHeader dgv->m_typDefectInfPnt
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
	''' <remarks>共有メモリ⇒m_typHyPoint</remarks>
	Private Function ReadFromShmem() As Boolean
		Dim bRet As Boolean = False
		Dim nRetc As ValueType = 0

		Try
			' 共有メモリ全体の読み込み
			m_typHyPoint = CType(ksMemRead(TBL_HY_POINT_NAME, 0, GetType(TBL_HY_POINT), nRetc), TBL_HY_POINT)

			If Not CBool(nRetc) Then
				' 共有メモリの読み込みに失敗
				bRet = False
				DspShmemStatus(False)
			Else
				bRet = True
				DspShmemStatus(True)
			End If
		Catch ex As Exception
			' 共有メモリ読み込み失敗
			DspShmemStatus(False)
			Dim str As String = txtInfo.Text
			str &= String.Format("ReadFromShmemAll Error [{0}]", ex.Message)
			txtInfo.Text = str
		End Try

		Return bRet

	End Function

	''' <summary>
	''' 共有メモリ全体を読み込む
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function ReadFromShmemAll() As Boolean
		Dim bRet As Boolean = False
		Dim nRetc As ValueType = 0

		Try
			' 共有メモリ全体の読み込み
			m_typHyPoint = CType(ksMemRead(TBL_HY_POINT_NAME, 0, GetType(TBL_HY_POINT), nRetc), TBL_HY_POINT)

			If Not CBool(nRetc) Then
				' 共有メモリの読み込みに失敗
				bRet = False
				DspShmemStatus(False)
			Else
				bRet = True
				DspShmemStatus(True)
			End If
		Catch ex As Exception
			' 共有メモリ読み込み失敗
			DspShmemStatus(False)
			Dim str As String = txtInfo.Text
			str &= String.Format("ReadFromShmemAll Error [{0}]", ex.Message)
			txtInfo.Text = str
		End Try

		Return bRet
	End Function

	''' <summary>
	''' 共有メモリに書き込み
	''' </summary>
	''' <returns>true:書き込み成功 false:書き込み失敗</returns>
	''' <remarks>m_typHyCoilInf⇒共有メモリ</remarks>
	Private Function WriteToShmem() As Boolean
		Dim bRet As Boolean = False

		Try
			If Not ksMemWrite(TBL_HY_POINT_NAME, m_typHyPoint) Then
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


	''' <summary>
	''' 表示を初期化する
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispInit()
		Try
			
            txtRefresh.Text = ""

            txtRecWriteBase.Text = ""
			txtRecWrite0.Text = ""
            txtRecWrite1.Text = ""
            txtRecDispBase.Text = ""
            '検査台位置の現在表示レコード
            txtRecDisp0.Text = ""
            txtRecDisp1.Text = ""
            txtRecDisp2.Text = ""
            txtRecDisp3.Text = ""
            'レコード情報
            txtKizukenNo.Text = ""
			txtDispStatus.Text = ""
            '通板距離
            txtLenDisp00.Text = ""
            txtLenDisp01.Text = ""
            txtLenDisp02.Text = ""
            txtLenDisp03.Text = ""
            '検査位置の現在表示行
            txtRowDisp000.Text = ""
            txtRowDisp001.Text = ""
            txtRowDisp002.Text = ""
            txtRowDisp010.Text = ""
            txtRowDisp011.Text = ""
            txtRowDisp012.Text = ""
            txtRowDisp020.Text = ""
            txtRowDisp021.Text = ""
            txtRowDisp022.Text = ""
            txtRowDisp030.Text = ""
            txtRowDisp031.Text = ""
            txtRowDisp032.Text = ""
            '現在書き込み行
            txtRowWrite000.Text = ""
            txtRowWrite001.Text = ""
            txtRowWrite002.Text = ""
            txtRowWrite010.Text = ""
            txtRowWrite011.Text = ""
            txtRowWrite012.Text = ""
            '検査完了行
            txtRowLast00.Text = ""
            txtRowLast01.Text = ""
            txtRowLast02.Text = ""
        Catch ex As Exception

        End Try
    End Sub

    ''' <summary>
    ''' dgvからデータを読み込む
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadFromDgv()
        Dim rec As Integer = cmbRecord.SelectedIndex

        Try
            '共通情報
            With m_typHyPoint
                .nRefresh = CInt(txtRefresh.Text)
                .nRecWriteBase = CInt(txtRecWriteBase.Text)
                .nRecWrite(0) = CInt(txtRecWrite0.Text)
                .nRecWrite(1) = CInt(txtRecWrite1.Text)
                .nRecDispBase = CInt(txtRecDispBase.Text)
                '通板ポイントの現在レコード
                .nRecDisp(0) = CInt(txtRecDisp0.Text)
                .nRecDisp(1) = CInt(txtRecDisp1.Text)
                .nRecDisp(2) = CInt(txtRecDisp2.Text)
                .nRecDisp(3) = CInt(txtRecDisp3.Text)
            End With

            'レコード情報
            With m_typHyPoint.pnt(rec)
                tMod_Common.StringToByte(.cKizukenNo, txtKizukenNo.Text, .cKizukenNo.Length)
                .emDispStatus = CType(txtDispStatus.Text, EM_DIV_DSP_STATUS)

                'SPM圧下無
                With .typ
                    '通板距離
                    .nLenDisp(0).nLen = CInt(txtLenDisp00.Text)
                    .nLenDisp(1).nLen = CInt(txtLenDisp01.Text)
                    .nLenDisp(2).nLen = CInt(txtLenDisp02.Text)
                    .nLenDisp(3).nLen = CInt(txtLenDisp03.Text)

                    '検査位置の現在表示行
                    .nRowDisp(0).nStage(0) = CInt(txtRowDisp000.Text)
                    .nRowDisp(0).nStage(1) = CInt(txtRowDisp001.Text)
                    .nRowDisp(0).nStage(2) = CInt(txtRowDisp002.Text)
                    .nRowDisp(1).nStage(0) = CInt(txtRowDisp010.Text)
                    .nRowDisp(1).nStage(1) = CInt(txtRowDisp011.Text)
                    .nRowDisp(1).nStage(2) = CInt(txtRowDisp012.Text)
                    .nRowDisp(2).nStage(0) = CInt(txtRowDisp020.Text)
                    .nRowDisp(2).nStage(1) = CInt(txtRowDisp021.Text)
                    .nRowDisp(2).nStage(2) = CInt(txtRowDisp022.Text)
                    .nRowDisp(3).nStage(0) = CInt(txtRowDisp030.Text)
                    .nRowDisp(3).nStage(1) = CInt(txtRowDisp031.Text)
                    .nRowDisp(3).nStage(2) = CInt(txtRowDisp032.Text)
                    '現在書き込み行
                    .nRowWrite(0).nStage(0) = CInt(txtRowWrite000.Text)
                    .nRowWrite(0).nStage(1) = CInt(txtRowWrite001.Text)
                    .nRowWrite(0).nStage(2) = CInt(txtRowWrite002.Text)
                    .nRowWrite(1).nStage(0) = CInt(txtRowWrite010.Text)
                    .nRowWrite(1).nStage(1) = CInt(txtRowWrite011.Text)
                    .nRowWrite(1).nStage(2) = CInt(txtRowWrite012.Text)
                    '検査完了行
                    .nRowLast(0) = CInt(txtRowLast00.Text)
                    .nRowLast(1) = CInt(txtRowLast01.Text)
                    .nRowLast(2) = CInt(txtRowLast02.Text)
                End With

            End With
        Catch ex As Exception

        End Try

    End Sub

    ''' <summary>
    ''' 共有メモリの内容を表示する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispShmem()
        Dim rec As Integer = cmbRecord.SelectedIndex

        Try
            '共通情報
            With m_typHyPoint
                txtRefresh.Text = CStr(.nRefresh)
                txtRecWriteBase.Text = CStr(.nRecWriteBase)
                txtRecWrite0.Text = CStr(.nRecWrite(0))
                txtRecWrite1.Text = CStr(.nRecWrite(1))
                txtRecDispBase.Text = CStr(.nRecDispBase)
                '検査台位置の現在表示レコード
                txtRecDisp0.Text = CStr(.nRecDisp(0))
                txtRecDisp1.Text = CStr(.nRecDisp(1))
                txtRecDisp2.Text = CStr(.nRecDisp(2))
                txtRecDisp3.Text = CStr(.nRecDisp(3))
            End With

            'レコード情報
            With m_typHyPoint.pnt(cmbRecord.SelectedIndex)
                txtKizukenNo.Text = tMod_Common.ByteToString(.cKizukenNo)
                txtDispStatus.Text = CStr(.emDispStatus)

                'SPM圧下無
                With .typ
                    '通板距離
                    txtLenDisp00.Text = CStr(.nLenDisp(0).nLen)
                    txtLenDisp01.Text = CStr(.nLenDisp(1).nLen)
                    txtLenDisp02.Text = CStr(.nLenDisp(2).nLen)
                    txtLenDisp03.Text = CStr(.nLenDisp(3).nLen)
                    '検査位置の現在表示行
                    txtRowDisp000.Text = CStr(.nRowDisp(0).nStage(0))
                    txtRowDisp001.Text = CStr(.nRowDisp(0).nStage(1))
                    txtRowDisp002.Text = CStr(.nRowDisp(0).nStage(2))
                    txtRowDisp010.Text = CStr(.nRowDisp(1).nStage(0))
                    txtRowDisp011.Text = CStr(.nRowDisp(1).nStage(1))
                    txtRowDisp012.Text = CStr(.nRowDisp(1).nStage(2))
                    txtRowDisp020.Text = CStr(.nRowDisp(2).nStage(0))
                    txtRowDisp021.Text = CStr(.nRowDisp(2).nStage(1))
                    txtRowDisp022.Text = CStr(.nRowDisp(2).nStage(2))
                    txtRowDisp030.Text = CStr(.nRowDisp(3).nStage(0))
                    txtRowDisp031.Text = CStr(.nRowDisp(3).nStage(1))
                    txtRowDisp032.Text = CStr(.nRowDisp(3).nStage(2))
                    '現在書き込み行
                    txtRowWrite000.Text = CStr(.nRowWrite(0).nStage(0))
                    txtRowWrite001.Text = CStr(.nRowWrite(0).nStage(1))
                    txtRowWrite002.Text = CStr(.nRowWrite(0).nStage(2))
                    txtRowWrite010.Text = CStr(.nRowWrite(1).nStage(0))
                    txtRowWrite011.Text = CStr(.nRowWrite(1).nStage(1))
                    txtRowWrite012.Text = CStr(.nRowWrite(1).nStage(2))
                    '検査完了行
                    txtRowLast00.Text = CStr(.nRowLast(0))
                    txtRowLast01.Text = CStr(.nRowLast(1))
                    txtRowLast02.Text = CStr(.nRowLast(2))
                End With

            End With
        Catch ex As Exception

		End Try
	
	End Sub

    Private Sub frmHyPoint_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        m_timer1.Stop()
        m_timer1.Dispose()
    End Sub
End Class