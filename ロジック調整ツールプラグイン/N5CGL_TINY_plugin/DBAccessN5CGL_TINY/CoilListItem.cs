using System;
using System.Collections.Generic;
using System.Text;

namespace DBAccessN5CGL_TINY
{
    /// <summary>
    /// 出側コイル
    /// </summary>
    public class CoilListItem
    {
        #region コイル一覧メンバ

        /// <summary>
        /// 登録日付
        /// </summary>
        private DateTime m_datetime;

        /// <summary>
        /// 管理No
        /// </summary>
        private string m_filename;

        /// <summary>
        /// コイルNo
        /// </summary>
        private string m_coilnum;

        /// <summary>
        /// カット番号(0～) (子番)
        /// </summary>
        private int m_cut_no;

        /// <summary>
        /// 入側コイルのカット回数
        /// </summary>
        private int m_ncut;

        /// <summary>
        /// 入側コイルの採寸開始位置[mm]
        /// </summary>
        private int m_pos_from;

        /// <summary>
        /// 入側コイルの採寸終了位置[mm]
        /// </summary>
        private int m_pos_to;

        /// <summary>
        /// 入側コイルの採寸終了位置[mm] (疵を探すときに使う)
        /// </summary>
        private int m_pos_to_defect;

        /// <summary>
        /// 材料板幅[mm]
        /// </summary>
        private int m_width;

        /// <summary>
        /// 注文板厚[mm]
        /// </summary>
        private float m_thickness;

        /// <summary>
        /// 検査実測長[mm]
        /// </summary>
        private int m_length_base;

        /// <summary>
        /// 検査表面状態_表
        /// </summary>
        private string m_surface_up;

        /// <summary>
        /// 検査表面状態_裏
        /// </summary>
        private string m_surface_dn;

        /// <summary>
        /// 検査基準_表
        /// </summary>
        private string m_insp_level_up;

        /// <summary>
        /// 検査基準_裏
        /// </summary>
        private string m_insp_level_dn;

        /// <summary>
        /// 主材質コード
        /// </summary>
        private string m_primary_context;

        /// <summary>
        /// 表面処理1桁目
        /// </summary>
        private string m_surf_proc_dig1;

        /// <summary>
        /// 表面処理2桁目
        /// </summary>
        private string m_surf_proc_dig2;

        /// <summary>
        /// 計画次工程コード
        /// </summary>
        private string m_next_proc_sched;

        /// <summary>
        /// 表保存疵個数
        /// </summary>
        private int m_total_ndefects_up;

        /// <summary>
        /// 裏保存疵個数
        /// </summary>
        private int m_total_ndefects_dn;

        #endregion


        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="datetime">登録日付</param>
        /// <param name="filename">管理No</param>
        /// <param name="coilnum">コイルNo</param>
        /// <param name="cut_no">カット番号</param>
        /// <param name="ncut">入側コイルのカット回数</param>
        /// <param name="pos_from">入側コイルの採寸開始位置</param>
        /// <param name="pos_to">入側コイルの採寸終了位置</param>
        /// <param name="pos_to_defect">入側コイルの採寸終了位置 (疵を探すときに使う)</param>
        /// <param name="width">材料板幅</param>
        /// <param name="thickness">注文板厚</param>
        /// <param name="length_base">検査実測長</param>
        /// <param name="surface_up">検査表面状態_表</param>
        /// <param name="surface_dn">検査表面状態_裏</param>
        /// <param name="insp_level_up">検査基準_表</param>
        /// <param name="insp_level_dn">検査基準_裏</param>
        /// <param name="primary_context">主材質コード</param>
        /// <param name="surf_proc_dig1">表面処理1桁目</param>
        /// <param name="surf_proc_dig2">表面処理2桁目</param>
        /// <param name="next_proc_sched">計画次工程コード</param>
        /// <param name="total_ndefects_up">表保存疵個数</param>
        /// <param name="total_ndefects_dn">裏保存疵個数</param>
        public CoilListItem(
            DateTime datetime,
            string filename,
            string coilnum,
            int cut_no,
            int ncut,
            int pos_from,
            int pos_to,
            int pos_to_defect,
            int width,
            float thickness,
            int length_base,
            string surface_up,
            string surface_dn,
            string insp_level_up,
            string insp_level_dn,
            string primary_context,
            string surf_proc_dig1,
            string surf_proc_dig2,
            string next_proc_sched,
            int total_ndefects_up,
            int total_ndefects_dn)
        {
            m_datetime = datetime;
            m_filename = filename;
            m_coilnum = coilnum + "_" + (cut_no+1) + "_" + (ncut+1);
            m_cut_no = cut_no;
            m_ncut = ncut;
            m_pos_from = pos_from;
            m_pos_to = pos_to;
            m_pos_to_defect = pos_to_defect;
            m_width = width;
            m_thickness = thickness;
            m_length_base = length_base;
            m_surface_up = surface_up;
            m_surface_dn = surface_dn;
            m_insp_level_up = insp_level_up;
            m_insp_level_dn = insp_level_dn;
            m_primary_context = primary_context;
            m_surf_proc_dig1 = surf_proc_dig1;
            m_surf_proc_dig2 = surf_proc_dig2;
            m_next_proc_sched = next_proc_sched;
            m_total_ndefects_up = total_ndefects_up;
            m_total_ndefects_dn = total_ndefects_dn;
        }

        #region コイル一覧メンバ(外部参照)

        /// <summary>
        /// 登録日付
        /// </summary>
        /// <remarks>必須</remarks>
        public DateTime DateTime
        {
            get { return m_datetime; }
        }

        /// <summary>
        /// 管理No
        /// </summary>
        /// <remarks>必須</remarks>
        public string Filename
        {
            get { return m_filename; }
        }

        /// <summary>
        /// コイルNo
        /// </summary>
        public string CoilNum
        {
            get { return m_coilnum; }
        }

        /// <summary>
        /// 出側単位としての長さ[mm]
        /// </summary>
        public int LengthAfterCut
        {
            get { return m_pos_to - m_pos_from; }
        }

        /// <summary>
        /// 入側単位としての長さ[mm]
        /// </summary>
        public int LengthBase
        {
            get { return m_length_base; }
        }

        /// <summary>
        /// 入側コイルの採寸開始位置[mm]
        /// </summary>
        public int PosFrom
        {
            get { return m_pos_from; }
        }

        /// <summary>
        /// 入側コイルの採寸終了位置[mm]
        /// </summary>
        public int PosTo
        {
            get { return m_pos_to; }
        }

        /// <summary>
        /// 入側コイルの採寸終了位置[mm] (疵を探すときに使う)
        /// </summary>
        internal int PosToDefect
        {
            get { return m_pos_to_defect; }
        }

        /// <summary>
        /// 材料板幅[mm]
        /// </summary>
        public int Width
        {
            get { return m_width; }
        }

        /// <summary>
        /// 注文板厚[mm]
        /// </summary>
        public float Thickness
        {
            get { return m_thickness; }
        }

        /// <summary>
        /// 検査表面状態_表
        /// </summary>
        public string SurfaceUp
        {
            get { return m_surface_up; }
        }

        /// <summary>
        /// 検査表面状態_裏
        /// </summary>
        public string SurfaceDown
        {
            get { return m_surface_dn; }
        }

        /// <summary>
        /// 検査基準_表
        /// </summary>
        public string InspLevelUp
        {
            get { return m_insp_level_up; }
        }

        /// <summary>
        /// 検査基準_裏
        /// </summary>
        public string InspLevelDown
        {
            get { return m_insp_level_dn; }
        }

        /// <summary>
        /// 主材質コード
        /// </summary>
        public string PrimaryContext
        {
            get { return m_primary_context; }
        }

        /// <summary>
        /// 表面処理1桁目
        /// </summary>
        public string SurfProcDig1
        {
            get { return m_surf_proc_dig1; }
        }

        /// <summary>
        /// 表面処理2桁目
        /// </summary>
        public string SurfProcDig2
        {
            get { return m_surf_proc_dig2; }
        }

        /// <summary>
        /// 計画次工程コード
        /// </summary>
        public string NextProcSched
        {
            get { return m_next_proc_sched; }
        }

        /// <summary>
        /// 表保存疵個数
        /// </summary>
        public int TotalNDefectsUp
        {
            get { return m_total_ndefects_up; }
        }

        /// <summary>
        /// 裏保存疵個数
        /// </summary>
        public int TotalNDefectsDn
        {
            get { return m_total_ndefects_dn; }
        }

        #endregion

        #region 判定疵種、グレードごとの疵個数

        /// <summary>
        /// 疵種ごとの疵個数
        /// </summary>
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_type_up = new Dictionary<int, int>();
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_type_dn = new Dictionary<int, int>();

        /// <summary>
        /// グレードごとの疵個数
        /// </summary>
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_sevirity_up = new Dictionary<int, int>();
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_sevirity_dn = new Dictionary<int, int>();

        /// <summary>
        /// 疵種ごとの疵個数をセット
        /// </summary>
        /// <param name="insp_surface">表or裏</param>
        /// <param name="type">疵種</param>
        /// <param name="n">疵個数</param>
        internal void SetNDefectsOnType(SwdData.InspSurface insp_surface, int type, int n)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    m_ndefects_on_type_up[type] = n;
                    break;

                case SwdData.InspSurface.Down:
                    m_ndefects_on_type_dn[type] = n;
                    break;
            }
        }

        /// <summary>
        /// 疵種ごとの疵個数を取得
        /// </summary>
        /// <param name="insp_surface">表or裏</param>
        /// <param name="type">疵種</param>
        /// <returns>疵個数</returns>
        public int GetNDefectsOnType(SwdData.InspSurface insp_surface, int type)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    return m_ndefects_on_type_up.ContainsKey(type) ? m_ndefects_on_type_up[type] : 0;

                case SwdData.InspSurface.Down:
                    return m_ndefects_on_type_dn.ContainsKey(type) ? m_ndefects_on_type_dn[type] : 0;
            }
            return 0;
        }

        /// <summary>
        /// グレードごとの疵個数をセット
        /// </summary>
        /// <param name="insp_surface">表or裏</param>
        /// <param name="sevirity">グレード</param>
        /// <param name="n">疵個数</param>
        internal void SetNDefectsOnSevirity(SwdData.InspSurface insp_surface, int sevirity, int n)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    m_ndefects_on_sevirity_up[sevirity] = n;
                    break;

                case SwdData.InspSurface.Down:
                    m_ndefects_on_sevirity_dn[sevirity] = n;
                    break;
            }
        }

        /// <summary>
        /// グレードごとの疵個数を取得
        /// </summary>
        /// <param name="insp_surface">表or裏</param>
        /// <param name="sevirity">グレード</param>
        /// <returns>疵個数</returns>
        public int GetNDefectsOnSevirity(SwdData.InspSurface insp_surface, int sevirity)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    return m_ndefects_on_sevirity_up.ContainsKey(sevirity) ? m_ndefects_on_sevirity_up[sevirity] : 0;

                case SwdData.InspSurface.Down:
                    return m_ndefects_on_sevirity_dn.ContainsKey(sevirity) ? m_ndefects_on_sevirity_dn[sevirity] : 0;
            }
            return 0;
        }

#endregion

    }
}
