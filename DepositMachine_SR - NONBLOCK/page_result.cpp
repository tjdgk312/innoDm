#include "page_result.h"
#include "ui_page_result.h"
#include "global.h"


page_result::page_result(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_result)
{
    ui->setupUi(this);

    set_resultButton(true);

    result_timer = new QTimer(this);
    connect(result_timer , SIGNAL(timeout()), this, SLOT(set_timeout()));

    etcList << trUtf8("인식되지 않은 지폐 :") << trUtf8("동전 :") << trUtf8("수표 :") << trUtf8("상품권 :") << trUtf8("할인권 :");

    // set deposit hook event
    void* my_data_ptr;
    vaultSetDepositEvents(&page_result::OnDepositCompleteEvent,my_data_ptr);
    init_DepositStr();
    init_curDepositStr();

    additionalFlag = 0;
    additionalState = 0;
    ui->btn_additionalDeposit->hide();

    ui->table_result->setColumnWidth(0,250);
    ui->table_result->setColumnWidth(1,100);
    ui->table_result->setColumnWidth(2,214);
}


void page_result::init_DepositStr(){
    qDebug() << "initialize Deposit struct";
    preDeposit.sumCount = 0;
    preDeposit.sumTotal = 0;
    for(int i=0; i<12; i++){
        preDeposit.currencyUnit[i] = 0;
        preDeposit.currencyCount[i] = 0;
        preDeposit.currencySum[i] = 0;
    }
}

void page_result::init_curDepositStr(){
    qDebug() << "initialize Deposit struct";
    curDeposit.sumCount = 0;
    curDeposit.sumTotal = 0;
    curDeposit.curTotal = 0;
    curDeposit.curCount = 0;
    for(int i=0; i<12; i++){
        curDeposit.currencyCount[i] = 0;
        curDeposit.currencySum[i] = 0;
    }
}

page_result::~page_result()
{
    delete ui;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void page_result::set_timeout()
{
//    if(current_step == IDLE_DEPOSIT){
//        p_Main->select_Page(LOGINPAGE);
//        result_timer->stop();
//    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void page_result::OnDepositCompleteEvent(LPVAULTDEPOSIT_INFO deposit_info_ptr, int status, void* user_data){
    qDebug() << "Called Deposit Complete Event" << status;
    cmd_RequestStop();

    p_Main->deviceStatusFlag = status;

    p_Main->pResult->curDeposit = *deposit_info_ptr;

    if(p_Main->pResult->curDeposit.sumCount == 0){   // 입금받은게 없어
        if(p_Main->pLoading->depositFlag){          // 이전에 입금된게 있으면.
            qDebug() << "이전에 입금된게 있다~pre deposit";
            p_Main->select_Page(RESULTPAGE);
        }
        else{
            p_Main->pLoading->init_loadingPage(1);
            p_Main->select_Page(LOADINGPAGE);
        }
    }
    else{
        p_Main->pLoading->depositFlag = 1;
        p_Main->select_Page(RESULTPAGE);

        // AT DBCHANGE
        if(get_inGateState() == GATE_OPENED){               // 벌크모드 구분이 요기잉네
            QString values;
            values.append(QString("'%1',").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
            values.append(QString("'%1',").arg(p_Main->deviceNum));
            values.append(QString("'%1',").arg(p_Main->turnNum));
            values.append(QString("'%1',").arg(p_Main->depositNum));
            values.append("'bulk',");
            values.append(QString("'%1',").arg(p_Main->curCountryText));
            values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumTotal,0,'f',0));
            values.append(QString("'%1',").arg(p_Main->pResult->curDeposit.sumCount,0,'f',0));
            QString etcSum= "null";
            values.append(QString("'%1',").arg(etcSum));        // 기타입금
            values.append(QString("'%1',").arg(p_Main->curUserId));         // userid is primary id
            values.append("'0',");                                                         // process time 0 으로저장

            QString currencySum;
            for(int i=0; i<12; i++){
                currencySum.append(QString("%1-").arg(p_Main->pResult->curDeposit.currencySum[i],0,'f',0));
            }
            values.append(QString("'%1',").arg(currencySum));        // 입금 세부 기록

            values.append("'-',");                                    //tmp 1
            values.append("'-'");                                    //tmp 2

            p_Main->currentTransaction = values;

            QVariantList buf;
            if(p_Main->pDatabase->SelectRecord(RECORD_TABLE,"primaryid",QString("depositNum ='%1'").arg(p_Main->depositNum),&buf)==0){
                // 레코드 존재확인
                qDebug() << "bulk deposit data DB save: "  << p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);
            }
            else{
                QString depositTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                qDebug() << buf.at(0) << "bulk deposit data DB update: " << p_Main->pDatabase->UpdateRecord
                            (RECORD_TABLE,QString("time='%1', totalAmount='%2', etcAmount='%3', log='%4'").arg(depositTime)
                             .arg(p_Main->pResult->curDeposit.sumTotal).arg(etcSum).arg(currencySum),QString("depositNum='%1'").arg(p_Main->depositNum));
            }

            p_Main->totalBagCount += p_Main->pResult->curDeposit.curCount;
            p_Main->pDatabase->UpdateRecord(INFO_TABLE, QString("totalCount = '%1'").arg(p_Main->totalBagCount)
                                            ,"recName = 'deviceInfo'");
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void page_result::resultPage_waitTimer(int timeCnt)
{
    ui->lbl_status->setText(QString(trUtf8("%1 초 후에 자동으로 완료 동작이 진행됩니다.")).arg(timeCnt));
}

void page_result::resultPage_State(QString text)
{
    ui->label->setText(text);
}


void page_result::lang_change()
{
    ui->retranslateUi(this);
}

void page_result::resultPage_init()
{
    qDebug() <<"결과 페이지 왔냐!!";
    set_resultButton(1);

    if(p_Main->deviceStatusFlag==VAULT_STATUS_ERROR){
        ui->label->setText(trUtf8("기기에 이상이 발생하였습니다. \n 투입구를 확인해주세요."));
    }
    else{
        ui->label->setText(trUtf8("입금 할 금액을 계속 투입해 주세요.  \n입금을 모두완료 하신 경우 \"입금완료\" 버튼을 클릭해 주세요"));
    }

    ui->edit_eachTotal->setText(QString("%1   %2").arg(QString("%L1").arg(curDeposit.sumTotal+preDeposit.sumTotal ,0,'f',0,' ')).arg(p_Main->curCountryText));

    //--------------------------------------------------------------------------------------------
    qDebug() << "result init";
    int countUnit = 0;
    while(ui->table_result->rowCount()){
        ui->table_result->removeRow(0);
    }
    ui->table_result->setRowCount(0);

    if(additionalFlag <= 0){
        ui->btn_cancelDeposit->show();
    }
    if(p_Main->currentDepositMode != 3){
        for(int i=0; i<12; i++){
            if(curDeposit.currencyCount[i]+preDeposit.currencyCount[i] != 0){
                ui->table_result->insertRow(ui->table_result->rowCount());
                ui->table_result->setItem(ui->table_result->rowCount()-1,0,new QTableWidgetItem(QString("%1").arg(curDeposit.currencyUnit[i])));
                ui->table_result->setItem(ui->table_result->rowCount()-1,1,new QTableWidgetItem(QString(trUtf8(":  %1")).arg(curDeposit.currencyCount[i]+preDeposit.currencyCount[i])));
                ui->table_result->setItem(ui->table_result->rowCount()-1,2,new QTableWidgetItem(QString("%L1 %2").arg(curDeposit.currencySum[i]+preDeposit.currencySum[i],0,'f',0,' ').arg(p_Main->curCountryText)));
                ui->table_result->item(ui->table_result->rowCount()-1,2)->setTextAlignment(Qt::Alignment( Qt::AlignRight | Qt::AlignVCenter));
            }
        }
    }

    switch(p_Main->currentDepositMode){
    case 1:
        qDebug() << "DepositComplete Count" << curDeposit.sumCount;
        if(curDeposit.sumCount == 150){
            qDebug() << "DepositComplete Count MAX";
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m4.mp3").arg(PATH_ROOT)),1,15,true);
            ui->label->setText(trUtf8("포켓 수량 제한을 초과하였습니다.  \n 계속해서 입금하시려면 \"추가입금\" 버튼을 클릭해 주세요."));
            ui->btn_additionalDeposit->show();
            p_Main->pLoading->current_step = IDLE_DEPOSIT;                      // simple에는 있길래 포함시켜봄
        }
        else if(p_Main->pResult->curDeposit.sumCount == 0){
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("").arg(PATH_ROOT)),1,15,true);
            p_Main->pLoading->timingCount = 0;
            p_Main->pLoading->current_step = WAIT_INPUT;
        }
        else{
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m4.mp3").arg(PATH_ROOT)),1,15,true);
            ui->btn_additionalDeposit->hide();
            p_Main->pLoading->timingCount = 0;
            p_Main->pLoading->current_step = WAIT_INPUT;
        }
        break;
    case 2:
        if(curDeposit.sumCount == 150){
            set_resultButton(0);
            ui->btn_cancelDeposit->hide();
            ui->btn_additionalDeposit->hide();
            additionalFlag = 1;
            ui->label->setText(trUtf8("포켓 수량을 초과하였습니다.  \n 자동입금 처리중입니다."));
            p_Main->pLoading->current_step = MID_INOPEN;
            vaultOpenAcceptorGate();
        }
        else if(p_Main->pResult->curDeposit.sumCount == 0){
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("").arg(PATH_ROOT)),1,15,true);
            p_Main->pLoading->timingCount = 0;
            p_Main->pLoading->current_step = WAIT_INPUT;
        }
        else{
            p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/m4.mp3").arg(PATH_ROOT)),1,15,true);
            ui->btn_additionalDeposit->hide();
            p_Main->pLoading->timingCount = 0;
            p_Main->pLoading->current_step = WAIT_INPUT;
        }
        break;
    case 3:
    {
        p_Main->pLoading->current_step = IDLE_DEPOSIT;
        QString tmp1;
        QString tmp2;
        int bufCnt;
        for(int i=0; i<etcList.count(); i++){
            tmp1 = etcList.at(i);
            tmp2 = QString("      %L1").arg(curDeposit.etcEnvelope[i] ,0,'f',0,' ');

            ui->table_result->insertRow(ui->table_result->rowCount());
            ui->table_result->setItem(ui->table_result->rowCount()-1,0,new QTableWidgetItem(tmp1));
            ui->table_result->setItem(ui->table_result->rowCount()-1,1,new QTableWidgetItem(""));
            ui->table_result->setItem(ui->table_result->rowCount()-1,2,new QTableWidgetItem(tmp2));
            ui->table_result->item(ui->table_result->rowCount()-1,2)->setTextAlignment(Qt::Alignment( Qt::AlignRight | Qt::AlignVCenter));
        }
        break;
    }
    }

////    ui->list_result->addItem(QString("입금자명 :                      %1").arg(p_Main->strUser.fullName));
////    ui->list_result->addItem(QString("입금시간 :                      %1").arg(QDateTime::fromTime_t(curDeposit.DepositDateTime).toString("yyyy.MM.dd hh:mm:ss")));

//    preDeposit = curDeposit;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void page_result::on_btn_cancelDeposit_clicked()
{
    cmd_RequestStop();
    p_Main->pLoading->init_loadingPage(21);      // 처리대기 중
    p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
    p_Main->pLoading->current_step = CANCEL_OUTOPEN;
    vaultOpenGate();

    additionalFlag = 0;
    qDebug() << "clicked Deposit Cancel Button";

//    ui->btn_confirmDeposit->setEnabled(false);
//    ui->btn_additionalDeposit->setEnabled(false);
    init_DepositStr();
    p_Main->pLoading->depositFlag = 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void page_result::on_btn_confirmDeposit_clicked()
{
//    print_calculateReceipt();
    cmd_RequestStop();

    ui->btn_cancelDeposit->show();
    additionalFlag = 0;
    qDebug() << "clicked Deposit CONFIRM Button";
    // later 비용정산 카운트 조건추가bulk
//    vaultOpenAcceptorGate();

    if(p_Main->currentDepositMode == 1){            // 일반모드
        // BLOCK STATE
        //p_Main->pLoading->current_step = COMP_BLOPEN;
        //vaultOpenBlock();
        p_Main->pLoading->current_step = COMP_INOPEN;
        vaultOpenAcceptorGate();
    }
    else if(p_Main->currentDepositMode == 2){            // 바로입금모드
//        p_Main->transactionList.insert(QString("%1").arg(p_Main->depositNum),p_Main->currentTransaction.remove("'"));
//        p_Main->currentTransaction = "";
//        p_Main->depositNum++;
        // later 서버에 기록저장해야함

        // BLOCK STATE
        p_Main->pLoading->current_step = COMP_INOPEN;
        vaultOpenAcceptorGate();
//        p_Main->pLoading->timingCount = 3;
            //        p_Main->pLoading->current_step = COMP_INOPEN;
            //        vaultOpenAcceptorGate();
    }
    else if(p_Main->currentDepositMode == 3){       // 기타입금
        QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        QString values;
        values.append(QString("'%1',").arg(curTime));  // later 시간비교할수도있음,일단은 장비ID랑 입금번호 로 구별
        values.append(QString("'%1',").arg(p_Main->deviceNum));
        values.append(QString("'%1',").arg(p_Main->turnNum));
        values.append(QString("'%1',").arg(p_Main->depositNum));
        values.append("'etc',");
        values.append(QString("'%1',").arg(p_Main->curCountryText));
        values.append(QString("'%1',").arg(curDeposit.sumTotal,0,'f',0));
        values.append("'0',");
        QString etcSum;
        for(int i=0; i<5; i++){
            etcSum.append(QString("%1-").arg(curDeposit.etcEnvelope[i],0,'f',0));           // later 소수점 자릿수 구분해야되....
        }
        values.append(QString("'%1',").arg(etcSum));        // 기타입금

        values.append(QString("'%1',").arg(p_Main->curUserId));
        values.append("'0',");                                                         // process time 0 으로저장

        QString currencySum = "null";
        values.append(QString("'%1',").arg(currencySum));        // 입금 세부 기록
        values.append(QString("'%1',").arg(p_Main->curBarCode));                                    //tmp 1
        values.append("'-'");                                    //tmp 2

        p_Main->currentTransaction = values;
        qDebug() << "etc deposit data DB save: " << p_Main->pDatabase->InsertRecord(RECORD_TABLE,p_Main->currentTransaction);
        p_Main->transactionList.insert(QString("%1").arg(curTime),p_Main->currentTransaction.remove("'"));
        p_Main->currentTransaction = "";
        p_Main->depositNum++;

        // BLOCK STATE
        //p_Main->pLoading->current_step = COMP_BLOPEN;
        //vaultOpenBlock();
        p_Main->pLoading->current_step = COMP_INOPEN;
        vaultOpenAcceptorGate();
    }

    print_calculateReceipt();       // 비용정산 영수증

//    init_DepositStr();
//    init_curDepositStr();

    p_Main->pLoading->init_loadingPage(21);      // 처리대기 중
    p_Main->select_Page(LOADINGPAGE);

    // BLOCK STATE
//        p_Main->pLoading->current_step = COMP_BLOPEN;
//        vaultOpenBlock();
    //p_Main->pLoading->current_step = COMP_INOPEN;
    //vaultOpenAcceptorGate();

    p_Main->pLoading->depositFlag = 0;
}

void page_result::print_calculateReceipt(){
    p_Main->printFlag = 1;
    QTextCodec* codec = QTextCodec::codecForName("eucKR");

    InitPrinterStatus();

    SetCharCodeTable(255,2);
    // unsigned char changeFont[] = {0x1b, 0x74, 0x00};
    // ControlCommand(changeFont,3);

//    InitPageMode(0,0,_2INCH,390);       // 인치 및 길이설정
    InitPageMode(0,0,384,390);       // 인치 및 길이설정

    Page_SetStandardMode();

    SetTextStyle(false,true,1,1,false);
    SetTextAlignment(1);
    QString tmp = trUtf8("  입금영수증");

    TextSaveSpool(codec->fromUnicode(tmp).data());  Page_Newline();
    SetFontSize(0);
    Page_DotFeed(15);

    SetTextStyle(false,true,0,0,false);
    SetTextAlignment(2);
    TextSaveSpool(QString("%1\r\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8().data()); SetTextAlignment(2);

    //    Page_DrawBox(0,130,_2INCH,0,2);

    Page_DrawBox(0,130,400,0,2);
    Page_DotFeed(5);

    tmp = p_Main->deviceNum;
    TextSaveSpool(codec->fromUnicode(tmp).data());
    SetTextAlignment(0);

    Page_Newline();
    if(p_Main->currentDepositMode==3){
        tmp = trUtf8("  기타입금");
        TextSaveSpool(codec->fromUnicode(tmp).data());
        Page_Newline();
        tmp = " -----------------------------------";
        TextSaveSpool(codec->fromUnicode(tmp).data());
        Page_Newline();
        for(int i=0; i<5; i++){
            switch(i){
            case 0:
                tmp = trUtf8("  인식되지 않은 지폐:");
                break;
            case 1:
                tmp = trUtf8("  동전 :");
                break;
            case 2:
                tmp = trUtf8("  수표:");
                break;
            case 3:
                tmp = trUtf8("  상품권:");
                break;
            case 4:
                tmp = trUtf8("  할인권:");
                break;
            }
            SetTextAlignment(0);
            //    SetTextStyle(false,false,1,1,false);
            TextSaveSpool(codec->fromUnicode(tmp).data());

            SetTextAlignment(2);
            tmp = QString("%L1  %2").arg(curDeposit.etcEnvelope[i],0,'f',0,' ').arg(p_Main->curCountryText);
            TextSaveSpool(codec->fromUnicode(tmp).data());
            Page_DotFeed(5);
        }
    }
    else{
        tmp = trUtf8("  계수입금");
        TextSaveSpool(codec->fromUnicode(tmp).data());
        Page_Newline();
        tmp = " -----------------------------------";
        TextSaveSpool(codec->fromUnicode(tmp).data());
        Page_Newline();
        for(int i=0; i<12; i++){
            if(curDeposit.currencyUnit[i] != 0 ){
                if(curDeposit.currencyCount[i] != 0){
                    SetTextAlignment(0);
                    tmp = QString("  %1 :").arg(curDeposit.currencyUnit[i]);
                    //    SetTextStyle(false,false,1,1,false);
                    TextSaveSpool(codec->fromUnicode(tmp).data());

                    int cnt = 20 - tmp.count();
                    tmp = QString("%1").arg(curDeposit.currencyCount[i]+preDeposit.currencyCount[i]);
                    for(int i=0; i<cnt; i++){
                        tmp.prepend(" ");
                    }

                    TextSaveSpool(codec->fromUnicode(tmp).data());

                    SetTextAlignment(2);
                    tmp = QString("%L1 %2").arg(curDeposit.currencySum[i]+preDeposit.currencySum[i],0,'f',0,' ').arg(p_Main->curCountryText);
                    TextSaveSpool(codec->fromUnicode(tmp).data());
                    Page_DotFeed(5);
                }
            }
        }
    }

    tmp = " -----------------------------------";
    TextSaveSpool(codec->fromUnicode(tmp).data());

    SetTextAlignment(0);
    tmp = trUtf8("  총입금액:");
    //    SetTextStyle(false,false,1,1,false);
    TextSaveSpool(codec->fromUnicode(tmp).data());
    SetTextAlignment(2);
    tmp = QString("%L1  %2").arg(curDeposit.sumTotal+curDeposit.curTotal,0,'f',0,' ').arg(p_Main->curCountryText);
    TextSaveSpool(codec->fromUnicode(tmp).data());
    Page_DotFeed(5);

    Page_Newline();
    Page_Newline();

    SetTextAlignment(1);
//    OneDimensionBarcodeSaveSpool(CODE128,2,50,TRUE,"20150127123456789");  // later barcode 기준정해야함

    Page_LineFeed(1);

    BmpSaveSpool(p_Main->printerlib,"SetFiles/mark.bmp");

    Page_Print_StandardMode();

    Page_Newline();
    Page_Newline();
    Page_Newline();

    //    SetTextAlignment(1);
    Page_SetPosition(145,390);
//    TextSaveSpool("    www.innostec.co.kr\r\n\r\n\r\n\r\n\r\n");

    FeedToMark();
    CutPaper(0);

    PrintSpool(p_Main->printerlib,TRUE);

    GetPrinterStatus(p_Main->printerlib, 1000);
}

void page_result::receipt_Setup(int flag, int number){

    QTextCodec* codec = QTextCodec::codecForName("eucKR");

    //    QTextCodec codec("eucKR");
    //    codec = QTextCodec::codecForName("eucKR");

    p_Main->printFlag = flag;

    InitPrinterStatus();

    SetCharCodeTable(255,2);
    // unsigned char changeFont[] = {0x1b, 0x74, 0x00};
    // ControlCommand(changeFont,3);

    InitPageMode(0,0,_2INCH,420);       // 인치 및 길이설정

    Page_SetStandardMode();

    SetFontSize(0);
    //    Page_SetPosition(0,105);

    SetFontSize(0);
    Page_DrawBox(0,130,_2INCH,0,2);
    SetTextStyle(false,true,2,2,false);
    SetTextAlignment(1);
    QString tmp = "식  권";
    TextSaveSpool(codec->fromUnicode(tmp).data()); Page_Newline();
    Page_DotFeed(15);

    SetTextStyle(false,true,0,0,false);
    SetTextAlignment(2);
    TextSaveSpool(QString("%1\r\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8().data()); SetTextAlignment(2);
    Page_DotFeed(10);

    SetTextAlignment(1);
    tmp = QString("%1 ♥ %2").arg(p_Main->curEventStr->eventMale).arg(p_Main->curEventStr->eventFemale);
    TextSaveSpool(codec->fromUnicode(tmp).data()); Page_Newline();
    Page_DotFeed(5);
    tmp = "식사 맛있게 하시고";
    TextSaveSpool(codec->fromUnicode(tmp).data()); Page_Newline();
    Page_DotFeed(5);
    tmp = "즐거운 시간 되세요!";
    TextSaveSpool(codec->fromUnicode(tmp).data()); Page_Newline();
    Page_DotFeed(5);
    //        TextSaveSpool(QString(trUtf8("   %1   ")).arg(number).toUtf8().data());
    Page_Newline();


//    SetTextAlignment(1);
    Page_SetPosition(145,390);

    // 바코드 코드구성
    QString barcode = QString("%1%2%3").arg(QDateTime::currentDateTime().toString("yyyyMMdd")).arg(p_Main->pResult->curDeposit.VaultDepositId).arg(number);
    OneDimensionBarcodeSaveSpool(CODE128,2,50,TRUE,barcode.toUtf8().data());

    Page_LineFeed(1);

    Page_Print_StandardMode();

    SetTextAlignment(1);
    TextSaveSpool("www.innostec.co.kr\r\n\r\n\r\n\r\n\r\n");

    FeedToMark();
    CutPaper(0);

    PrintSpool(p_Main->printerlib,TRUE);

    GetPrinterStatus(p_Main->printerlib, 1000);
}

void page_result::on_btn_receipt_clicked()
{
}

void page_result::on_btn_additionalDeposit_clicked()
{
    cmd_RequestStop();

    int ret = p_Main->pMsgDialog->init_MessageDialog(24);
    if(ret){
        p_Main->pLoading->timingCount = 0;
        ui->btn_cancelDeposit->hide();
        ui->btn_additionalDeposit->hide();
        additionalFlag = 1;
        p_Main->pLoading->init_loadingPage(24);      // 처리대기 중
        p_Main->select_Page(LOADINGPAGE);
        // BLOCK STATE
//        p_Main->pLoading->current_step = MID_BLOPEN;
//        vaultOpenBlock();
        p_Main->pLoading->current_step = MID_INOPEN;
        vaultOpenAcceptorGate();
//        vaultIntermediateDeposit();
    }
}

void page_result::set_resultButton(bool checked){
    btnState = checked;
    ui->btn_cancelDeposit->setEnabled(checked);
    ui->btn_additionalDeposit->setEnabled(checked);
    ui->btn_confirmDeposit->setEnabled(checked);
}
