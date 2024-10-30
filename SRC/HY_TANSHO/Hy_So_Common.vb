Public Module Hy_So_Common

	'====================================================================
    '	実績画面、疵マップ表示画面 カット位置情報用構造体
	'====================================================================
    Public Structure CUT_INF_DATA
        Dim nKubun As Integer                                       '// 区分
        Dim dPosS As Single                                         '// From [m] (出側物理コイル先端から距離)
		'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>		
		Dim dPosS_MM As Single                                      '// From [mm] (出側物理コイル先端から距離)
		'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
    End Structure

    'マップセル情報
    Public Structure TYP_MAP_LIST_VALUE
        Dim nRec As Integer                                 ' レコードNo(探傷画面のみ)
        Dim nPnt As Integer                                 ' 疵情報テーブルへのポインタ(探傷画面のみ)
        Dim nKizuNo As Integer                              ' 疵No
        Dim nTid As Integer                                 ' 疵種EdasysID
        Dim nTYuu As Integer                                ' 疵種優先度
        Dim nGid As Integer                                 ' グレードEdasysID
        Dim nGYuu As Integer                                ' グレード優先度
        Dim nAria As Integer                                ' 面積 
        Dim nY As Integer                                   ' 長手位置
		'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		Dim nY_Out As Integer                               ' 長手位置(出側)
		'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
        Dim nCycDiv As Integer                              ' 周期疵区分
        Dim strFFType As String                             ' FF疵種(TCM用)
        Dim strFFGrade As String                            ' FFグレード(TCM用)
        Dim strProc As String                               ' 前工程(TCM用)
        ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
        Dim nKouteiID As Integer                            ' 工程種別 
        ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

        Shared Operator =(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Boolean
        	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
            'If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Then
			If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Or a.nY_Out <> b.nY_Out Then
			'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                Return False
            End If
            Return True
        End Operator

        Shared Operator <>(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Boolean
        	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
            'If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Then
            If a.nRec <> b.nRec Or a.nPnt <> b.nPnt Or a.nKizuNo <> b.nKizuNo Or a.nY_Out <> b.nY_Out Then
			'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                Return True
            End If
            Return False
        End Operator

        ''' <summary>
        ''' 文字列表現
        ''' </summary>
        ''' <returns>文字列表現</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return String.Format("KizuNo={0} TypeID={1} GradeID={2}", nKizuNo, nTid, nGid)
        End Function

        ''' <summary>
        ''' イニシャル
        ''' </summary>
        Public Sub Initialize()
            nRec = -1
            nPnt = -1
            nKizuNo = -1
            strFFType = ""
            strFFGrade = ""
            strProc = ""
			'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
			nY_Out = 0
			'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
        End Sub

    End Structure

    '-------------------------------------------------------
    '列情報
    '-------------------------------------------------------
    ''' <summary>
    ''' 列情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_COL
        Dim strKanriNo As String                            '管理No.
        Dim nKizuNo As Integer                              '疵No
        Dim nTid As Integer                                 '疵種EasysID
        Dim nGid As Integer                                 'グレードEdasysID
    End Structure

    ''' <summary>
    ''' 列情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_TCM_COL
        Dim strKanriNo As String                            '管理No.
        Dim nKizuNo As Integer                              '疵No
        Dim strProc As String                               ' 前工程(TCM用)
        Dim strFFType As String                             ' FF疵種(TCM用)
        Dim strFFGrade As String                            ' FFグレード(TCM用)
    End Structure

    '-------------------------------------------------------
    '行情報
    '-------------------------------------------------------
    ''' <summary>
    ''' 行情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_ROW
        Dim nRec As Integer                                 'レコード番号(探傷画面のみ)
        Dim nMemRowPos As Integer                           'メモリ行番号(探傷画面のみ)
        Dim nEnable As Integer                              '情報受信状態(探傷画面のみ)
        Dim emKensa As EM_DIV_KENSA                         '検査状態 (DIV_KENSA_NON･･･)
        Dim emKiki As EM_DIV_KIKI                           '機器状態 (DIV_KIKI_OK･･･)
        Dim nKizuCnt As Integer                             '行単位表示対象疵個数
        Dim nLen_i As Integer                               '入側距離(要は溶接点からの距離) [m]
        Dim nLen_o As Integer                               '出側距離(要はカットからの距離) [m]
        Dim nCut As Integer                                 'シャーカット区分(0:シャーカット無し 1:シャーカットされた)
        Dim bYousetu As Boolean                             '溶接点フラグ
        Dim col() As G_MAP_COL                              '列情報
        Dim tcm() As G_MAP_TCM_COL                          '列情報
        'イニシャライズ
        Public Sub initialize()
            ReDim col(MAP_COL_NUM - 1)
            ReDim tcm(1 - 1)
        End Sub
    End Structure

    '-------------------------------------------------------
    '面情報
    '-------------------------------------------------------
    ''' <summary>
    ''' 面情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_MEN
        Dim row() As G_MAP_ROW
        'イニシャライズ
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            If nRow = 0 Then
                ReDim row(MAP_ROW_NUM - 1)
            Else
                ' 行数指定の初期化が行われた場合の対応
                ReDim row(nRow - 1)
            End If
            For ii As Integer = 0 To row.Length - 1
                row(ii).initialize()
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    'マップ切替
    '-------------------------------------------------------
    ''' <summary>
    ''' マップ切替
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure G_MAP_STAGE
        Dim men() As G_MAP_MEN
        'イニシャライズ
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim men(NUM_MEN - 1)
            For ii As Integer = 0 To men.Length - 1
                men(ii).initialize(nRow)
            Next
        End Sub
    End Structure

    '-------------------------------------------------------
    'レコード
    '-------------------------------------------------------
    Public Structure DSP_G_MAP
        Dim stage() As G_MAP_STAGE
        Public Sub initialize(Optional ByVal nRow As Integer = 0)
            ReDim stage(MAP_STAGE_NUM - 1)
            For ii As Integer = 0 To stage.Length - 1
                stage(ii).initialize(nRow)
            Next
        End Sub
    End Structure



    '-------------------------------------------------------
    'TCM疵情報(実績画面用)
    '-------------------------------------------------------
    Public Structure TYP_MAE_DEFECT_DATA
        Dim ff As DSP_TYP_MAE_FFINF                     ' FF情報
        Dim data As DSP_TYP_MAE_DEFECTINF               ' 前工程疵情報
        Dim strKanriNo As String                        ' 管理No.
        Dim nKizuNo As Integer                          ' 自工程DB 疵No
        Public Sub initialize()
            strKanriNo = ""
            ff.initialize()
        End Sub

    End Structure

    ''' <summary>
    ''' 画面表示コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>表示名称</remarks>
    Public Class cls_CmbMakerItem

        ''' <summary>nMode</summary>
        Public mode As Integer          'nMode
        ''' <summary>表示名</summary>
        Public name As String           '表示名

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nMode">モード</param>
        ''' <param name="strName">表示名</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nMode As Integer, ByVal strName As String)
            mode = nMode
            name = strName
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class

    '疵マーカーラベル
    Public Enum EM_MARKER_MODE
        nMarkerModeNone = 0     '疵なし
        nMarkerModeType         '疵種
        nMarkerModeGrade        'ｸﾞﾚｰﾄﾞ
    End Enum

    '疵マーカー色替え
    Public Enum EM_MARKER_COLOR_MODE
        nMarkerColorModeType = 0    '疵種
        nMarkerColorModeGrade       'ｸﾞﾚｰﾄﾞ
    End Enum

    ''' <summary>
    ''' 調整済フォントサイズの取得(DataGridView)
    ''' </summary>
    ''' <param name="dgv">対象DataGridView</param>
    ''' <param name="strString">表示文字列</param>
    ''' <param name="nDefaultFontSize">既定フォントサイズ</param>
    ''' <param name="nMaxWidth">最大表示幅</param>
    ''' <param name="nMaxHeight">最大表示高さ</param>
    ''' <returns>最大表示幅に適したフォントサイズ(1pt単位)</returns>
    ''' <remarks>
    ''' フォントサイズが8未満の場合は8を返却する。
    ''' また、既定フォントサイズで最大表示幅を超えない場合は既定フォントサイズを返す
    ''' </remarks>
    Public Function GetGridFontScaleSize(ByVal dgv As DataGridView, ByVal strString As String, ByVal nDefaultFontSize As Single, ByVal nMaxWidth As Integer, ByVal nMaxHeight As Integer) As Single
        Dim nFontWidth As Single                    ' 文字表示幅
        Dim nFontHeight As Single                   ' 文字表示高さ
        Dim nScaleFontSize As Single                ' 調整フォントサイズ
        Dim fntScale As System.Drawing.Font         ' 調整フォント
        Dim nMaxDspWidth As Integer                 ' 文字表示最大幅
        Dim g As Graphics = dgv.CreateGraphics()
        Dim fntDefault As Font = New System.Drawing.Font("ＭＳ 明朝", nDefaultFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        Try

            ' 初期化
            nFontWidth = g.MeasureString(strString, fntDefault).Width
            nScaleFontSize = nDefaultFontSize
            fntScale = New System.Drawing.Font("ＭＳ 明朝", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            ' 文字表示最大幅は表示最大幅から既定フォントサイズ半角１文字分余裕を持たせる
            nMaxDspWidth = nMaxWidth - CShort(g.MeasureString("@", fntDefault).Width)
            If nMaxDspWidth < 0 Then
                nMaxDspWidth = nMaxWidth
            End If

            ' 表示範囲内に収まる場合は既定フォントサイズを返す
            If nFontWidth <= nMaxDspWidth Then
                nFontHeight = g.MeasureString(strString, fntScale).Height
                If nFontHeight + 2 < nMaxHeight Then
                    fntScale.Dispose()
                    Return nDefaultFontSize
                End If
            End If

            ' 1ptずつフォントサイズを小さくして、文字表示最大幅を下回るフォントサイズを算出
            While nScaleFontSize > 0
                nScaleFontSize = nScaleFontSize - 1.0F
                If nScaleFontSize < 6 Then
                    Exit While
                End If

                fntScale = New System.Drawing.Font("ＭＳ 明朝", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                nFontWidth = g.MeasureString(strString, fntScale).Width

                If nFontWidth <= nMaxDspWidth Then
                    nFontHeight = g.MeasureString(strString, fntScale).Height
                    If nFontHeight + 2 < nMaxHeight Then
                        Exit While
                    End If
                End If

            End While

            ' 余り小さいと見えないのでフォントサイズが7未満は7にする。
            If nScaleFontSize < 6 Then
                nScaleFontSize = 6
            End If

            fntScale.Dispose()

        Catch ex As Exception

        Finally
            If g IsNot Nothing Then
                g.Dispose()
                g = Nothing
            End If

        End Try

        Return nScaleFontSize
    End Function

End Module
