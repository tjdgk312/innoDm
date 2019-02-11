#include "messagedialog.h"
#include "ui_messagedialog.h"
#include "global.h"

MessageDialog::MessageDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    currentFlag = 0;
    select = 0;
//    this->setWindowModality(Qt::NonModal);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

int MessageDialog::init_MessageDialog(int msgFlag){

//    if (! this-> isActiveWindow ()) {
//    this-> setWindowFlags ((windowFlags () & Qt :: WindowStaysOnTopHint));
//    this-> setWindowFlags ((windowFlags () & ~ Qt :: WindowStaysOnTopHint));
//    this-> show ();}
//    ui->lbl_flag->setText(QString("%1").arg(chk_fingerState()));
    this->setParent(p_Main);

    currentFlag = msgFlag;
    ui->btn_yes->show();
    ui->btn_no->show();
    ui->btn_yes->setText(trUtf8("예"));
    ui->btn_no->setText(trUtf8("아니오"));
    ui->pushButton->hide();
    ui->lineEdit->hide();
    ui->pushButton->setEnabled(true);

    int execFlag = 0;

    switch(currentFlag){
    case 0:
        ui->lbl_infoText->setText(trUtf8("지문인식에 실패하였습니다.\n재시도 하시겠습니까?"));
        break;
    case 1:
        ui->lbl_infoText->setText(trUtf8("바코드 인식에 실패하였습니다.\n재시도 하시겠습니까?"));
        break;
    case 2:
        ui->lbl_infoText->setText(trUtf8("통신 연결이 해제되어 동작이 불가합니다. \n 관리자에게 문의해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 3:
        ui->lbl_infoText->setText(trUtf8("기기정보 저장에 실패하였습니다"));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 4:
        ui->lbl_infoText->setText(trUtf8("기기초기화 작업중입니다. \n 잠시만 기다려주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 5:
        ui->pushButton->show();
        ui->lineEdit->show();
        ui->lineEdit->setText("");
        ui->lbl_infoText->setText(trUtf8("현송 코드를 입력해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->setText(trUtf8("취소"));
        break;
    case 6:
        ui->pushButton->show();
        ui->lineEdit->show();
        ui->lineEdit->setText("");
        ui->lbl_infoText->setText(trUtf8("ID를 입력해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->setText(trUtf8("취소"));
        break;
    case 7:
        ui->pushButton->show();
        ui->lineEdit->show();
        ui->lineEdit->setText("");
        ui->lbl_infoText->setText(trUtf8("관리자 모드진입\n비밀번호 & 카드인증을 진행해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->setText(trUtf8("취소"));

        p_Main->cardOpType = 99;
        p_Main->cardRetryCount = -1;
        p_Main->pLoading->timingCount = 6;
        p_Main->assignRfcard = 1;
        break;
    case 8:
        if(getFingerTemplate(p_Main->templateBuf)){
            ui->lbl_infoText->setText(trUtf8("지문 인증 요청중입니다...."));
            ui->btn_no->setText(trUtf8("취소"));
            ui->btn_yes->hide();
            p_Main->assignFlag = 0;     // 인증 플래그 0으로
            p_Main->eventFlag = 3;                      // Request DB data
            p_Main->pLoading->init_loadingPage(23);                 // 로딩 페이지 타이머 대기 스탭 진입
        }
        else{
            ui->pushButton->show();
            ui->lineEdit->show();
            ui->lineEdit->setText("");
            ui->lbl_infoText->setText(trUtf8("지문템플릿 가져오기에 실패하였습니다. \n ID를 입력해주세요."));
            ui->btn_yes->setText(trUtf8("확인"));
            ui->btn_no->setText(trUtf8("취소"));
            currentFlag = 6;
        }
        break;
    case 9:  
        this->setParent(p_Main->pRepair);
        ui->pushButton->show();
        ui->lineEdit->show();
        ui->lineEdit->setText("");
        ui->lbl_infoText->setText(trUtf8("관리자 모드진입\n비밀번호 & 카드인증을 진행해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->setText(trUtf8("취소"));

        p_Main->cardOpType = 98;
        p_Main->cardRetryCount = -1;
        p_Main->pLoading->timingCount = 6;
        p_Main->assignRfcard = 1;
        break;
    case 10:
        ui->lbl_infoText->setText(trUtf8("지문등록에 성공하였습니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 11:
        ui->lbl_infoText->setText(trUtf8("지문인식기가 연결되어 있지 않습니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 12:
        ui->lbl_infoText->setText(trUtf8("카드인증에 실패하였습니다.\n재시도 하시겠습니까?"));
        break;
    case 13:
        execFlag = 1;
        ui->lbl_infoText->setText(trUtf8("출력 용지가 부족합니다.\n계속해서 진행 하시겠습니까?"));
        break;
    case 14:
        ui->lbl_infoText->setText(trUtf8("게이트 닫기에 실패하였습니다.\n재시도 하시겠습니까?"));
        break;
    case 15:
        ui->lbl_infoText->setText(trUtf8("인증 시간을 초과하였습니다.\n재시도 하시겠습니까?"));
        break;
    case 16:
        ui->lbl_infoText->setText(trUtf8("백 제한 수량을 초과하여 기기 동작이 불가합니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 17:
        if(p_Main->pLogin->assignInfo == 1){
            ui->lbl_infoText->setText(trUtf8("지문 인증시간 초과되었습니다.\n서버 모듈 상태를 확인해주세요."));
        }
        else{
            ui->lbl_infoText->setText(trUtf8("인증시간이 초과되었습니다.\n통신 상태를 확인해주세요."));
        }
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 18:
        ui->pushButton->show();
        ui->lineEdit->show();
        ui->lineEdit->setText("");
        ui->lbl_infoText->setText(trUtf8("엔지니어 모드진입\n비밀번호 & 카드인증을 진행해주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->setText(trUtf8("취소"));

        p_Main->cardOpType = 999;
        p_Main->cardRetryCount = -1;
        p_Main->pLoading->timingCount = 6;
        p_Main->assignRfcard = 1;
        break;
    case 19:
        ui->lbl_infoText->setText(trUtf8("출력 용지가 부족합니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 20:
        ui->lbl_infoText->setText(trUtf8("기기 상태를 체크합니다\n잠시만 기다려주세요."));
        ui->btn_yes->hide();
        ui->btn_no->hide();
        break;
    case 21:
        ui->lbl_infoText->setText(trUtf8("출력 장치가 로딩되지 않았습니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 22:
        ui->lbl_infoText->setText(trUtf8("지폐 투입구를 확인해 주세요."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 23:
        ui->lbl_infoText->setText(trUtf8("시스템 전원이 불안정 하여 동작을 중지합니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    case 24:
        ui->lbl_infoText->setText(trUtf8("추가입금은 취소가 불가능 합니다."
                                         "\n진행 하시겠습니까?"));
        execFlag = 1;
        break;
    case 25:
        ui->lbl_infoText->setText(trUtf8("시스템 데이터 로딩 중 입니다."));
        ui->btn_yes->setText(trUtf8("확인"));
        ui->btn_no->hide();
        break;
    }

    if(execFlag == 1){
        this->exec();
    }
    else{
        this->show();
    }

    return select;
//    this->setParent(p_Main);
//    this->setModal(false);
//    this->raise();
//    this->exec();
}

void MessageDialog::on_btn_yes_clicked()
{
    qDebug() << "dialog yes clicked";
    switch(currentFlag){
    case 0:
        if(p_Main->finCmdType == 1){        // 인증 로그인
            cmd_fingerIdenStart();
        }
        else if(p_Main->finCmdType == 2){   // 지문정보스캔
            cmd_fingerScanStart();
        }
        else{                               // 3 = 지문등록
            cmd_fingerEnrollStart();
        }
        p_Main->select_Page(LOADINGPAGE);
        p_Main->pLoading->init_loadingPage(12);
        p_Main->setEnabled(true);
        this->close();
        break;
    case 5:
        if(ui->lineEdit->text()== ""){
            ui->lbl_infoText->setText(trUtf8("현송코드를 입력해 주세요...."));
            break;
        }
        p_Main->curTransCode = ui->lineEdit->text();
    //    ui->edit_PW->clear();
        ui->pushButton->setEnabled(false);
        ui->lbl_infoText->setText(trUtf8("현송 인증 요청중입니다...."));
        p_Main->assignFlag = 0;     // 인증 플래그 0으로
        p_Main->eventFlag = 2;                      // Request DB data
        p_Main->pLoading->init_loadingPage(23);                 // 로딩 페이지 타이머 대기 스탭 진입
        break;
    case 6:
        if(ui->lineEdit->text()== ""){
            ui->lbl_infoText->setText(trUtf8("ID를 입력해 주세요...."));
            break;
        }
        p_Main->curUserId = ui->lineEdit->text();
        if(p_Main->curUserId== ""){
            p_Main->curUserId = "nullUser";
        }
    //    ui->edit_PW->clear();
        ui->pushButton->setEnabled(false);
        ui->lbl_infoText->setText(trUtf8("로그인 인증 요청중입니다...."));
        p_Main->assignFlag = 0;     // 인증 플래그 0으로
        p_Main->eventFlag = 1;                      // Request DB data
        p_Main->pLoading->init_loadingPage(23);                 // 로딩 페이지 타이머 대기 스탭 진입
        break;
    case 7:
    {
        QString enCode = QString("inn%1%2%3").arg(QDate::currentDate().toString("yy"))
                .arg(QDateTime::currentDateTime().toString("mm")).arg(QDate::currentDate().dayOfWeek());
        qDebug() << "manager Password: " << enCode;
        if(ui->lineEdit->text() == p_Main->managerPW || ui->lineEdit->text() == enCode){
            p_Main->select_Page(MANAGEPAGE);
            p_Main->pLoading->current_step = IDLE_DEPOSIT;
            p_Main->assignRfcard = 0;
            p_Main->cardRetryCount = 0;
            this->hide();
        }
        else{
            ui->lbl_infoText->setText(trUtf8("관리자 비밀번호를 다시 확인해 주세요."));
        }
        break;
    }
    case 9:
    {
        QString enCode = QString("inn%1%2%3").arg(QDate::currentDate().toString("yy"))
                .arg(QDateTime::currentDateTime().toString("mm")).arg(QDate::currentDate().dayOfWeek());
        qDebug() << "manager Password: " << enCode;
        if(ui->lineEdit->text() == p_Main->managerPW || ui->lineEdit->text() == enCode){
            p_Main->assignRfcard = 0;
            p_Main->cardRetryCount = 0;
            p_Main->pRepair->set_ManageFrame();
            this->hide();
        }
        else{
            ui->lbl_infoText->setText(trUtf8("관리자 비밀번호를 다시 확인해 주세요."));
        }
        break;
    }
    case 12:
        p_Main->cardOpType = 0;
        p_Main->pSearch->setCurrentStackPage(1);
        p_Main->select_Page(SEARCHPAGE);
        p_Main->pLoading->init_loadingPage(27);         // 로딩스텝만 전환하고 화면은 서치페이지에서 표시
        p_Main->setEnabled(true);
        this->close();
        break;
    case 14:
        this->close();
        break;
    case 15:
        this->close();
        if(p_Main->pLogin->assignInfo == 1){
            p_Main->pLogin->on_btn_fingerAssign_clicked();
        }
        else if(p_Main->pLogin->assignInfo == 2){
            p_Main->pLogin->on_btn_IDAssign_clicked();
        }
        else if(p_Main->pLogin->assignInfo == 3){
            p_Main->pLogin->on_btn_CardAssign_clicked();
        }
        break;
    case 17:
        p_Main->select_Page(LOGINPAGE);
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        this->close();
        break;
    case 18:
    {
        // inn + year 2 letter ex)18 + minute 2 letter + dayof week 1 letter monday is 1 sunday is 7
        //    ex) current datetime is 2018-06-04 16:32:25 monday ------> pw is  inn1821
        QString enCode = QString("inn%1%2%3").arg(QDate::currentDate().toString("yy"))
                .arg(QDateTime::currentDateTime().toString("mm")).arg(QDate::currentDate().dayOfWeek());
        qDebug() << "manager Password: " << enCode;
        if(ui->lineEdit->text() == enCode){
            p_Main->select_Page(TESTPAGE);
            p_Main->pLoading->current_step = IDLE_DEPOSIT;
            p_Main->assignRfcard = 0;
            p_Main->cardRetryCount = 0;
            this->hide();
        }
        else{
            ui->lbl_infoText->setText(trUtf8("엔지니어 코드를 다시 확인해 주세요."));
        }
        break;
    }
    default:
        p_Main->setEnabled(true);
        this->close();
        break;
    }
    select = 1;
}

void MessageDialog::on_btn_no_clicked()
{
    p_Main->setEnabled(true);
    select = 0;
    switch(currentFlag){
    case 0:
        p_Main->pLogin->assignInfo = 0;
        p_Main->select_Page(LOGINPAGE);
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        break;
    case 2:
        p_Main->pLogin->assignInfo = 0;
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        break;
    case 5:
    case 6:
        p_Main->eventFlag = 0;
        break;
    case 7:
        p_Main->assignRfcard = 0;
        p_Main->cardRetryCount = 0;
        break;
    case 8:
        p_Main->eventFlag = 0;
        p_Main->pLogin->assignInfo = 0;
        p_Main->select_Page(LOGINPAGE);
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        break;
    case 9:
        p_Main->assignRfcard = 0;
        p_Main->cardRetryCount = 0;
        break;
    case 12:
        p_Main->eventFlag = 0;
        p_Main->pLogin->assignInfo = 0;
        p_Main->select_Page(LOGINPAGE);
        p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        break;
    case 18:
        p_Main->assignRfcard = 0;
        p_Main->cardRetryCount = 0;
        break;
    }
    this->hide();
}

void MessageDialog::setAssignResult(){
    QString resultStr;
    ui->pushButton->setEnabled(true);
    switch(p_Main->assignFlag){
    case 1:
        qDebug() << "success";
//        ui->edit_ID->clear();
        this->hide();
        switch(currentFlag){
        case 5:
        {
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/c2.mp3").arg(PATH_ROOT)),1,15,true);
            cmd_SettlementStart();
            p_Main->pLoading->init_loadingPage(5);
            p_Main->select_Page(LOADINGPAGE);
            // 현송시작
            int idx=0;
            QSqlQuery query;
            bool ret = query.exec(QString("SELECT sum(totalAmount) FROM %1").arg(RECORD_TABLE));
            while (query.next())
            {
                if(query.value(idx).isValid()){   // 검색 열 갯수검사
                    qDebug() << "settle return1: " << query.value(idx);
                    p_Main->curBagAmount = query.value(idx).toString();
                }
            }
            qDebug() << "settle return: " << ret << p_Main->curBagAmount;
            ret = query.exec(QString("SELECT sum(totalCount) FROM %1").arg(RECORD_TABLE));
            while (query.next())
            {
                if(query.value(idx).isValid()){   // 검색 열 갯수검사
                    qDebug() << "settle return2: " << query.value(idx);
                    p_Main->curBagCount = query.value(idx).toString();
                }
            }
            qDebug() << "settle return: " << ret << p_Main->curBagCount;
            break;
        }
        case 6:
            p_Main->select_Page(MENUPAGE);
            break;
        case 8:
            p_Main->select_Page(MENUPAGE);
            p_Main->pLoading->preState_step = p_Main->pLoading->current_step;
            p_Main->pLoading->current_step = IDLE_DEPOSIT;
            break;
        case 12:
            p_Main->select_Page(MENUPAGE);
            break;
        }
        break;
    case 2:
        qDebug() << "failed";

        switch(currentFlag){
        case 12:
            init_MessageDialog(12);
            break;
        case 5:
            resultStr = trUtf8("ID 또는 코드를 다시 확인해 주세요.");
            ui->lbl_infoText->setText(resultStr);
            break;
        case 6:
            resultStr = trUtf8("ID 또는 코드를 다시 확인해 주세요.");
            ui->lbl_infoText->setText(resultStr);
            break;
        case 8:
            this->hide();
            init_MessageDialog(0);
            break;
        }
        break;
    }
}

void MessageDialog::lang_change()
{
    ui->retranslateUi(this);
}

void MessageDialog::on_edit_input_selectionChanged()
{
}

void MessageDialog::on_pushButton_clicked()
{
    key_pad->open_relativePad(ui->lineEdit,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void MessageDialog::setInfoText(QString text)
{
    ui->lbl_infoText->setText(text);
}

