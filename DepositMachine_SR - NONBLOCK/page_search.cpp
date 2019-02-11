#include "page_search.h"
#include "ui_page_search.h"
#include "global.h"

page_search::page_search(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_search)
{
    ui->setupUi(this);
}

void page_search::on_btnTimeList_clicked(){
    // DB에서 예식시간 정보를 검색하여 표시 한 후 페이지 전환

    qDebug() << "time List button pressed";
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    //초기화 작업 시에 저장된 컨트롤러의 메모리 주소 좌표를 얻기
    int Select_X;
    for(int i=0; i<btnTimeList.count(); i++){
        if(button->objectName() == btnTimeList.at(i)->objectName()){
            Select_X = i;               //editValue의 x축 분류를 저장하기 위함
            break;
        }
    }
    switch(Select_X){
    case 0:     // escrow 모드
    case 1:     // bulk 모드
    case 2:     // other 모드
    case 3:     // 국가1
    case 4:     // 국가2
    case 5:     // 국가3
    case 6:     // 국가4
    case 7:     // 국가5
        break;
    }
    ui->stackedWidget->setCurrentIndex(4);
}

void page_search::on_btnHoleList_clicked(){
    // DB에서 예식홀 정보를 검색하여 표시 한 후 페이지 전환

    qDebug() << "hole List button pressed";
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    //초기화 작업 시에 저장된 컨트롤러의 메모리 주소 좌표를 얻기
    int Select_X;
    for(int i=0; i<btnHoleList.count(); i++){
        if(button->objectName() == btnHoleList.at(i)->objectName()){
            Select_X = i;               //editValue의 x축 분류를 저장하기 위함
            break;
        }
    }
    switch(Select_X){
    case 0:     // escrow 모드
    case 1:     // bulk 모드
    case 2:     // other 모드
    case 3:     // 국가1
    case 4:     // 국가2
    case 5:     // 국가3
    case 6:     // 국가4
    case 7:     // 국가5
        break;
    }
    ui->stackedWidget->setCurrentIndex(4);
}

page_search::~page_search()
{
    delete ui;
}


void page_search::on_btn_Back_clicked()
{
    int index = ui->stackedWidget->currentIndex();
    switch(index){
    case 0:         // 검색 메뉴
        p_Main->select_Page(EVENTVIEWPAGE);
        break;
    case 1:         // 이름 검색
    case 2:         // 예식 시간 검색
    case 3:         // 홀 검색
    case 4:         // 검색 결과
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 5:         // 선택 정보
        switch(p_Main->pLogin->assignInfo){
        case 1:
        case 2:
            // 지문인증에서 바로 결과 메뉴로s
            p_Main->select_Page(EVENTVIEWPAGE);
            break;
        case 3:
            ui->stackedWidget->setCurrentIndex(4);
            break;
        }
        break;
    }
}

void page_search::init_searchPage(){
    switch(p_Main->pLogin->assignInfo){
    case 1:
    case 2:
        break;
    case 3:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 4:                                         // 축의금납부
        break;
    case 5:                                         // 축의금 납부 후 식권출력
        break;
    }
}

void page_search::lang_change()
{
    ui->retranslateUi(this);
}

void page_search::on_btn_nameSearch_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void page_search::on_btn_timeSearch_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void page_search::on_btn_holeSearch_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void page_search::on_edit_searchname_selectionChanged()
{
//    key_pad->open_relativePad(ui->edit_searchname,this);           //에디트 컨트롤의 정보를 가지고 text 설정 함수 호출
}

void page_search::on_btn_nameSearchClick_clicked()
{
    // 검색 후 결과에 따른 처리 추가
    ui->stackedWidget->setCurrentIndex(4);
}

void page_search::on_btn_selectWedding_clicked()
{
    // 선택된 정보를 결과창에 표시하고 스택전환
    ui->stackedWidget->setCurrentIndex(5);
}


// 스택 변경시 마다 초기화 할 것들
void page_search::on_stackedWidget_currentChanged(int arg1)
{
}

void page_search::on_btn_deposit_clicked()
{
    if(p_Main->curEventStr->totalCash.toInt() <= 0){
        cmd_RequestStop();
        // later 정산완료된거 처리해야됨
        return;
    }

    QString tmpStr;
    p_Main->pLoading->init_loadingPage(1);
//    tmpStr.append("38");
    tmpStr.append(DEFAULTCOUNTRY);                    // later 현재 국가 화폐로
//    setDepositCountry(tmpStr.toUtf8().data());                                                                                               // SDK - setCountry
    p_Main->select_Page(LOADINGPAGE);

    //later 계약금액 구분입금 모드 설정

    int tmpCount = 0;
    if(p_Main->curEventStr->balanceCash.toInt() >= 50000){
        tmpCount = p_Main->curEventStr->balanceCash.toInt()/50000;
        tmpStr.append("01");                                 // 카운트모드 설정 0=count 1=mixed 2=single
        tmpStr.append("1E");                                // 입금단위설정 = 모든단위
    }
    else{
        tmpCount = p_Main->curEventStr->balanceCash.toInt()/10000;
        tmpStr.append("02");                                 // 카운트모드 설정 0=count 1=mixed 2=single
        tmpStr.append("02");                                // 입금단위설정 = 만원단위
    }
    QString tmpInt = QString::number(tmpCount,16);
    if(tmpInt.count() < 2){
        tmpStr.append("0");
    }
    tmpStr.append(tmpInt);         // 계수 카운트 수량 설정

    setDepositCount(tmpStr.toUtf8().data());

    qDebug() << "set count info" << tmpCount << tmpStr;
}

void page_search::on_btn_Back_2_clicked()
{
    p_Main->select_Page(EVENTVIEWPAGE);
}

void page_search::on_btn_deposit_2_clicked()
{
    QString tmpStr;
    p_Main->pLoading->init_loadingPage(1);
//    tmpStr.append("38");
    tmpStr.append(p_Main->currentCountry);
    setDepositCountry(tmpStr.toUtf8().data());                                                                                               // SDK - setCountry
    p_Main->select_Page(LOADINGPAGE);
    ui->stackedWidget->setCurrentIndex(7);
}

void page_search::on_btn_couponComp_clicked()
{
    qDebug() << "Coupon Print Clicked";
    qDebug() << "Coupon Print Count: " << p_Main->couponCount;


    QString values;
    values.append(QString("'%1',").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
    values.append(QString("'%1',").arg(p_Main->deviceNum));
    values.append(QString("'%1',").arg(p_Main->depositNum));
    values.append("'축의금입금',");
    values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumTotal));
    values.append(QString("'%1',").arg(p_Main->curEventStr->primaryid));
    values.append(QString("'%1',").arg(p_Main->couponCount));
    values.append("'0',");                                                         // process time 0 으로저장
    values.append("'-'");                                                          // Sp column

    p_Main->currentTransaction = values;


    if(p_Main->couponCount > 0){
        p_Main->printCount = 0;
        qDebug() << "Coupon Print Count: " << p_Main->couponCount << p_Main->printCount;
        p_Main->pResult->receipt_Setup(2,p_Main->printCount);       // 비용정산 영수증
        p_Main->pLoading->init_loadingPage(20);      // 식권 출력중 이미지 로딩
        p_Main->select_Page(LOADINGPAGE);
    }
    else{
    }
}

void page_search::on_btn_countUp_clicked()
{
}

void page_search::on_btn_countDown_clicked()
{
}

void page_search::setCurrentStackPage(int page){
    ui->stackedWidget->setCurrentIndex(page);
}
