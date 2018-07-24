// =====================================================================================================================//
//                                                                                                                      //
//                                       키움 자동매매 프로그램 제작  C# 초보자 학습용 소스                             //
// 2016.04.14 ~ 2016.05.13                                                                                              //  
// ** 많은 도움을 주신 키움개발자 카페 회원분들께 감사드립니다. http://cafe.naver.com/kiwoomopenapi                                                                                                                     //
// =====================================================================================================================//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using KiwoomCode;


namespace Kiwoom0414
{
    public partial class Form1 : Form
    {

        public struct ConditionList
        {
            public string strConditionName;
            public int nIndex;
        }

        private int _scrNum = 5000;
        private string _strRealConScrNum = "0000";
        private string _strRealConName = "0000";
        private int _nIndex = 0;

        private bool _bRealTrade = false;         //true면 프로그램 실행시 조건검색종목 자동 매수매도                       

        // 화면번호 생산
        private string GetScrNum()
        {
            if (_scrNum < 9999)
                _scrNum++;
            else
                _scrNum = 5000;

            return _scrNum.ToString();
        }

        // 실시간 연결 종료                          
        private void DisconnectAllRealData()
        {
            for (int i = _scrNum; i > 5000; i--)
            {
                axKHOpenAPI.DisconnectRealData(i.ToString());
            }

            _scrNum = 5000;
        }

        // =================================================
        // 폼 초기화 
        // =================================================

        public Form1()
        {
            InitializeComponent();

            axKHOpenAPI.Hide();                // 화면에 OpenAPI 촌스런 로고 없애기

            // 변수 초기화    
            for (int i = 0; i < 10; i++)
            {
                tbl편입종목코드[i] = "";
                tbl편입종목명[i] = "";
            }
        }

        //그리드뷰 속도향상
        public class QuickDataGridView : DataGridView
        {
            public QuickDataGridView()
            {
                DoubleBuffered = true;
            }
        }
        
        //그리드뷰에 번호넣기
        private void dataGridView1_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            var grid = sender as DataGridView;
            var rowIdx = (e.RowIndex + 1).ToString();

            var centerFormat = new StringFormat()
            {
                Alignment = StringAlignment.Center,
                LineAlignment = StringAlignment.Center,
            };
            var headerBounds = new Rectangle(e.RowBounds.Left, e.RowBounds.Top, grid.RowHeadersWidth, e.RowBounds.Height);
            e.Graphics.DrawString(rowIdx, this.Font, SystemBrushes.ControlText, headerBounds, centerFormat);

            // this.dataGridView1.TopLeftHeaderCell.Value = "no";
        }

        // 그리드뷰 색깔 변화
        public void grid_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == -1 || e.RowIndex == -1)
                return;

            DataGridView view = sender as DataGridView;

            if (view == null)
                return;

            DataGridViewCell cell = view[e.ColumnIndex, e.RowIndex];

            string text = cell.FormattedValue.ToString();

            if (text.StartsWith("+") == true)
            {
                cell.Style.ForeColor = Color.Red;
            }
            else if (text.StartsWith("-") == true)
            {
                cell.Style.ForeColor = Color.Blue;
            }
            else if (text.StartsWith("!") == true)
            {
                cell.Style.ForeColor = Color.DarkOrange;
            }
            else
            {
                cell.Style.ForeColor = Color.Black;
            }
        }

        // =================================================================            
        // 로그를 출력합니다.(메인폼 ListBox  "실시간" , "검색코드" 에 출력)
        // =================================================================

        public void Logger(Log type, string format, params Object[] args)
        {
            string message = String.Format(format, args);

            switch (type)
            {
                case Log.실시간:

                    lst실시간.BeginUpdate();        // listView.Items.Add 처리 속도 향상시키기(?)
                    lst실시간.EndUpdate();

                    lst실시간.Items.Add(message);
                    lst실시간.SelectedIndex = lst실시간.Items.Count - 1;
                    break;

                case Log.검색코드:

                    검색코드.BeginUpdate();        // listView.Items.Add 처리 속도 향상시키기
                    검색코드.EndUpdate();

                    검색코드.Items.Add(message);
                    검색코드.SelectedIndex = 검색코드.Items.Count - 1;
                    break;

                case Log.lst매수미체결:

                    lst매수미체결.BeginUpdate();        // listView.Items.Add 처리 속도 향상시키기
                    lst매수미체결.EndUpdate();

                    lst매수미체결.Items.Add(message);
                    lst매수미체결.SelectedIndex = lst매수미체결.Items.Count - 1;
                    break;

                case Log.lst매도미체결:

                    lst매도미체결.BeginUpdate();        // listView.Items.Add 처리 속도 향상시키기
                    lst매도미체결.EndUpdate();

                    lst매도미체결.Items.Add(message);
                    lst매도미체결.SelectedIndex = lst매도미체결.Items.Count - 1;
                    break;
                default:
                    break;
            }
        }

        // 로그인 창을 엽니다.
        private void button3_Click(object sender, EventArgs e)
        {
            if (axKHOpenAPI.CommConnect() == 0)
            {
                Logger(Log.실시간, "// 로그인창 열기 성공!");
                Logger(Log.실시간, "// 최초 접속시 업데이트로 시간이 걸립니다.");
            }
            else
            {
                Logger(Log.실시간, "로그인창 열기 실패");
            }
        }

        // =================================================
        // 프로그램 종료 , 로그아웃
        // =================================================

        //프로그램 종료 빠르게 실행하기
        private void button7_Click(object sender, EventArgs e)
        {
            Application.Exit();
            System.Diagnostics.Process[] mProcess = System.Diagnostics.Process.GetProcessesByName(Application.ProductName);
            foreach (System.Diagnostics.Process p in mProcess)
                p.Kill();
        }

        // 로그아웃
        private void button4_Click(object sender, EventArgs e)
        {
            DisconnectAllRealData();
            axKHOpenAPI.CommTerminate();
            Logger(Log.실시간, "로그아웃");
        }

        // 키움증권 Open API 접속상태 확인
        private void button5_Click(object sender, EventArgs e)
        {
            if (axKHOpenAPI.GetConnectState() == 0)
            {
                Logger(Log.실시간, "Open API 연결 : 미연결");
            }
            else
            {
                Logger(Log.실시간, "Open API 연결 : 서버와 연결되었습니다.");
            }
        }

        // ==========================================<< 2. 조회 요청한 일반 데이터(OnReceiveTrData) TR수신부(비실시간) >>==================================//
        //               
        //  일반 조회 요청한 모든 TR은 여기서 수신된다.
        //
        // ================================================================================================================================================//

        private void axKHOpenAPI_OnReceiveTrData(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveTrDataEvent e)
        {
            int int현재가;
            int int거래량;
            int int예수금;
            int int주문가능금액;
            int int실현손익;
            int int유가잔고평가액;
            int int매수금액;
            int int매도금액;
            int int매매수수료;
            int int매매세금;
            int int평가손익;
            int int매입금액;

            string str종목코드;
            string str현재가;
            Single f등락률;
            string str등락률;
            string str등락률old;
            string str거래량;
            string str전일대비기호;
            string str보유수량;
            string str매입가;
            string str매입금액;
            string str평가금액;
            string str평가손익;
            string str수익률;
            string str예수금;
            string str주문가능금액;
            string str실현손익;
            string str유가잔고평가액;
            string str매수금액;
            string str매도금액;
            string str매매수수료;
            string str매매세금;
            string str주문수량;
            string str미체결수량;
            string str원주문번호;
            double d당일수익률;

            //----------------------------------------------------
            // 2. [실시간미체결요청] 매수/매도 미체결  수신               ========================================================>>>>>>>> 작 업 중 <<<<<<<<<<<<<<<<<<<<<<<<<<<
            //----------------------------------------------------
            // opt10075

            if (e.sRQName == "매수미체결")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);

                for (int i = 0; i < nCnt; i++)
                {
                    str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목코드").Trim();
                    str종목명 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim();
                    str주문수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "주문수량").TrimStart('0');
                    str미체결수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "미체결수량").TrimStart('0');
                    str원주문번호 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "원주문번호").Trim();

                    Logger(Log.lst매수미체결, e.sRQName + ">>종목코드:{0},종목명:{1},주문수량:{2},미체결수량:{3},원주문번호:{4}", str종목코드, str종목명, str주문수량, str미체결수량, str원주문번호);
                }
            }
            else if (e.sRQName == "매도미체결")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);

                for (int i = 0; i < nCnt; i++)
                {
                    str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목코드").Trim();
                    str종목명 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim();
                    str주문수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "주문수량").TrimStart('0');
                    str미체결수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "미체결수량").TrimStart('0');
                    str원주문번호 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "원주문번호").Trim();

                    Logger(Log.lst매도미체결, e.sRQName + ">>종목코드:{0},종목명:{1},주문수량:{2},미체결수량:{3},원주문번호:{4}", str종목코드, str종목명, str주문수량, str미체결수량, str원주문번호);
                }
            }


            // =========================================================================================
            // 2-1.  실시간 "관심종목정보" 수신 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            // =========================================================================================

            //Logger(Log.검색코드, ">>OnReceiveTrData=" + e.sRQName);               // 요청한 수신데이터 출력로그 


            // 다수의 종목을 요청하는 관심종목정보로 주식시세 수신 
            else if (e.sRQName == "관심종목정보")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);

                for (int i = 0; i < nCnt; i++)
                {
                    str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목코드").Trim();
                    str현재가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "현재가").Trim();
                    str거래량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "거래량").Trim();
                    str전일대비기호 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "전일대비기호").Trim();

                    if (Int32.TryParse(str현재가, out int현재가))
                        str현재가 = String.Format("{0:###,###,##0 원}", Math.Abs(int현재가));        // Math.Abs(); 로 현재가앞에 - 부호 제거

                    str등락률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "등락율").Trim();
                    str등락률old = str등락률;

                    if (Single.TryParse(str등락률, out f등락률))
                        str등락률 = string.Format("{0:###,##0.#0}", f등락률) + " %";

                    else
                        str등락률 = str등락률old;


                    if (Int32.TryParse(str거래량, out int거래량))
                        str거래량 = String.Format("{0:###,###,##0 주}", int거래량);

                    // ================================================================================================
                    // 수신한 "관심종목정보" 실시간데이터를(현재가,거래량,전일대비기호)  f그리드업데이트 함수 활용 출력
                    // ================================================================================================

                    f그리드업데이트(str종목코드, str현재가, str등락률, str거래량, str전일대비기호, "", "", "", "");              // 주식시세 정보만 출력하고 보유수량 정보는 별도로 출력하므로 "" 공란으로 만듬
                }
            }

            // ===========================================================================================
            // 2-2. "계좌평가잔고내역요청" 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분
            //       조건검색된 종목을 매수하면 보유수량,매입가,평가금액,수익률을 업데이트 한다. 
            // ===========================================================================================
            // opw00018
            
            else if (e.sRQName == "계좌평가잔고내역")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);           // 데이터 갯수 얻기
                int nPrevNext = 0;

                if (e.sPrevNext != null && !e.sPrevNext.Equals(""))
                {
                    nPrevNext = Int32.Parse(e.sPrevNext);                            //연속데이터 존재여부
                }

                //Logger(Log.검색코드, e.sRQName + ">>> 갯수= " + String.Format("{0:#,#}", nCnt));
                {
                    for (int i = 0; i < nCnt; i++)
                    {
                        str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목번호").Trim();                     // 종목코드가 종목번호로 사용됨(계좌잔고평가현황에서)
                        str종목코드 = str종목코드.Substring(str종목코드.Length - 6);                                               //<-- 모의투자는 종목코드 앞에 A가 붙이서 A 뺴고 6자리로 만들기
                        str보유수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "보유수량").TrimStart('0');             //string data = "003210";   Console.WriteLine(data.TrimStart('0'));
                        str매입가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매입가").TrimStart('0');                 // 수신데이터 000000060  삭제 TrimStart('0')
                        str평가금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "평가금액").TrimStart('0');
                        str수익률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "수익률(%)").TrimStart('0');

                        //Logger(Log.검색코드, e.sRQName + ">>> 종목:{0},수량:{1},매입가:{2},평가금액:{3},수익률:{4}", str종목코드, str보유수량, str매입가, str평가금액, str수익률);

                        // 데이터그리드뷰 헤더 타이틀 명칭 바꾸기 {보유잔고와 매수종목갱신간의 타이틀이 다르므로 교대로 바뀜)
                        DataGridViewCellStyle columnHeaderStyle = new DataGridViewCellStyle();
                        columnHeaderStyle.BackColor = Color.Beige;
                        dataGridView1.ColumnHeadersDefaultCellStyle = columnHeaderStyle;
                        dataGridView1.Columns[3].HeaderText = "등락률";
                        dataGridView1.Columns[4].HeaderText = "거래량";
                        dataGridView1.Columns[5].HeaderText = "기호";
                        dataGridView1.Columns[8].HeaderText = "평가금액";

                        // =========================================================================================
                        // 현재가 수신한 일반데이터(보유수량,매입가,평가금액,수익률)  f그리드업데이트 함수 활용 출력
                        // =========================================================================================

                       f그리드업데이트(str종목코드, "", "", "", "", str보유수량, str매입가, str평가금액, str수익률);               // 위에서 시세정보는 출력했으므로 나머지 보유잔고쪽 출력

                    }

                    // 연속조회가 있을 경우 다음 화면 요청하여 출력
                    if (nPrevNext == 2)
                    {
                        GetMyAccountState(nPrevNext);
                    }
                    else
                    {
                        //nPrevNext = 0;                     // 이부분에 작성할 코드를 잘 모르겠음 
                    }

                }
            }

            // =====================================================================================
            // 2-3. "주식기본정보" 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            // =====================================================================================
            // OPT1001 : 주식기본정보 

            else if (e.sRQName == "주식기본정보")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);

                for (int i = 0; i < nCnt; i++)
                {
                    str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목코드").Trim();
                    str현재가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "현재가").Trim();

                    if (Int32.TryParse(str현재가, out int현재가))
                        str현재가 = String.Format("{0:###,###,##0 원}", int현재가);

                    str등락률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "등락율").Trim();
                    str등락률old = str등락률;

                    if (Single.TryParse(str등락률, out f등락률))
                        str등락률 = string.Format("{0:###,##0.#0}", f등락률) + " %";
                    else
                        str등락률 = str등락률old;

                    str전일대비기호 = "";
                    str보유수량 = "";
                    str매입가 = "";
                    str평가금액 = "";
                    str수익률 = "";

                    //Logger(Log.검색코드, ">>주식기본정보 :" + str종목코드 + " {0} | 현재가:{1:N0} | 등락율:{2} | 거래량:{3:N0} ",
                    //    axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim(), str현재가, str등락률,
                    //    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "거래량").Trim()));

                    // ==================================================================================
                    // 현재가 수신한 데이터를 f그리드업데이트 함수 활용 출력
                    // ==================================================================================

                    f그리드업데이트(str종목코드, str현재가, str등락률,
                                   (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "거래량").Trim()),
                                   str전일대비기호,
                                   (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "보유수량").Trim()),
                                   (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매입가").Trim()),
                                   (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "평가금액").Trim()),
                                   (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "수익률").Trim()));
                }

                if (e.sRQName == "주식주문")
                {
                    string s원주문번호 = axKHOpenAPI.GetCommData(e.sTrCode, "", 0, "").Trim();

                    long n원주문번호 = 0;
                    bool canConvert = long.TryParse(s원주문번호, out n원주문번호);


                    if (canConvert == true)
                        txt원주문번호.Text = s원주문번호;
                    else
                        Logger(Log.실시간, "잘못된 원주문번호 입니다");
                }
            }

            // =========================================================================================
            // 2-4. "예수금상세현황요청" 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            // =========================================================================================
            // opw00001

            else if (e.sRQName == "예수금상세현황요청")
            {
                str예수금 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "예수금").TrimStart('0');
                str주문가능금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "주문가능금액").TrimStart('0');

                if (Int32.TryParse(str예수금, out int예수금))
                    str예수금 = String.Format("{0:#,# 원}", int예수금);
                if (Int32.TryParse(str주문가능금액, out int주문가능금액))
                    str주문가능금액 = String.Format("{0:#,# 원}", int주문가능금액);

                // 조회데이터 출력하기
                dataGridView2.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;                    // 텍스트 오른쪽 정렬 사용 MiddleRight
                dataGridView2.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[0].Value = str예수금;
                dataGridView2.Rows[i].Cells[1].Value = str주문가능금액;

                richTextBox1.Text = str예수금;                 // 수익률 전광판 
                richTextBox1.Text = "수익 달성!";             // 수익률 전광판   

                // Logger(Log.검색코드, e.sRQName + ">>> 예수금:{0},주문가능금액:{1}", str예수금, str주문가능금액);
            }

            // =========================================================================================
            // 2-5.  [일자별실현손익요청] 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            // =========================================================================================
            // opt10074
            else if (e.sRQName == "일자별실현손익요청")
            {
                str실현손익 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "실현손익").TrimStart('0');
                str매수금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매수금액").TrimStart('0');
                str매도금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매도금액").TrimStart('0');
                str매매수수료 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매매수수료").TrimStart('0');
                str매매세금 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매매세금").TrimStart('0');

                if (Int32.TryParse(str실현손익, out int실현손익))
                    str실현손익 = String.Format("{0:#,# 원}", int실현손익);
                if (Int32.TryParse(str매수금액, out int매수금액))
                    str매수금액 = String.Format("{0:#,# 원}", int매수금액);
                if (Int32.TryParse(str매도금액, out int매도금액))
                    str매도금액 = String.Format("{0:#,# 원}", int매도금액);
                if (Int32.TryParse(str매매수수료, out int매매수수료))
                    str매매수수료 = String.Format("{0:#,# 원}", int매매수수료);
                if (Int32.TryParse(str매매세금, out int매매세금))
                    str매매세금 = String.Format("{0:#,# 원}", int매매세금);

                double d실현손익;
                double d매수금액;
                d실현손익 = int실현손익;
                d매수금액 = int매수금액;

                d당일수익률 = d실현손익 / d매수금액 * 100;             // 당일 수익률 계산

                // 조회데이터 출력하기
                dataGridView2.Columns[7].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[7].Value = str실현손익;
                dataGridView2.Columns[3].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[3].Value = str매수금액;
                dataGridView2.Columns[4].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[4].Value = str매도금액;
                dataGridView2.Columns[5].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[5].Value = str매매수수료;
                dataGridView2.Columns[6].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[6].Value = str매매세금;
                dataGridView2.Columns[8].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[8].Value = Math.Round(d당일수익률, 2) + "%";            // 당일수익률 소수점 셋째자리 반올림Math.Round(대상,자릿수) 

                richTextBox1.Text = str실현손익;                // 수익률 전광판
            }

            // ==================================================================================================
            // 2-6.  [계좌평가현황요청] 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            //        유가잔고평가액 갱신 
            // ==================================================================================================
            // OPW00004

            else if (e.sRQName == "계좌평가현황요청")
            {
                str유가잔고평가액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "유가잔고평가액").Trim();

                if (Int32.TryParse(str유가잔고평가액, out int유가잔고평가액))
                    str유가잔고평가액 = String.Format("{0:#,# 원}", int유가잔고평가액);

                // 조회데이터 출력하기
                dataGridView2.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                dataGridView2.Rows[i].Cells[2].Value = str유가잔고평가액;
            }

            // ==================================================================================================
            // 2-7.  [전업종지수요청] 수신한 데이터를 데이터그리드뷰 형식에 맞게 업데이트해 주는 부분 
            //        상태바 하단에 업종지수 갱신 
            // ==================================================================================================
            // opt20003

            else if (e.sRQName == "전업종지수요청")
            {

                timer1.Interval = 10000;
                timer1.Start();

                str종목명 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim();
                str현재가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "현재가").Trim();
                str등락률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "등락률").Trim();

                // 상태바에 코스피지수 출력하기
                lbl코스피.Text = str종목명 + " " + str현재가 + " " + str등락률;
            }

            //=======================================================
            // 2-8. [보유주식현황] 출력하기 (총보유 주식을 출력)         ========== 20개 넘으면 중복 출력됨 , 해결 못함 
            //=======================================================
            // opw00018

            else if (e.sRQName == "보유주식현황")
            {

                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);          // 데이터 갯수 얻기
                int nPrevNext = 0;

                if (e.sPrevNext != null && !e.sPrevNext.Equals(""))
                {
                    nPrevNext = Int32.Parse(e.sPrevNext);           //연속데이터 존재여부
                }

                // Logger(Log.검색코드, e.sRQName + ">>> 갯수= " + String.Format("{0:#,#}", nCnt));

                dataGridView1.Rows.Add(nCnt);

                //dataGridView1.Rows.Add(row1);
                //dataGridView1.Rows.Add(row);
                //dataGridView1.RowCount = nCnt;                    // 데이터그리드뷰 Row 카운트 (row 갯수 지정을 미리해서 직접 위치 지정해서 넣을수 있게 ?
                //dataGridView1.Rows.Add(dataGridView1.Rows.Count + 1);
                {

                    for (int i = 0; i < nCnt; i++)
                    {
                        str종목코드 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목번호").Trim();            // 종목코드가 종목번호로 사용됨(계좌잔고평가현황에서)
                        str종목코드 = str종목코드.Substring(str종목코드.Length - 6);                                      //<-- 모의투자는 종목코드 앞에 A가 붙이서 A 빼고 6자리로 만들기
                        str종목명 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim();
                        str보유수량 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "보유수량").TrimStart('0');    //string data = "003210";   Console.WriteLine(data.TrimStart('0'));
                        str매입가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매입가").TrimStart('0');        // 수신데이터 000000060  삭제 TrimStart('0')
                        str매입금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "매입금액").TrimStart('0');    // 수신데이터 000000060  삭제 TrimStart('0')
                        str평가금액 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "평가금액").TrimStart('0');
                        str수익률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "수익률(%)").TrimStart('0');
                        str현재가 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "현재가").Trim();
                        str평가손익 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "평가손익").Trim();
                        str등락률 = axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "등락율").Trim();
                        str등락률old = str등락률;

                        if (Single.TryParse(str등락률, out f등락률))
                            str등락률 = string.Format("{0:###,##0.#0}", f등락률) + " %";
                        else
                            str등락률 = str등락률old;
                        if (Int32.TryParse(str현재가, out int현재가))
                            str현재가 = String.Format("{0:###,###,##0 원}", Math.Abs(int현재가));         //Math.Abs(); 로 현재가앞에 - 부호 제거
                        if (Int32.TryParse(str보유수량, out int보유수량))
                            str보유수량 = String.Format("{0:#,# 주}", int보유수량);                       //천단위 콤마 사용
                        if (Int32.TryParse(str매입가, out int매입가))
                            str매입가 = String.Format("{0:#,# 원}", int매입가);                           //천단위 콤마 사용
                        if (Int32.TryParse(str매입금액, out int매입금액))
                            str매입금액 = String.Format("{0:#,# 원}", int매입금액);                       //천단위 콤마 사용
                        if (Int32.TryParse(str평가금액, out int평가금액))
                            str평가금액 = String.Format("{0:#,# 원}", int평가금액);                       //천단위 콤마 사용
                        if (Int32.TryParse(str평가손익, out int평가손익))
                            str평가손익 = String.Format("{0:#,# 원}", int평가손익);

                        str수익률old = str수익률;
                        if (Single.TryParse(str수익률, out f수익률))
                            str수익률 = String.Format("{0:0.00}%", f수익률);

                        // 보유수량 헤더 타이틀 명칭 바꾸기 
                        DataGridViewCellStyle columnHeaderStyle = new DataGridViewCellStyle();
                        columnHeaderStyle.BackColor = Color.Beige;
                        dataGridView1.ColumnHeadersDefaultCellStyle = columnHeaderStyle;
                        dataGridView1.Columns[3].HeaderText = "등락률";
                        dataGridView1.Columns[4].HeaderText = "평가손익";
                        dataGridView1.Columns[5].HeaderText = "매입금액";
                        dataGridView1.Columns[8].HeaderText = "총평가금액";

                        // 데이터그리드뷰에 보유종목 현황 출력
                        //dataGridView1.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;          // 텍스트 오른쪽 정렬 사용 MiddleRight
                        dataGridView1.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
                        dataGridView1.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[3].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[4].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[5].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[6].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[7].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[8].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                        dataGridView1.Columns[9].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                        dataGridView1.Rows[i].Cells[0].Value = str종목코드;
                        dataGridView1.Rows[i].Cells[1].Value = str종목명;
                        dataGridView1.Rows[i].Cells[2].Value = str현재가;
                        dataGridView1.Rows[i].Cells[3].Value = str등락률;
                        dataGridView1.Rows[i].Cells[4].Value = str평가손익;
                        dataGridView1.Rows[i].Cells[5].Value = str매입금액;
                        dataGridView1.Rows[i].Cells[6].Value = str보유수량;
                        dataGridView1.Rows[i].Cells[7].Value = str매입가;
                        dataGridView1.Rows[i].Cells[8].Value = str평가금액;
                        dataGridView1.Rows[i].Cells[9].Value = str수익률;
                    }

                    // 연속조회가 있을 경우 다음 화면 요청하여 출력
                    if (nPrevNext == 2)
                    {
                        MyAccountState(nPrevNext);
                    }
                    else
                    {
                        //    isUsingListMyStock = false;     //MyAccountState(nPrevNext=0);                    // 이부분에 작성할 코드를 잘 모르겠음 
                    }

                }

            }

            // =================================================
            // OPT10081 : 주식일봉차트조회
            // 키움증권 일자별 시세조회하는 부분 
            // =================================================

            else if (e.sRQName == "주식일봉차트조회")
            {
                int nCnt = axKHOpenAPI.GetRepeatCnt(e.sTrCode, e.sRQName);

                for (int i = 0; i < nCnt; i++)
                {
                    //데이터그리드뷰에 종목 기간별 현재가 출력하기
                    this.dataGridView1.Rows.Add(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목코드").Trim(),
                     (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "종목명").Trim()),
                    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "현재가").Trim()),
                    (axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "등락율").Trim()),
                    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "거래량").Trim()),
                    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "시가").Trim()),
                    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "고가").Trim()),
                    Int32.Parse(axKHOpenAPI.CommGetData(e.sTrCode, "", e.sRQName, i, "저가").Trim()));
                }
            }
        }

        // =================================================
        //  로그인 이벤트 함수
        // =================================================

        private void axKHOpenAPI_OnEventConnect(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnEventConnectEvent e)
        {
            if (Error.IsError(e.nErrCode))
            {
                Logger(Log.실시간, "// [로그인 처리결과] " + Error.GetErrorMessage());

                // 조건식 로컬저장 자동으로 진행하기
                System.Threading.Thread.Sleep(1000);         //  로그인 성공후 잠시 기다려서 조건식 불러오기 자동 실행하기
                int lRet;
                lRet = axKHOpenAPI.GetConditionLoad();

                // 계좌번호 불러오기
                lbl아이디.Text = axKHOpenAPI.GetLoginInfo("USER_ID");
                lbl이름.Text = axKHOpenAPI.GetLoginInfo("USER_NAME");
                string[] arr계좌 = axKHOpenAPI.GetLoginInfo("ACCNO").Split(';');
                cbo계좌.Items.AddRange(arr계좌);
                cbo계좌.SelectedIndex = 0;
            }
            else
            {
                Logger(Log.실시간, "로그인창 열기 실패");
                Logger(Log.실시간, "로그인 실패로 조건식리스트 불러오기 실패");
            }
        }

        // ==================================================<<매수 매도주문 체결 정보(OnReceiveChejanData) 수신부>>==============================//
        //
        // 체결 잔고 수신 이벤트 함수
        // 매수 매도주문 체결 정보 출력부
        //
        // =======================================================================================================================================//
        int int현재가;
        int int거래량;
        int int매입가;
        int int보유수량;
        int int평가금액;

        Single f등락률;
        Single f수익률;
        //string str수익률;
        string str수익률old;
        //string str등락률;
        string str등락률old;
        //string str거래량;
        //string str전일대비기호;
        //string str보유수량;
        //string str매입가;
        //string str평가금액;

        private void axKHOpenAPI_OnReceiveChejanData(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveChejanDataEvent e)
        {
            if (e.sGubun == "0")
            {
                // this.dataGridView1.Rows.Add(axKHOpenAPI.GetChejanData(911));

                Logger(Log.검색코드, "구분 : 주문체결통보");             // 매수 매도후 주문 체결 출력하는 부분
                Logger(Log.검색코드, "주문/체결시간 : " + axKHOpenAPI.GetChejanData(908));
                Logger(Log.검색코드, "종목명 : " + axKHOpenAPI.GetChejanData(302));
                Logger(Log.검색코드, "주문수량 : " + axKHOpenAPI.GetChejanData(900));
                Logger(Log.검색코드, "주문가격 : " + axKHOpenAPI.GetChejanData(901));
                Logger(Log.검색코드, "체결수량 : " + axKHOpenAPI.GetChejanData(911));
                Logger(Log.검색코드, "체결가격 : " + axKHOpenAPI.GetChejanData(910));
                Logger(Log.검색코드, "=======================================");
            }
            else if (e.sGubun == "1")
            {
                Logger(Log.검색코드, "구분 : 잔고통보");
            }
            else if (e.sGubun == "3")
            {
                Logger(Log.검색코드, "구분 : 특이신호");
            }
        }

        // ===============================================================<< 메세지(OnReceiveMsg)  수신부>> ========================================//
        //
        // 메세지 수신 이벤트 함수
        // 종목코드 메세지수신
        // 
        // =========================================================================================================================================//

        private void axKHOpenAPI_OnReceiveMsg(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveMsgEvent e)
        {
            Logger(Log.실시간, "=================<< 특별한 소식>>=====================");
            Logger(Log.실시간, "화면번호:{0} | RQName:{1} | TRCode:{2} | 메세지:{3}", e.sScrNo, e.sRQName, e.sTrCode, e.sMsg);
        }

        // =======================================================<<실시간 f그리드업데이트 함수 최초 구현부>> ======================================//
        // 
        // 종목코드만 넣고 추가 값을 넣으면 알아서 그리드에서 종목코드 찾아서 그 위치에 넣어줌
        //
        //==========================================================================================================================================//
        private void f그리드업데이트(string str종목코드, string str현재가, string str등락률, string str거래량,
                                     string str전일대비기호, string str보유수량, string str매입가, string str평가금액, string str수익률)
        {
            int IDX;
            string str종목코드2;

            // i 값 찾기  (종목코드 비교)
            IDX = -1;
            for (int i = 0; i < dataGridView1.Rows.Count; i++)
            {
                // 종목코드와 그리드 종목코드를 비교하여 같으면 루프 돌리기 중단!!!
                str종목코드2 = dataGridView1.Rows[i].Cells[0].Value.ToString();
                str종목코드2 = str종목코드2.Trim();

                if (str종목코드 == str종목코드2)
                {
                    //Logger(Log.실시간, "@@@@@ 찾음>> i={0} | str종목코드 : {2}", i, str종목코드2);                               // 로그 찍는 연습 
                    IDX = i;
                    break;  // 루프 중단
                }
            }

            // 종목코드를 비교해서 찾았으면 아래에서 i 값을 이용해서 처리
            if (IDX > -1)
            {
                //Logger(Log.실시간, " >>!!!종목찾음=" + str종목코드 + "|" + dataGridView1.Rows[IDX].Cells[0].Value);            // 제대로 수신되는지 확인용 로그 작성 ******

                // 같으면 해당줄에 현재가 업데이트  
                if (Int32.TryParse(str현재가, out int현재가))
                    str현재가 = String.Format("{0:###,###,##0 원}", Math.Abs(int현재가));            //천단위 콤마 사용 및 현재가앞 - 부호 제거

                str등락률old = str등락률;
                if (Single.TryParse(str등락률, out f등락률))
                    str등락률 = string.Format(" {0:0.00}% ", f등락률);        // 등락률에 % 기호 추가
                else
                    str등락률 = str등락률old;
                if (Int32.TryParse(str거래량, out int거래량))
                    str거래량 = String.Format("{0:#,# 주}", int거래량);
                if (Int32.TryParse(str보유수량, out int보유수량))
                    str보유수량 = String.Format("{0:#,# 주}", int보유수량);
                if (Int32.TryParse(str매입가, out int매입가))
                    str매입가 = String.Format("{0:#,# 원}", int매입가);
                if (Int32.TryParse(str평가금액, out int평가금액))
                    str평가금액 = String.Format("{0:#,# 원}", int평가금액);

                str수익률old = str수익률;
                if (Single.TryParse(str수익률, out f수익률))
                    str수익률 = String.Format("{0:0.00}%", f수익률);

                //==============================================================
                // 전일대비기호를 그림(문자)으로 교체, 글자색 바꾸기 ,강조하기 
                //==============================================================
                {
                    if (str전일대비기호 != "")
                    {
                        if (str전일대비기호 == "2")             // 2번은 상승의미
                        {
                            str전일대비기호 = "상승";
                        }
                        else if (str전일대비기호 == "5")        // 5번은 하락의미
                        {
                            str전일대비기호 = "하락";
                        }
                        else if (str전일대비기호 == "1")        // 1번은 상한의미
                        {
                            str전일대비기호 = "상한";
                        }
                        else if (str전일대비기호 == "3")        // 3번은 보합의미
                        {
                            str전일대비기호 = "보합";
                        }
                        dataGridView1.Rows[IDX].Cells[5].Value = str전일대비기호;
                        dataGridView1.Rows[IDX].Cells[5].Style.Font = new Font("Fixsys", 9);                                       // Cell 폰트 지정하기
                        if (str전일대비기호 == "상승") dataGridView1.Rows[IDX].Cells[5].Style.ForeColor = Color.Red;               // 상승은 빨강색
                        else if (str전일대비기호 == "하락") dataGridView1.Rows[IDX].Cells[5].Style.ForeColor = Color.Blue;
                        else if (str전일대비기호 == "상한")
                        {
                            dataGridView1.Rows[IDX].Cells[5].Style.ForeColor = Color.Red;                                              // 상한가 빨강색 
                            dataGridView1.Rows[IDX].Cells[5].Style.Font = new Font("Fixsys", 9, FontStyle.Bold);                       // 상한가 강조
                        }
                        else if (str전일대비기호 == "보합") dataGridView1.Rows[IDX].Cells[5].Style.ForeColor = Color.Black;
                        else dataGridView1.Rows[IDX].Cells[5].Style.ForeColor = Color.Black;
                    }
                }

                dataGridView1.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;                    // 텍스트 오른쪽 정렬 사용 MiddleRight
                if (str현재가 != "") { dataGridView1.Rows[IDX].Cells[2].Value = str현재가; }                                       // 수신정보 비어있는 경우 갱신 안함 
                dataGridView1.Columns[3].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                if (str등락률 != "") { dataGridView1.Rows[IDX].Cells[3].Value = str등락률; }
                dataGridView1.Columns[4].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
                if (str거래량 != "") { dataGridView1.Rows[IDX].Cells[4].Value = str거래량; }
                if (str전일대비기호 != "") { dataGridView1.Rows[IDX].Cells[5].Value = str전일대비기호; }
                if (str보유수량 != "") { dataGridView1.Rows[IDX].Cells[6].Value = str보유수량; }
                if (str매입가 != "") { dataGridView1.Rows[IDX].Cells[7].Value = str매입가; }
                if (str평가금액 != "") { dataGridView1.Rows[IDX].Cells[8].Value = str평가금액; }
                if (str수익률 != "") { dataGridView1.Rows[IDX].Cells[9].Value = str수익률; }
            }
            return;
        }

        // =====================================================<< 3. 실시간 데이터(OnReceiveRealData) 수신부 >>===================================================//
        //
        //                                           << 실시간 데이터 수신부 : 장중에만 신호 발생한다. 휴일엔 수신 불가 >>
        //                                                    실시간 주식시세 수신하여 데이터그리드뷰에 직접 출력
        // ========================================================================================================================================================//
        private void axKHOpenAPI_OnReceiveRealData(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveRealDataEvent e)
        {
            if (e.sRealType == "주식체결")                  // "주식시세"가 안올때 "주식체결"로 수신함           
            {
                //Logger(Log.실시간, "종목코드 : {0} | 현재가 : {1:C} | 등락율 : {2} | 누적거래량 : {3:N0} ", e.sRealKey,
                //                     Int32.Parse(axKHOpenAPI.GetCommRealData(e.sRealType, 10).Trim()),
                //                    axKHOpenAPI.GetCommRealData(e.sRealType, 12).Trim(),
                //                     Int32.Parse(axKHOpenAPI.GetCommRealData(e.sRealType, 13).Trim()));

                // ========================================================================
                // 실시간 수신한 주식체결!!  데이터를 데이터그리드뷰에 업데이트해주는 부분
                // ========================================================================

                f그리드업데이트(e.sRealKey, axKHOpenAPI.GetCommRealData(e.sRealType, 10).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 12).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 13).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 25).Trim(), "", "", "", "");
            }
            else if (e.sRealType == "주식시세")
            {
                //Logger(Log.실시간, e.sRealType + " - 종목코드 : {0} | RealType : {1} | RealData : {2}", e.sRealKey, e.sRealType, e.sRealData);

                // ========================================================================
                // 실시간 수신한 주식시세!!   데이터를 데이터그리드뷰에 업데이트해주는 부분
                // ========================================================================
                f그리드업데이트(e.sRealKey, axKHOpenAPI.GetCommRealData(e.sRealType, 10).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 12).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 13).Trim(),
                                            axKHOpenAPI.GetCommRealData(e.sRealType, 25).Trim(), "", "", "", "");
            }
        }

        // =================================================
        // 수동으로 계좌정보 조회
        // =================================================

        private void button6_Click(object sender, EventArgs e)
        {
            lbl아이디.Text = axKHOpenAPI.GetLoginInfo("USER_ID");
            lbl이름.Text = axKHOpenAPI.GetLoginInfo("USER_NAME");

            string[] arr계좌 = axKHOpenAPI.GetLoginInfo("ACCNO").Split(';');

            cbo계좌.Items.AddRange(arr계좌);
            cbo계좌.SelectedIndex = 0;
        }

        // =================================================
        // 단일종목 "주식기본정보"로 현재가 조회
        // =================================================
        // OPT10001
        private void button8_Click(object sender, EventArgs e)
        {
            // txt주문종목코드
            axKHOpenAPI.SetInputValue("종목코드", txt종목코드.Text.Trim());

            int nRet = axKHOpenAPI.CommRqData("주식기본정보", "OPT10001", 0, GetScrNum());
            _scrNum++;

            if (Error.IsError(nRet))
            {
                Logger(Log.실시간, "[OPT10001] : " + Error.GetErrorMessage());
            }
            else
            {
                Logger(Log.실시간, "[OPT10001] : " + Error.GetErrorMessage());
            }
        }

        // =================================================
        // 단일종목 "주식일봉차트조회"로 기간별 조회
        // =================================================
        // OPT10081
        private void button9_Click(object sender, EventArgs e)
        {
            string today = DateTime.Now.ToString("yyyyMMdd");                //오늘 날짜 조회

            axKHOpenAPI.SetInputValue("종목코드", txt종목코드.Text.Trim());
            axKHOpenAPI.SetInputValue("기준일자", today);                   // 오늘 날짜 기준
            axKHOpenAPI.SetInputValue("수정주가구분", "1");

            int nRet = axKHOpenAPI.CommRqData("주식일봉차트조회", "OPT10081", 0, GetScrNum());
            _scrNum++;

            if (Error.IsError(nRet))
            {
                Logger(Log.실시간, "[OPT10081] : " + Error.GetErrorMessage());
            }
            else
            {
                Logger(Log.실시간, "[OPT10081] : " + Error.GetErrorMessage());
            }
        }

        // ========================================================== <<메인폼 실행>> ===========================================================//
        //
        //
        //
        // ======================================================================================================================================//
        private void Form1_Load(object sender, EventArgs e)
        {
            // =================================================
            // 거래구분목록 지정
            for (int i = 0; i < 13; i++)
                cbo거래구분.Items.Add(KOACode.hogaGb[i].name);

            cbo거래구분.SelectedIndex = 0;                                                                                    // 콤보막스 첫번째 항목을 기본으로 설정 "0" 

            // =================================================
            // 주문유형
            for (int i = 0; i < 5; i++)
                cbo매매구분.Items.Add(KOACode.orderType[i].name);

            cbo매매구분.SelectedIndex = 0;
        }

        private void txt주문종목코드_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        private void txt주문수량_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        private void txt주문가격_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        private void txt원주문번호_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        private void txt종목코드_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        private void txt조회날짜_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || e.KeyChar == Convert.ToChar(Keys.Back)))
            {
                e.Handled = true;
            }
        }

        //============================================================= << 수동 주문 >> ========================================================//
        //
        // 종목별 수동 주문 
        //
        //=====================================================================================================================================//

        private void btn주문_Click(object sender, EventArgs e)
        {
            // 계좌번호 입력 여부 확인
            if (cbo계좌.Text.Length != 10)
            {
                Logger(Log.검색코드, "계좌번호 10자리를 입력해 주세요");

                return;
            }

            // 종목코드 입력 여부 확인
            if (txt주문종목코드.TextLength != 6)
            {
                Logger(Log.검색코드, "종목코드 6자리를 입력해 주세요");

                return;
            }

            // 주문수량 입력 여부 확인
            int n주문수량;

            if (txt주문수량.TextLength > 0)
            {
                n주문수량 = Int32.Parse(txt주문수량.Text.Trim());
            }
            else
            {
                Logger(Log.검색코드, "주문수량을 입력하지 않았습니다");

                return;
            }

            if (n주문수량 < 1)
            {
                Logger(Log.검색코드, "주문수량이 1보다 작습니다");

                return;
            }

            // ======================================================================
            // 거래구분 취득
            // 0:지정가, 3:시장가, 5:조건부지정가, 6:최유리지정가, 7:최우선지정가,
            // 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK,
            // 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:시간외종가
            // ======================================================================
            string s거래구분;
            s거래구분 = KOACode.hogaGb[cbo거래구분.SelectedIndex].code;

            // 주문가격 입력 여부
            int n주문가격 = 0;

            if (txt주문가격.TextLength > 0)
            {
                n주문가격 = Int32.Parse(txt주문가격.Text.Trim());
            }

            if (s거래구분 == "3" || s거래구분 == "13" || s거래구분 == "23" && n주문가격 < 1)
            {
                Logger(Log.검색코드, "주문가격이 1보다 작습니다");
            }

            // ====================================
            // 매매구분 취득
            // (1:신규매수, 2:신규매도 3:매수취소, 
            // 4:매도취소, 5:매수정정, 6:매도정정)
            // ====================================
            int n매매구분;
            n매매구분 = KOACode.orderType[cbo매매구분.SelectedIndex].code;

            // 원주문번호 입력 여부
            if (n매매구분 > 2 && txt원주문번호.TextLength < 1)
            {
                Logger(Log.검색코드, "원주문번호를 입력해주세요");
            }

            // =============================
            // 주식 수동 매수/ 매도 처리부
            // =============================
            int lRet;

            lRet = axKHOpenAPI.SendOrder("주식주문", GetScrNum(), cbo계좌.Text.Trim(),
                                        n매매구분, txt주문종목코드.Text.Trim(), n주문수량,
                                        n주문가격, s거래구분, txt원주문번호.Text.Trim());
            if (lRet == 0)
            {
                Logger(Log.실시간, "주문이 전송 되었습니다");
            }
            else
            {
                Logger(Log.실시간, "주문이 전송 실패 하였습니다. [에러] : " + lRet);
            }
        }

        // =====================================================<<조건식 저장 및 조건식리스트 불러오기>>===========================================//
        //
        // 자동으로 로컬에 저장된 조건식 리스트 불러오기
        // OnReceiveConditionVer ==>> GetConditionNameList()
        //
        // =======================================================================================================================================//

        private void axKHOpenAPI_OnReceiveConditionVer(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveConditionVerEvent e)
        {

            if (e.lRet == 1)                                              // 조건식 저장이 성공이면
            {
                string strConList;

                strConList = axKHOpenAPI.GetConditionNameList().Trim();   // 조건식 리스트 호출하기    

                Logger(Log.실시간, strConList);

                // 분리된 문자 배열 저장
                string[] spConList = strConList.Split(';');               // 리스트가 ; 로 구분되어 수신되므로 잘라서 배열에 담는다

                System.Array.Sort(spConList);                             // 조건식 배열 오류로 추가 소스 삽입 

                // ComboBox 출력
                for (int i = 0; i < spConList.Length; i++)
                {
                    if (spConList[i].Trim().Length >= 2)
                    {
                        string[] spCon = spConList[i].Split('^');
                        int nIndex = Int32.Parse(spCon[0]);               // spCon[0]에는 000,001과 같이 인덱스값이 들어있음  
                        string strConditionName = spCon[1];               // spCon[1]에는 조건명이 들어있음      
                        cbo조건식.Items.Add(strConditionName);            // 매수조건식 콤보박스 출력
                        cbo조건식매도.Items.Add(strConditionName);        // 매도조건식 콤보박스 출력 (지금은 구분없이 같이 출력됨)

                    }
                    // Logger(Log.검색코드, "[이벤트] 조건식 탑재 성공 (건당 이벤트 발생)" );
                }
            }
            else
            {
                Logger(Log.실시간, "[이벤트] 조건식 저장 실패로 수동버튼으로 불러오세요 : " + e.sMsg);        // 에러처리.. 
            }
            cbo조건식.SelectedIndex = 0;                                                                  // 첫번째 조건식 콤보박스 장착
            cbo조건식매도.SelectedIndex = 12;                                                             // 매도 조건식 콤보박스 장착이므로 순서 지정 필요
        }

        //========================================================================
        // 자동으로 불러오기 실패시 수동으로 조건식 로컬 저장하기
        //========================================================================
        private void button10_Click(object sender, EventArgs e)
        {
            int lRet;

            lRet = axKHOpenAPI.GetConditionLoad();

            if (lRet == 1)
            {
                Logger(Log.실시간, "조건식 저장이 성공 되었습니다");
            }
            else
            {
                Logger(Log.실시간, "조건식 저장이 실패 하였습니다");
            }
        }

        // ======================================================================
        // 자동으로 불러오기 실패시 수동으로 로컬에 저장된 조건식 리스트 불러오기
        // ======================================================================

        private void button11_Click(object sender, EventArgs e)
        {
            string strConList;

            strConList = axKHOpenAPI.GetConditionNameList().Trim();

            Logger(Log.실시간, strConList);

            // 분리된 문자 배열 저장
            string[] spConList = strConList.Split(';');

            System.Array.Sort(spConList);                             // 조건식 배열 오류로 추가 소스 삽입 

            // ComboBox 출력
            for (int i = 0; i < spConList.Length; i++)
            {
                if (spConList[i].Trim().Length >= 2)
                {
                    string[] spCon = spConList[i].Split('^');
                    int nIndex = Int32.Parse(spCon[0]);
                    string strConditionName = spCon[1];
                    cbo조건식.Items.Add(strConditionName);           // 기존 키움샘플에서 수정된 소스  // cbo조건식.Items.Insert(nIndex, strConditionName);
                    cbo조건식매도.Items.Add(strConditionName);
                }
            }
            cbo조건식.SelectedIndex = 0;
            cbo조건식매도.SelectedIndex = 12;
        }

        // =================================================
        // 매수 조건식 실시간 조회기능
        // =================================================

        private void btn조건실시간조회_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();                 //모든행의 값을 지운다.

            int lRet;
            string strScrNum = GetScrNum();

            lRet = axKHOpenAPI.SendCondition(strScrNum,
                                              cbo조건식.Text,
                                              cbo조건식.SelectedIndex,
                                              1);       // 1번은 실시간 조회

            if (lRet == 1)
            {
                _strRealConScrNum = strScrNum;
                _strRealConName = cbo조건식.Text;
                _nIndex = cbo조건식.SelectedIndex;

                Logger(Log.실시간, "조건식 실시간 조회 실행이 성공 되었습니다");
            }
            else
            {
                Logger(Log.실시간, "조건식 실시간 조회 실행이 실패 하였습니다");
            }
        }

        // =================================================
        // 매도 조건식 실시간 조회기능
        // =================================================

        private void btn매도실시간조회_Click_1(object sender, EventArgs e)
        {
            // dataGridView1.Rows.Clear();                    //모든행의 값을 지운다.

            int lRet;
            string strScrNum = GetScrNum();

            lRet = axKHOpenAPI.SendCondition(strScrNum,
                                             cbo조건식매도.Text,
                                             cbo조건식매도.SelectedIndex,
                                             1);          // 1번은 실시간 조회
            if (lRet == 1)
            {
                _strRealConScrNum = strScrNum;
                _strRealConName = cbo조건식매도.Text;
                _nIndex = cbo조건식매도.SelectedIndex;

                Logger(Log.실시간, "매도조건식 실시간 조회 실행이 성공 되었습니다");
            }
            else
            {
                Logger(Log.실시간, "매도조건식 실시간 조회 실행이 실패 하였습니다");
            }
        }

        // =================================================<<조건조회 실시간 편입/이탈 정보 업데이트>>========================================//
        // * 자동주문 로직**
        // 조건조회 실시간 편입/이탈 정보 업데이트하여 데이터그리드뷰에 갱신하기
        //  
        // ====================================================================================================================================//

        private void axKHOpenAPI_OnReceiveRealCondition(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveRealConditionEvent e)
        {
            Logger(Log.검색코드, "======= 조건조회 실시간 편입/이탈 =======");
            Logger(Log.검색코드, "[종목코드] : " + e.sTrCode);
            Logger(Log.검색코드, "[실시간타입] : " + e.strType);
            Logger(Log.검색코드, "[조건명] : " + e.strConditionName);
            Logger(Log.검색코드, "[조건명 인덱스] : " + e.strConditionIndex);

            str실시간타입 = e.strType;
            str종목코드 = e.sTrCode;
            str종목명 = axKHOpenAPI.GetMasterCodeName(e.sTrCode);

            f편입체크(e.strType, str종목코드, str종목명);

            // =================================================<< 조건검색종목 자동 매수 >> ===================================================//
            //
            //                                                  실시간 편입종목 자동주문 로직
            //
            // =================================================================================================================================//

            // 실시간 편입종목 시장가 매수주문 로직
            if (_bRealTrade && e.strType == "I")
            {
                // 계좌번호 입력 여부 확인
                if (cbo계좌.Text.Length != 10)
                {
                    Logger(Log.검색코드, "계좌번호 10자리를 입력해 주세요");

                    return;
                }

                // 주식주문
                int lRet;

                lRet = axKHOpenAPI.SendOrder("주식주문",
                                            GetScrNum(),
                                            cbo계좌.Text.Trim(),
                                            1,      // 매매구분
                                            e.sTrCode.Trim(),   // 종목코드
                                            10,      // 주문수량
                                            1,      // 주문가격 
                                            "03",    // 거래구분 (시장가)
                                            "0");    // 원주문 번호

                if (lRet == 0)
                {
                    Logger(Log.검색코드, "편입종목 자동매수 주문이 전송 되었습니다");
                }
                else
                {
                    Logger(Log.검색코드, "편입종목 매수주문이 실패 하였습니다. [에러] : " + lRet);
                }

                f편입체크(e.strType, str종목코드, str종목명);
            }

                       // 실시간 이탈종목 시장가 매도주문 로직 ========== 사용 안함 참고자료!!
                        if (_bRealTrade && e.strType == "D")
                        {


                            // 계좌번호 입력 여부 확인
                            if (cbo계좌.Text.Length != 10)
                            {
                                Logger(Log.검색코드, "계좌번호 10자리를 입력해 주세요");

                                return;
                            }

                            // 주식주문
                            int lRet;

                            lRet = axKHOpenAPI.SendOrder("주식주문",
                                                        GetScrNum(),
                                                        cbo계좌.Text.Trim(),
                                                        2,      // 매매구분
                                                        e.sTrCode.Trim(),   // 종목코드
                                                        10,      // 주문수량
                                                        1,      // 주문가격 
                                                        "03",    // 거래구분 (시장가)
                                                        "0");    // 원주문 번호

                            if (lRet == 0)
                            {
                                Logger(Log.검색코드, "이탈종목 자동매도 주문이 전송 되었습니다");
                            }
                            else
                            {
                                Logger(Log.검색코드, "이탈종목 매도주문이 실패 하였습니다. [에러] : " + lRet);
                            }

                            f편입체크(e.strType, str종목코드, str종목명);
           
            //----------------------------------------------------
            // [실시간미체결요청]으로 매수 미체결  요청            =======================================================>>>>>>  작업중 <<<<<<<<<<<<<<<<<<<<<<<   
            //----------------------------------------------------
            // opt10075

            string 계좌번호 = cbo계좌.Text.Trim();

            // 매수미체결 요청
            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("체결구분", "1");                           // 체결구분 = 0:전체, 1:종목
            axKHOpenAPI.SetInputValue("매매구분", "1");                           // 매매구분 = 0:전체, 1:매도, 2:매수

            axKHOpenAPI.CommRqData("매수미체결", "opt10075", 0, GetScrNum());

            //매도미체결 요청
            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("체결구분", "1");                           // 체결구분 = 0:전체, 1:종목
            axKHOpenAPI.SetInputValue("매매구분", "2");                           // 매매구분 = 0:전체, 1:매도, 2:매수

            axKHOpenAPI.CommRqData("매도미체결", "opt10075", 0, GetScrNum());
        }
    }

        //지역변수 선언
        string str실시간타입;
        string str종목코드;
        string str종목명;
        int MAX편입종목;
        int i;

        public string[] tbl편입종목코드 = new string[20];
        public string[] tbl편입종목명 = new string[20];

        // =================================================
        // 실시간 편입종목을 업데이트
        // =================================================

        private void f편입체크(string str실시간타입, string str종목코드, string str종목명)
        {
            MAX편입종목 = 20;

            switch (str실시간타입)

            {
                case "I":

                    for (i = 0; i < MAX편입종목; i++)

                    {
                        if (str종목코드 == tbl편입종목코드[i]) return;           // 기존 등록 종목
                    }

                    for (i = 0; i < MAX편입종목; i++)

                    {
                        if (tbl편입종목코드[i] == "")
                        {                           
                            tbl편입종목코드[i] = str종목코드;                    // 추가 등록
                            tbl편입종목명[i] = str종목명;

                            this.dataGridView1.Rows.Add();

                            f종목편입update(i);
                            return;
                        }
                    }
                    break;                                                        // 등록테이블 Full

/*                case "D":                      // 이탈 종목은 메인화면에서 이미 매수가 되어있으므로 업데이트 필요없음 !!                                 

                    for (i = 0; i < MAX편입종목; i++)                             // MAX편입종목

                    {
                        if (str종목코드 == tbl편입종목코드[i])
                        {
                            tbl편입종목코드[i] = "" ;                             // 삭제완료
                            tbl편입종목명[i] = "" ;
                            //this.dataGridView1.Rows.RemoveAt(0);

                            f종목편입update(i);
                            return;
                        }
                    }
                   break;                                                        // 미 등록 종목
*/
                default:
                    return;
            }
        }

        // =================================================
        // 업데이트된 편입종목을 데이터그리드뷰에 갱신   =================================================  갱신 제대로 안됨 ---==============================
        // =================================================

        private void f종목편입update(int i)

        {
            if (tbl편입종목코드.Length > 1)
            {

                // Logger(Log.검색코드, " for i={0} /종목코드={1}>>", i, strCodeList[i]);   // 테스트용 로그 ********

                // dataGridView1.Rows.Remove(dataGridView1.Rows(0));     // 0번 Row 삭제 하고...
                //dataGridView1.Rows.Insert(0, a, b, c);          // 추가하기...
                //dataGridView1.Rows.Add(tbl편입종목코드[i]);
                //dataGridView1.Rows.Add(tbl편입종목명[i]);
                //dataGridView1.Rows.Add((i).ToString());
                
                dataGridView1.Rows[i].Cells[0].Value = tbl편입종목코드[i];
                dataGridView1.Rows[i].Cells[1].Value = tbl편입종목명[i];
    }
}

        // =====================================================<< 조건식 TR 메세지 수신부 >> ================================================//
        //      
        // 수신된 종목코드 문자열 분리
        // 최초 조건검색후 종목코드 수신하는곳으로 이후에는 OnReceiveRealCondition 에서 실시간 수신됨 
        // ===================================================================================================================================//

        private void axKHOpenAPI_OnReceiveTrCondition(object sender, AxKHOpenAPILib._DKHOpenAPIEvents_OnReceiveTrConditionEvent e)
        {
            //지역변수 선언
            int lRet;
            int cnt종목코드;
            string str종목리스트;
            str종목리스트 = e.strCodeList;
            int i;
            
            // 데이터그리드뷰에  매수 조건검색 종목코드 표시하기
            string[] strCodeList = e.strCodeList.Split(';');
            cnt종목코드 = strCodeList.Length - 1;

            // Logger(Log.검색코드, "[코드리스트 길이] : " + strCodeList.Length.ToString());

            if (strCodeList.Length > 1)
            {
                // 종목코드 갯수를 카운트한다
                dataGridView1.RowCount = strCodeList.Length - 1;             // 맨마지막에 공백이 오므로 -1 처리함 strCodeList.Length-1;

                // Logger(Log.검색코드, " i 카운트 갯수={0} ", dataGridView1.RowCount);         // 테스트용 로그 ********

                for (i = 0; i < strCodeList.Length - 1; i++)

                {
                    // Logger(Log.검색코드, " for i={0} /종목코드={1}>>", i, strCodeList[i]);   // 테스트용 로그 ********

                    //검색코드.Items.Insert(i, strCodeList[i]);

                    //Logger(Log.검색코드, "===== 조건식 조회  =====");           //  매수, 매도 조건명 구분확인용 로그
                    // Logger(Log.검색코드, "[화면번호] : " + e.sScrNo);
                    // Logger(Log.검색코드, "[종목리스트] : " + e.strCodeList);
                    // Logger(Log.검색코드, "[조건명] : " + e.strConditionName);
                    // Logger(Log.검색코드, "[조건명 인덱스] : " + e.nIndex);

                    
                    // ============================================<< 1. 조건검색종목 자동 매수,, 매도 구현부 >> =======================================//
                    //
                    //                                          1차 검색종목 즉시 매수주문 / 매도조건식 실행시 즉시 매도주문
                    //
                    // =================================================================================================================================//
                    string str매수조건식명;
                    string str매도조건식명;

                    str매수조건식명 = cbo조건식.Text;
                    str매도조건식명 = cbo조건식매도.Text;

                    //   매수조건 처리
                    if (e.strConditionName == str매수조건식명)
                    {
                        // 종목코드, 종목명, 현재가등  갯수대로  데이터그리드뷰에 출력한다
                        // 매수조식만 업데이트하고 매도식은 화면 표시가 필요없음
                        dataGridView1.Rows[i].Cells[0].Value = strCodeList[i];                                                   // 종목코드 출력
                        dataGridView1.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;           // 종목명 텍스트 왼쪽 정렬 사용 MiddleLeft
                        dataGridView1.Rows[i].Cells[1].Value = axKHOpenAPI.GetMasterCodeName(strCodeList[i]);                    // 종목명   출력 

                        //====================================================================================================
                        //<-- 한번에 전체 "관심종목정보" 를 호출 (100개 단위로 해야되서 100개 넘어가면 100개 단위로 분리 필요)
                        // ** 여기서 "관심종목정보"란 HTS의 관심종목이 아니고 복수의 종목을 가져올때 쓰는 TR명이다(?). 
                        //====================================================================================================
                        //관심종목정보
                        
                        // Logger(Log.검색코드, "관심종목정보 호출7>> {0} /CNT={1}", str종목리스트, cnt종목코드);        // 테스트용 로그 ********
                        if (cnt종목코드 > 100) cnt종목코드 = 100;
                        axKHOpenAPI.CommKwRqData(str종목리스트, 0, cnt종목코드, 0, "관심종목정보", GetScrNum());      // 관심종목정보 

                        if (_bRealTrade == true)                                    // if (_bRealTrade && e.strType == "I")
                        {
                            // 해당 종목 10주 시장가 주문
                            // =================================================

                            // 계좌번호 입력 여부 확인
                            if (cbo계좌.Text.Length != 10)
                            {
                                Logger(Log.검색코드, "계좌번호 10자리를 입력해 주세요");

                                return;
                            }

                            // =================================================
                            // 주식주문

                            lRet = axKHOpenAPI.SendOrder("주식주문",
                                                        GetScrNum(),
                                                        cbo계좌.Text.Trim(),
                                                        1,                        // 매매구분(1.신규매수 2. 신규매도)
                                                         strCodeList[i],          // 종목코드리스트를 이용해 순차적으로 주문
                                                        10,                      // 주문수량
                                                        0,                        // 주문가격 
                                                        "03",                     // 거래구분 (시장가)
                                                        "0");                     // 원주문 번호
                            System.Threading.Thread.Sleep(240);                  //  초당 5회 시간조절 

                            if (lRet == 0)
                            {
                                Logger(Log.검색코드, strCodeList[i] + "  : 자동 매수주문 전송!!");
                            }
                            else
                            {
                                Logger(Log.검색코드, strCodeList[i] + "  : 매수주문 전송실패!! [에러] : " + lRet);
                            }
                        }
                        //====================================================================================================
                        //<-- 한번에 전체 "관심종목정보" 를 호출 (100개 단위로 해야되서 100개 넘어가면 100개 단위로 분리 필요)
                        // ** 여기서 "관심종목정보"란 HTS의 관심종목이 아니고 복수의 종목을 가져올때 쓰는 TR명이다(?). 
                        //====================================================================================================
                        //관심종목정보

                        
                        // Logger(Log.검색코드, "관심종목정보 호출7>> {0} /CNT={1}", str종목리스트, cnt종목코드);                          // 테스트용 로그 ********

                        if (cnt종목코드 > 100) cnt종목코드 = 100;
                        axKHOpenAPI.CommKwRqData(str종목리스트, 0, cnt종목코드, 0, "관심종목정보", GetScrNum());                       // 관심종목정보 
                    }

                    //  매도조건 처리
                    else if (e.strConditionName == str매도조건식명)
                    {

                        if (_bRealTrade == true)                                  //if (_bRealTrade && e.strType == "I")

                        {
                            lRet = axKHOpenAPI.SendOrder("주식주문",
                                            GetScrNum(),
                                            cbo계좌.Text.Trim(),
                                            2,                        // 매매구분(1.신규매수 2. 신규매도)
                                            strCodeList[i],           // 종목코드리스트를 이용해 순차적으로 주문
                                            10,                       // 주문수량                                                       // 보유수량 파악하여 주문처리
                                            0,                        // 주문가격 
                                            "03",                     // 거래구분 (시장가)
                                            "0");                     // 원주문 번호

                            System.Threading.Thread.Sleep(260);                  //  초당 5회 시간조절

                            if (lRet == 0)
                            {
                                Logger(Log.검색코드, strCodeList[i] + "  : 자동 매도주문 전송!!");
                            }
                            else
                            {
                                Logger(Log.검색코드, strCodeList[i] + "  : 매도주문 전송실패!! [에러] : " + lRet);
                            }
                           
                        }

                    }
                }
            }
        }
 
        // ==================================================================
        // 실시간 조건검색식의 실시간 등록을 중단함
        // 실시간 조건검색식은 최대 10개까지만 사용가능합니다. 
        // ==================================================================

        private void btn_조건실시간중지_Click_1(object sender, EventArgs e)
        {

            //조건식 실시간등록 중단                                                        
            if (_strRealConScrNum != "0000" &&
                    _strRealConName != "0000")
            {
                axKHOpenAPI.SendConditionStop(_strRealConScrNum, _strRealConName, _nIndex);

                Logger(Log.실시간, "========= 실시간 조건 조회 중단 ==========");
                Logger(Log.실시간, "[화면번호] : " + _strRealConScrNum + " [조건명] : " + _strRealConName);
            }
        }
                
        // 데이터그리드뷰를 초기화 시킨다
        private void button12_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();     //모든행의 값을 지운다.
            dataGridView2.Rows.Clear();
        }

        // 사용된 화면번호를 모두 초기화 시킨다
        private void btn화면번호초기화_Click(object sender, EventArgs e)
        {
            axKHOpenAPI.SetRealRemove("ALL", "ALL");

            Logger(Log.실시간, "========= 전체 화면번호 초기화 ==========");
            Logger(Log.실시간, "[화면번호] : " + " [종목코드명] : " + "");
        }

        //----------------------------------------------------------------------------------------------------------------
        // [계좌평가잔고내역] 요청으로 보유종목 갱신은 보유종목수가 20개가 넘으면 1차 조회후 연속 조회 요청부를 구현해야함 
        // 비실시간이므로 2번 OnReceiveTrData로 가서 코딩
        // 계좌평가잔고내역으로 메인화면 매수종목의 보유수량,매입가,매입금액,수익률을 구현하면 된다.
        //----------------------------------------------------------------------------------------------------------------
        //opw00018
        private void button2_Click(object sender, EventArgs e)
        {
            string 계좌번호 = cbo계좌.Text.Trim();


            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("비밀번호", "");
            axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "");
            axKHOpenAPI.SetInputValue("조회구분", "1");                                  // 조회구분 = 1:합산, 2:개별               
            axKHOpenAPI.CommRqData("계좌평가잔고내역", "opw00018", 0, GetScrNum());      // 조회구분 = 0: 조회ㅣ, 2:연속 
                                                                                         // 1차 조회후 연속데이터가 있을때 다시 2번을 요청하여 자료가 없을때까지 반복
                                                                                         // 처음부터 연속조회요청 불가(타인계좌 사용할 수 없다 ~ 오류남)
        }

        //-----------------------------------------------------------
        // 20종목이상을 조회하려면 "계좌평가잔고내역" 연속조회를 구현
        //-----------------------------------------------------------

        public int GetMyAccountState(int prevNext)

        {
            string 계좌번호 = cbo계좌.Text.Trim();
            int rtnValue = 0;
            int nRet = 0;

            if (prevNext == 0)

            {
                axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
                axKHOpenAPI.SetInputValue("비밀번호", "");
                axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
                axKHOpenAPI.SetInputValue("조회구분", "1");
                nRet = axKHOpenAPI.CommRqData("계좌평가잔고내역", "opw00018", 0, GetScrNum());
                Logger(Log.검색코드, "=================================================");
            }
            else
            {
                axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
                axKHOpenAPI.SetInputValue("비밀번호", "");
                axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
                axKHOpenAPI.SetInputValue("조회구분", "1");
                nRet = axKHOpenAPI.CommRqData("계좌평가잔고내역", "opw00018", 2, GetScrNum());
            }
            if (Error.IsError(nRet))
            {
                Logger(Log.검색코드, "[opw00018] : " + Error.GetErrorMessage() + GetScrNum());
                rtnValue = -1;
            }
            else
            {
                Logger(Log.실시간, "[opw00018] : " + Error.GetErrorMessage() + GetScrNum());
                rtnValue = -1;
            }
            return rtnValue;
        }

        //----------------------------------------------
        // [예수금상세현황요청]으로 계좌현황 갱신하기
        // 비실시간이므로 2번 OnReceiveTrData로 가서 코딩
        //----------------------------------------------
        //opw00001

        private void button1_Click(object sender, EventArgs e)
        {

            string 계좌번호 = cbo계좌.Text.Trim();


            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("비밀번호", "");
            axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
            axKHOpenAPI.SetInputValue("조회구분", "1");                                  // 조회구분 = 1:추정조회, 2:일반조회               
            axKHOpenAPI.CommRqData("예수금상세현황요청", "opw00001", 0, GetScrNum());
        }

        //----------------------------------------------
        // [일자별실현손익요청]으로 계좌현황 갱신하기
        // 비실시간이므로 2번 OnReceiveTrData로 가서 코딩
        //----------------------------------------------
        //opt10074

        private void button13_Click(object sender, EventArgs e)

        {
            string 계좌번호 = cbo계좌.Text.Trim();
            string today = DateTime.Now.ToString("yyyyMMdd"); //오늘날짜

            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("시작일자", today);                                     // 당일날짜 
            axKHOpenAPI.SetInputValue("종료일자", today);                                     // 당일날짜 
            axKHOpenAPI.CommRqData("일자별실현손익요청", "opt10074", 0, GetScrNum());
        }
                
        //----------------------------------------------------
        // [계좌평가현황요청]으로 유가잔고평가액 요청               
        //----------------------------------------------------
        // OPW00004

        private void button15_Click(object sender, EventArgs e)
        {
            string 계좌번호 = cbo계좌.Text.Trim();

            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("비밀번호", "");
            axKHOpenAPI.SetInputValue("상장폐지조회구분", "0");               //상장폐지조회구분 = 0:전체, 1:상장폐지종목제외
            axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
            axKHOpenAPI.CommRqData("계좌평가현황요청", "OPW00004", 0, GetScrNum());
        }

        //-----------------------------------------------------------------------------------------------------------------------------------------------
        // [전업종지수요청]으로 코스피지수 요청   
        // 업종코드 = 001:종합(KOSPI), 002:대형주, 003:중형주, 004:소형주 101:종합(KOSDAQ), 201:KOSPI200, 302:KOSTAR, 701: KRX100 나머지 ※ 업종코드 참고            
        //-----------------------------------------------------------------------------------------------------------------------------------------------
        // opt20003

        private void button16_Click(object sender, EventArgs e)
        {
            axKHOpenAPI.SetInputValue("업종코드", "001");
            axKHOpenAPI.CommRqData("전업종지수요청", "opt20003", 0, GetScrNum());
        }

        //-------------------------------------------------------------------------------------------------------------------------------
        // [opw00018]을 이용하여 보유주식현황 요청으로 보유종목 갱신(보유종목수가 20개가 넘으면 1차 조회후 연속 조회 요청부를 구현해야함) 
        // 비실시간이므로 2번 OnReceiveTrData로 가서 코딩
        // 메인화면의 매수종목= 보유수량,매입가,매입금액,수익률을 구현하면 된다.
        //-------------------------------------------------------------------------------------------------------------------------------
        //opw00018
        private void button17_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();     //모든행의 값을 지운다.

            string 계좌번호 = cbo계좌.Text.Trim();

            axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
            axKHOpenAPI.SetInputValue("비밀번호", "");
            axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "");
            axKHOpenAPI.SetInputValue("조회구분", "1");                              // 조회구분 = 1:합산, 2:개별               
            axKHOpenAPI.CommRqData("보유주식현황", "opw00018", 0, GetScrNum());      // 조회구분 = 0: 조회ㅣ, 2:연속 
                                                                                     // 1차 조회후 연속데이터가 있을때 다시 2번을 요청하여 자료가 없을때까지 반복
                                                                                     // 처음부터 연속조회요청 불가(타인계좌 사용할 수 없다 ~ 오류남)
        }

        //-----------------------------------------------------------
        // 20종목이상을 조회하려면 "보유주식현황" 연속조회를 구현
        //-----------------------------------------------------------

        public int MyAccountState(int prevNext)

        {
            string 계좌번호 = cbo계좌.Text.Trim();
            int rtnValue = 0;
            int nRet = 0;

            if (prevNext == 0)
            {
                axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
                axKHOpenAPI.SetInputValue("비밀번호", "");
                axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
                axKHOpenAPI.SetInputValue("조회구분", "1");
                nRet = axKHOpenAPI.CommRqData("보유주식현황", "opw00018", 0, GetScrNum());
                Logger(Log.검색코드, "=================================================");
            }
            else
            {
                axKHOpenAPI.SetInputValue("계좌번호", 계좌번호);
                axKHOpenAPI.SetInputValue("비밀번호", "");
                axKHOpenAPI.SetInputValue("비밀번호입력매체구분", "00");
                axKHOpenAPI.SetInputValue("조회구분", "1");
                nRet = axKHOpenAPI.CommRqData("보유주식현황", "opw00018", 2, GetScrNum());
            }
            if (Error.IsError(nRet))
            {
                Logger(Log.검색코드, "[opw00018] : " + Error.GetErrorMessage() + GetScrNum());
                rtnValue = -1;
            }
            else
            {
                Logger(Log.검색코드, "[opw00018] : " + Error.GetErrorMessage() + GetScrNum());
                rtnValue = -1;
            }
            return rtnValue;
        }
        
        // =====================================================<<자동매매 시작하기 / 중단하기   >>===========================================//
        //
        //                                      
        // ===================================================================================================================================//

        private void btn자동주문_Click_1(object sender, EventArgs e)
        {
            DateTime date1 = DateTime.Now;                               // 현재시간과 data2에 정해진 시간을 비교하여 자동매매 시작 및 종료 시간 구현중
            DateTime date2 = new DateTime( 2016,05,08, 09, 00, 00);       // 설정값 불러와서 구현예정 

            int result = DateTime.Compare(date1, date2);
            string relationship;

            if (result < 0)
                relationship = "자동매매 시작전입니다. ";
            else if (result == 0)
                relationship = "자동매매를 시작합니다.";
            else
                relationship = "자동매매 가동시간으로 작동개시";

            Logger(Log.실시간, "{0} {1} {2}", date1, relationship, date2);    // 시간 비교 로그 출력

           // if (DateTime.Compare(09,10) >= 0 && DateTime.Compare(09,45);

            if (_bRealTrade)
            {
                btn자동주문.Text = "시스템 트레이딩 가동";
                _bRealTrade = false;
                Logger(Log.검색코드, "======= 자동 주문 중단 ========");
            }
            else
            {
                btn자동주문.Text = "시스템 트레이딩 중단";
                _bRealTrade = true;
                Logger(Log.검색코드, "======= 자동 주문 실행 ========");
            }
        }
        
        // 상태바에 현재날짜시간 표시하기 
        private void timer4_Tick(object sender, EventArgs e)
        {
                         
                lbl현재시간.Text = DateTime.Now.Year.ToString() + "년 " + DateTime.Now.Month.ToString().PadLeft(2, '0')
                 + "월" + DateTime.Now.Day.ToString().PadLeft(2, '0') + "일 " + DateTime.Now.Hour.ToString().PadLeft(2, '0')
                 + "시" + DateTime.Now.Minute.ToString().PadLeft(2, '0') + "분" + DateTime.Now.Second.ToString().PadLeft(2, '0') + "초";
        }

        
    }
}

// ===========================================================================================================================================
// *******************************************************************************************************************************************
// ****************************************************  키움 자동매매 프로그램 학습 완료 ****************************************************
// *******************************************************************************************************************************************
// ===========================================================================================================================================

