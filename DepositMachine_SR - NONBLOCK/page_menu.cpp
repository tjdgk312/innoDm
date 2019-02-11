#include "page_menu.h"
#include "ui_page_menu.h"
#include "global.h"

page_menu::page_menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::page_menu)
{
    ui->setupUi(this);
    ui->frm_country->hide();
    ui->frm_country->setEnabled(true);

    buttonList << ui->btn_manual << ui->btn_auto << ui->btn_etc << ui->btn_country_1 << ui->btn_country_2 <<ui->btn_country_3 <<ui->btn_country_4 << ui->btn_country_5;
    for(int i=0; i<buttonList.count(); i++){
        connect(buttonList.at(i),SIGNAL(pressed()),this,SLOT(on_menuBtn_pressed()));
    }

    btnGroup = new QButtonGroup();                                          // 화폐 종류 선택 토글버튼 그룹
    btnGroup->addButton(ui->btn_country_1,0);
    btnGroup->addButton(ui->btn_country_2,1);
    btnGroup->addButton(ui->btn_country_3,2);
    btnGroup->addButton(ui->btn_country_4,3);
    btnGroup->addButton(ui->btn_country_5,4);
}

void page_menu::init_menuPage(){
    ui->frm_country->show();
}


page_menu::~page_menu()
{
    delete ui;
}

void page_menu::lang_change()
{
    ui->retranslateUi(this);
}

void page_menu::on_menuBtn_pressed()
{
    qDebug() << "menu page pressed";

//    if(p_Main->initializeFlag == 0){        // 통신 끊김
//        p_Main->pMsgDialog->init_MessageDialog(2);
//        return;
//    }
//    if(p_Main->initializeFlag == 1){        // 통신 끊김
//        p_Main->pMsgDialog->init_MessageDialog(4);
//        return;
//    }

//    if(p_Main->rollLeft <= 10){
//        int ret = p_Main->pMsgDialog->init_MessageDialog(13);
//        if(!ret){
//            return;
//        }
//    }

    QPushButton* button = qobject_cast<QPushButton*>(sender());
    //초기화 작업 시에 저장된 컨트롤러의 메모리 주소 좌표를 얻기
    int Select_X;
    for(int i=0; i<buttonList.count(); i++){
        if(button->objectName() == buttonList.at(i)->objectName()){
            Select_X = i;               //editValue의 x축 분류를 저장하기 위함
            break;
        }
    }

    QString tmpStr;
    QString strRow;

    p_Main->currentTransaction = "";

    switch(Select_X){
    case 0:     // escrow 모드
        p_Main->currentDepositMode = ESCROWDEPOSIT;
        p_Main->pLoading->init_loadingPage(1);
        p_Main->select_Page(LOADINGPAGE);
        break;
    case 1:     // bulk 모드
        p_Main->currentDepositMode = BULKDEPOSIT;
        p_Main->pLoading->init_loadingPage(1);
        p_Main->select_Page(LOADINGPAGE);               // 완료모드 진입이랑 순서바꿈
//        p_Main->pLoading->current_step = CAL_BLOPEN;
//        vaultOpenBlock();
//        p_Main->pLoading->current_step = CAL_INOPEN;
//        vaultOpenAcceptorGate();
        //SDK - getCurrentUsecountry();  int return  0 or other page, frame change
        break;
    case 2:
//        p_Main->currentCountry = DEFAULTCOUNTRY;
        p_Main->setAudioMedia(QUrl::fromLocalFile(QString("%1audio/o1.mp3").arg(PATH_ROOT)),1,15,true);
        p_Main->currentDepositMode = Select_X+1;
        p_Main->select_Page(ETCPAGE);
        vaultOpenGate();
        break;
    case 3:     // 국가1
    case 4:     // 국가2
    case 5:     // 국가3
    case 6:     // 국가4
    case 7:     // 국가5

        p_Main->currentCountry = button->whatsThis();
        p_Main->curCountryText = button->text();

//        qDebug() << button->statusTip() << button->text()
//                 << p_Main->currentCountry << p_Main->curCountryText;
/*         tmpStr = p_Main->currentCountry;
         tmpStr.append(pocketCnt);
         setDepositCountry(tmpStr.toUtf8().data()); */                                                                                              // SDK - setCountry
//         p_Main->pLoading->init_loadingPage(1);
//         p_Main->select_Page(LOADINGPAGE);
         ui->frm_country->hide();
        break;
    }

    if(Select_X < 2){
        if(countryCount <= 1){
//            if(countryCount==0) tmpStr= p_Main->currentCountry;                                                  // 국가설정수가 0인경우 = Default 값은 DEFAULTCOUNTRY Define으로 계수기 값이 설정 되도록 작업
//            else{
//                strRow = countryList.at(0);
//                tmpStr = p_Main->pSetting->buttonList.at(strRow.toInt())->statusTip();           // statustip = 국가코드
//            }
            if(Select_X == 1){
                // 펌웨어에따라서 달라짐..
//                pocketCnt = "01";       // 150        // 예전 펌웨어일 경우 01로 업데이트해야함

                pocketCnt = "02";       // 150
            }
            else{
                // 펌웨어에따라서 달라짐..
//                pocketCnt = "01";       // 150        // 예전 펌웨어일 경우 01로 업데이트해야함
                pocketCnt = "02";       // 100
            }
            tmpStr = p_Main->currentCountry;
            tmpStr.append(pocketCnt);
            setDepositCountry(tmpStr.toUtf8().data());
            // SDK - setCountry
            clearStackerState();
        }
        else{
//            ui->frm_country->show();
        }
    }
}


//-----------------------------------------------------------------------------------------------------
void page_menu::on_btn_deposit_clicked()            // 수납 버튼 클릭 시
{
    if(p_Main->initializeFlag == 0){        // 통신 끊김
        p_Main->pMsgDialog->init_MessageDialog(2);
        return;
    }
    if(p_Main->initializeFlag == 1){        // 초기화 이전일 경우
        p_Main->pMsgDialog->init_MessageDialog(4);
        return;
    }

    p_Main->pLogin->assignInfo = 0;
    p_Main->select_Page(EVENTVIEWPAGE);
}

void page_menu::on_btn_deposit_2_clicked()
{
    if(p_Main->initializeFlag == 0){        // 통신 끊김
        p_Main->pMsgDialog->init_MessageDialog(2);
        return;
    }
    if(p_Main->initializeFlag == 1){        // 초기화 이전일 경우
        p_Main->pMsgDialog->init_MessageDialog(4);
        return;
    }

    p_Main->pLogin->assignInfo = 4;
    p_Main->select_Page(VISITORPAGE);
}

void page_menu::on_btn_back_clicked()
{
    p_Main->select_Page(EVENTVIEWPAGE);
}


void page_menu::on_btn_test_clicked()
{
    clearErrorState();
}

void page_menu::on_btn_home_clicked()
{
    p_Main->select_Page(LOGINPAGE);
}
