Public Module modCommon
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画面ID定義
    Public Const GAMEN_ID_EPC_COMMON As String = "EPC_COMMON"                   '固定パラメータ設定
    Public Const GAMEN_ID_EPC_PARA As String = "EPC_PARA"                       'ロジックパララン設定
    Public Const GAMEN_ID_EPC_CAMERA As String = "EPC_CAMERA"                   'カメラ設定
    Public Const GAMEN_ID_EPC_MIXCOLOR As String = "EPC_MIXCOLOR"               '混色分離設定
    Public Const GAMEN_ID_EPC_EDGE As String = "EPC_EDGE"                       'エッジ検出設定
    Public Const GAMEN_ID_EPC_SHADING As String = "EPC_SHADING"                 'シェーディング補正設定
    Public Const GAMEN_ID_EPC_THRESHOLD As String = "EPC_THRESHOLD"             '閾値設定
    Public Const GAMEN_ID_EPC_DEFECT As String = "EPC_DEFECT"                   '疵検出設定
    Public Const GAMEN_ID_EPC_MERGE As String = "EPC_MERGE"                     '疵統合処理設定
    ' <20221117> <小島> <最終工程検査装置高機能化その1> --->>>
    Public Const GAMEN_ID_EPC_TAJUUTHRESHOLD As String = "EPC_TAJUUTHRESHOLD"   '多重分解能閾値設定
    ' <20221117> <小島> <最終工程検査装置高機能化その1> ---<<<

    'Public Const SELECT_KEY_PCOMMON As String = "SELECT [Key],{0} FROM {1} where [Key]={2}"
    'Public Const SELECT_KEY_PCAMSET As String = "SELECT {0} FROM {1} ORDER BY camset ASC"
    'Public Const SELECT_KEY_PCAMSET_ANGLE As String = "SELECT {0} FROM {1} ORDER BY camangle, camset ASC"
    'Public Const SELECT_KEY_PSCODE As String = "SELECT {0} FROM {1} ORDER BY scode ASC"
    'Public Const SELECT_KEY_PCAMSET_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camset ASC"
    'Public Const SELECT_KEY_PCAMANGLE_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camangle ASC"
    'Public Const SELECT_KEY_PCAMSET_ANGLE_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camangle, camset ASC"

    'Public Const UPDATE_KEY_PSCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMSET_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camset = b.camset AND a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMANGLE_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle AND a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMSET_ANGLE_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle And a.camset = b.camset AND a.scode IN({5})"

    'Public Const SELECT_KEY As String = "SELECT [Key],{0} FROM {1}"
    Public Const SELECT_KEY_ORDER As String = "SELECT {0} FROM {1} ORDER BY {2} ASC"

    '---------------------------------------------------------------------------
    '// 自動生成画面ﾃﾞｰﾀｸﾞﾘｯﾄﾞ表示設定用定義
    '---------------------------------------------------------------------------
    '// GRID_FIRST_DISP_GID                 '// 初期表示画面ID
    '// GRID_FIRST_DISP_NAME                '// 初期表示画面ID表示名称
    '---------------------------------------------------------------------------
    '// 画面別定義
    '// XXXXXXXXXX_GRID_GID                 '// 画面IDのDB登録値 
    '//                                         システム上に画面が存在しない場合は値をセットしないと表示対象外となる
    '// XXXXXXXXXX_GRID_NAME                '// 画面表示上の画面ID
    '// XXXXXXXXXX_GRID_CNT                 '// 画面に紐付くグリッド数
    '// XXXXXXXXXX_GRID_ID                  '// グリッドIDのDB登録値(グリッド数だけカンマ区切りで定義)
    '// XXXXXXXXXX_GRID                     '// 画面表示上のグリッドID(グリッド数だけカンマ区切りで定義)
    '---------------------------------------------------------------------------
    Public Const GRID_FIRST_DISP_GID As String = EPC_COMMON_GRID_GID                            '// 初期表示画面ID
    Public Const GRID_FIRST_DISP_NAME As String = EPC_COMMON_GRID_NAME                          '// 初期表示画面ID表示名称
    '固定パラメータ設定画面
    Public Const EPC_COMMON_GRID_GID As String = GAMEN_ID_EPC_COMMON                            '// 画面IDのDB登録値
    Public Const EPC_COMMON_GRID_NAME As String = "固定パラメータ設定"                          '// 画面表示上の画面ID
    Public Const EPC_COMMON_GRID_CNT As Integer = 2                                             '// 画面に紐付くグリッド数
    Public Const EPC_COMMON_GRID_ID As String = "1,2"                                           '// グリッドIDのDB登録値
    Public Const EPC_COMMON_GRID As String = "共通,カメラセット別"                              '// 画面表示上のグリッドID
    'ロジックパララン設定画面
    Public Const EPC_PARA_GRID_GID As String = GAMEN_ID_EPC_PARA                                '// 画面IDのDB登録値
    Public Const EPC_PARA_GRID_NAME As String = "ロジックパララン設定"                          '// 画面表示上の画面ID
    Public Const EPC_PARA_GRID_CNT As Integer = 3                                               '// 画面に紐付くグリッド数
    Public Const EPC_PARA_GRID_ID As String = "1,2,3"                                           '// グリッドIDのDB登録値
    Public Const EPC_PARA_GRID As String = "共通,カメラセット別,検査表面状態別"
    'カメラ設定画面
    Public Const EPC_CAMERA_GRID_GID As String = GAMEN_ID_EPC_CAMERA                            '// 画面IDのDB登録値
    Public Const EPC_CAMERA_GRID_NAME As String = "カメラ設定"                                  '// 画面表示上の画面ID
    Public Const EPC_CAMERA_GRID_CNT As Integer = 3                                             '// 画面に紐付くグリッド数
    Public Const EPC_CAMERA_GRID_ID As String = "1,2,3"                                         '// グリッドIDのDB登録値
    Public Const EPC_CAMERA_GRID As String = "共通,ｶﾒﾗ角度・検査表面状態別,ｶﾒﾗ角度・ｶﾒﾗｾｯﾄ・検査表面状態別" '// 画面表示上のグリッドID
    '混色分離設定画面
    Public Const EPC_MIXCOLOR_GRID_GID As String = GAMEN_ID_EPC_MIXCOLOR                        '// 画面IDのDB登録値
    Public Const EPC_MIXCOLOR_GRID_NAME As String = "混色分離設定"                              '// 画面表示上の画面ID
    Public Const EPC_MIXCOLOR_GRID_CNT As Integer = 1                                           '// 画面に紐付くグリッド数
    Public Const EPC_MIXCOLOR_GRID_ID As String = "1"                                           '// グリッドIDのDB登録値
    Public Const EPC_MIXCOLOR_GRID As String = "カメラセット別"                                 '// 画面表示上のグリッドID
    'エッジ検出設定画面
    Public Const EPC_EDGE_GRID_GID As String = GAMEN_ID_EPC_EDGE                                '// 画面IDのDB登録値
    Public Const EPC_EDGE_GRID_NAME As String = "エッジ検出設定"                                '// 画面表示上の画面ID
    Public Const EPC_EDGE_GRID_CNT As Integer = 4                                               '// 画面に紐付くグリッド数
    Public Const EPC_EDGE_GRID_ID As String = "1,2,3,4"                                         '// グリッドIDのDB登録値
    Public Const EPC_EDGE_GRID As String = "カメラセット別,ｶﾒﾗｾｯﾄ・検査表面状態別,ｶﾒﾗ角度・ｶﾒﾗｾｯﾄ・検査表面状態別,検査表面状態別" '// 画面表示上のグリッドID
    'ｼｪｰﾃﾞｨﾝｸﾞ補正設定画面
    Public Const EPC_SHADING_GRID_GID As String = GAMEN_ID_EPC_SHADING                          '// 画面IDのDB登録値
    Public Const EPC_SHADING_GRID_NAME As String = "ｼｪｰﾃﾞｨﾝｸﾞ補正設定"                          '// 画面表示上の画面ID
    Public Const EPC_SHADING_GRID_CNT As Integer = 2                                            '// 画面に紐付くグリッド数
    Public Const EPC_SHADING_GRID_ID As String = "1,2"                                          '// グリッドIDのDB登録値
    Public Const EPC_SHADING_GRID As String = "ｶﾒﾗ角度・検査表面状態別,検査表面状態別"          '// 画面表示上のグリッドID
    '閾値設定画面
    Public Const EPC_THRESHOLD_GRID_GID As String = GAMEN_ID_EPC_THRESHOLD                      '// 画面IDのDB登録値
    Public Const EPC_THRESHOLD_GRID_NAME As String = "閾値設定"                                 '// 画面表示上の画面ID
    Public Const EPC_THRESHOLD_GRID_CNT As Integer = 1                                          '// 画面に紐付くグリッド数
    Public Const EPC_THRESHOLD_GRID_ID As String = "1"                                          '// グリッドIDのDB登録値
    Public Const EPC_THRESHOLD_GRID As String = "ｶﾒﾗ角度・検査表面状態別"                       '// 画面表示上のグリッドID
    '疵検出設定画面
    Public Const EPC_DEFECT_GRID_GID As String = GAMEN_ID_EPC_DEFECT                            '// 画面IDのDB登録値
    Public Const EPC_DEFECT_GRID_NAME As String = "疵検出設定"                                  '// 画面表示上の画面ID
    Public Const EPC_DEFECT_GRID_CNT As Integer = 2                                             '// 画面に紐付くグリッド数
    Public Const EPC_DEFECT_GRID_ID As String = "1,2"                                           '// グリッドIDのDB登録値
    Public Const EPC_DEFECT_GRID As String = "検査表面状態別,ｶﾒﾗ角度・検査表面状態別"           '// 画面表示上のグリッドID
    '統合処理設定画面
    Public Const EPC_MERGE_GRID_GID As String = GAMEN_ID_EPC_MERGE                              '// 画面IDのDB登録値
    Public Const EPC_MERGE_GRID_NAME As String = "統合処理設定"                                 '// 画面表示上の画面ID
    Public Const EPC_MERGE_GRID_CNT As Integer = 1                                              '// 画面に紐付くグリッド数
    Public Const EPC_MERGE_GRID_ID As String = "1"                                              '// グリッドIDのDB登録値
    Public Const EPC_MERGE_GRID As String = "共通"                                              '// 画面表示上のグリッドID
    ' <20221117> <小島> <最終工程高機能化その1> --->>>
    '閾値設定画面
    Public Const EPC_TAJUUTHRESHOLD_GRID_GID As String = GAMEN_ID_EPC_TAJUUTHRESHOLD             '// 画面IDのDB登録値
    Public Const EPC_TAJUUTHRESHOLD_GRID_NAME As String = "多重分解能閾値設定"                   '// 画面表示上の画面ID
    Public Const EPC_TAJUUTHRESHOLD_GRID_CNT As Integer = 3                                      '// 画面に紐付くグリッド数
    Public Const EPC_TAJUUTHRESHOLD_GRID_ID As String = "1,2,3"                                  '// グリッドIDのDB登録値
    Public Const EPC_TAJUUTHRESHOLD_GRID As String = "検査表面状態別(合成ラベル),検査表面状態別(単純二値化),検査表面状態別(フィルタ二値化)"      '// 画面表示上のグリッドID
    ' <20221117> <小島> <最終工程高機能化その1> ---<<<
    '---------------------------------------------------------------------------

End Module
