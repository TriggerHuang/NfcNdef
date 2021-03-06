//============================================================================
// Copyright [2013] <NFC>
// Name        : NfcNdef.cpp
// Author      : TriggerHuang
// Description : Example Application of NFC Project
//============================================================================
#define DEBUG_LEVEL 0
#define LOG_TAG "NfcNdefSample"

#include <gaia/base/Array.h>
#include <gaia/base/ByteArray.h>
#include <gaia/base/String.h>
#include <gaia/base/StringArray.h>
#include <gaia/base/Vector.h>
#include <gaia/base/Vector2D.h>
#include <gaia/base/gloger.h>
#include <gaia/core/Context.h>
#include <gaia/core/Message.h>
#include <gaia/core/Handler.h>
#include <gaia/core/ContentResolver.h>
#include <gaia/core/Persistence.h>
#include <gaia/core/event/OnClickListener.h>
#include <gaia/core/vision/Widget.h>
#include <gaia/core/vision/Scene.h>
#include <gaia/graphics/Color.h>
#include <gaia/ui/Button.h>
#include <gaia/ui/EditWidget.h>
#include <gaia/ui/LinearController.h>
#include <gaia/ui/TextWidget.h>
#include <gaia/ui/ScrollWidget.h>

#include <gaia/connectivity/nfc/NfcAdapter.h>
#include <gaia/connectivity/nfc/NfcManager.h>
#include <gaia/connectivity/nfc/NdefRecord.h>
#include <gaia/connectivity/nfc/NdefMessage.h>
#include <gaia/connectivity/nfc/NfcEvent.h>
#include <gaia/connectivity/nfc/MifareClassic.h>
#include <gaia/connectivity/nfc/Tag.h>
#include <gaia/connectivity/nfc/CreateNdefMessageCallback.h>
#include <gaia/core/Subscriber.h>
#include <gaia/core/Demand.h>
#include <gaia/core/DemandFilter.h>
#include <gaia/core/ComponentName.h>
#include <gaia/core/PendingDemand.h>
#include <gaia/net/Uri.h>
#include <gaia/storage/File.h>
#include <gaia/core/Parcelable.h>
#include <gaia/base/List.h>
#include <string>

#include "NfcNdef.h"
#include "nfcndef/R.h"

#define safe_delete(ptr) \
    if ((ptr) != 0) \
    { \
        delete ptr; \
        ptr = 0; \
    }


using namespace gaia::base;
using namespace gaia::core;
using namespace gaia::graphics;
using namespace gaia::content;
using namespace gaia::ui;

using gaia::storage::File;
using gaia::net::Uri;
using gaia::base::String;
using gaia::base::Vector;
using gaia::base::Vector2D;
using gaia::core::Widget;
using gaia::core::Context;
using gaia::core::Demand;
using gaia::core::DemandFilter;
using gaia::core::Message;
using gaia::core::Handler;
using gaia::base::ByteArray;


using gaia::connectivity::NfcAdapter;
using gaia::connectivity::NfcManager;
using gaia::connectivity::NdefMessage;
using gaia::connectivity::NdefRecord;
using gaia::connectivity::NfcEvent;
using gaia::connectivity::CreateNdefMessageCallback;
using gaia::connectivity::Tag;
using gaia::connectivity::MifareClassic;


  NfcNdef::NfcNdef() :
    mpCtrl(NULL),
    mpTextNotice(NULL),
    mpTextWarning(NULL),
    mpScrollView(NULL),
    mNfcEnabled(false),
    mpNfcAdapter(NULL),
    mpContext(NULL) {
}

  NfcNdef::~NfcNdef() {
}

void NfcNdef::showMessage(const gaia::base::String& msg) {
      if (mpTextNotice == NULL) return;

      mpTextNotice->setText(msg);
}

void NfcNdef::showWarning(const gaia::base::String& msg) {
      if (mpTextWarning == NULL) return;

      mpTextWarning->setText(msg);
}

String NfcNdef::bytesToString(const ByteArray& bs) {
    String s = "";
    byte_t b;
    // for (byte b : bs) {
    for (uint32_t i = 0; i < bs.size(); i++) {
        b = bs[i];
        //s= s + String::format("%02X", b);
        s= s + String::format("%c", b);
    }
    return s;
}


void NfcNdef::processNdefMessages(const Array<NdefMessage *> msgs)
{
	String metaInfo = "";
	// 用两个for循环便利所有的NdefMessage以及每个NdefMessage中的所有的Record
	for(unsigned int i = 0; i < msgs.length(); i++) {
		metaInfo = metaInfo + "NdefMessage " + String::valueOf(i) + ": ";
		Vector<NdefRecord>* recordV = msgs[i]->getRecordsN();
		// 每条 NdefMessage中，包含了多少个NdefRecord
		for (unsigned int j = 0; j < recordV->size(); j++) {
			metaInfo = metaInfo + "\n\t" + "NdefRecord" + String::valueOf(j) + "::";
			// 每条NdefMessage的payload的解析，需要根据相关的SPEC
			NdefRecord record = recordV->itemAt(j);
			//  NdefRecord::TnfType
			unsigned int tnf = record.getTnf();
			if (NdefRecord::TNF_WELL_KNOWN != tnf) {
				showMessage("TNF error!");
				return;
			}

			ByteArray Type = record.getType();
			if (!bytesToString(Type).equals(bytesToString(NdefRecord::RTD_TEXT()))){
				showMessage("TYPE ERROR");
				return;
			}

			metaInfo = metaInfo + "\n\t\t" + "Tnf =";
			metaInfo = metaInfo + String::valueOf(tnf);

				// 根据RTD的 RTD_TEXT协议解析
			ByteArray payload = record.getPayload();
			String textEncoding = payload[0]&0x80? String("UTF-16"):String("UTF-8");
			metaInfo = metaInfo + "\n\t\t" + "textEncoding ="+textEncoding;

			int languageCodeLehgth = payload[0]&0x3f;
			metaInfo = metaInfo + "\n\t\t" + "languageCodeLehgth ="+String::valueOf(languageCodeLehgth);

			String languageCode = String(payload, 1, languageCodeLehgth);
			metaInfo = metaInfo + "\n\t\t" + "languageCode =" + languageCode;

				// 第一个byte是状态子，接下来是languageCodeLehgth长度的语言编码，随后开始才
				// 是真正的text数据.
			String playloadText = String(payload, languageCodeLehgth+1, payload.size()-1);
			metaInfo = metaInfo + "\n\t\t" + "payload size =" + String::valueOf((int32_t)payload.size());

			metaInfo = metaInfo + "\n\t\t" + "Type =";
			metaInfo = metaInfo + bytesToString(Type);

			metaInfo = metaInfo + "\n\t\t" + "payload =";

			//metaInfo = metaInfo + bytesToString(payload);
			metaInfo = metaInfo + playloadText;
		}
		metaInfo = metaInfo + "\n\n";
	}

	showMessage(metaInfo);
}


void NfcNdef::resolveDemand(const Demand& demand) {
	String action = demand.getAction();
	    GLOG(LOG_TAG, LOGINFO, "action = %s resolveDemand", action.string());
	    // 首先判断该demand是不是真的来自于ACTION_NDEF_DISCOVERED 正常情况下，都是的
	    if (NfcAdapter::ACTION_NDEF_DISCOVERED().equals(action)){
	    	// 然后判断，该demand是否携带了EXTRA_NDEF_MESSAGES类型的tag
	    	// 请参考 NfcService发送Intent时的逻辑，这里仅仅是个逆过程
	    	if (demand.hasExtra(NfcAdapter::EXTRA_NDEF_MESSAGES())) {
	    		// 从demand中获取 NdefMessage
	    		Array<Parcelable> rawMsgs = demand.getParcelableArrayExtra(NfcAdapter::EXTRA_NDEF_MESSAGES());
	    		Array<NdefMessage *> msgs(rawMsgs.length());

	    		// 一个NDEF Tag中，可能包含了不只一条NdefMessage
	            for (uint32_t i = 0; i < rawMsgs.length(); i++) {
	                bool isNull  = rawMsgs[i].isNull();
	                NdefMessage *msg = rawMsgs[i].getN<NdefMessage>();
	                msgs[i] = msg;
	            }
	            // 将获取到的NdefMessage数组进行解析
	            processNdefMessages(msgs);
	    	}else {
	    		//error, 没有extra，可做出错处理
	    	}
	    }else{
	    //	showWarning("unknown demond:" + String(action.string()) );
	    }
}

void NfcNdef::onInit(Persistence* const p) {
	GLOGENTRY();
    mpContext = new gaia::core::Context(this);
    Scene* pScene = Scene::SceneFactory(this);
    mpCtrl = new LinearController(this);
    mpCtrl->setOrientation(LinearController::HORIZONTAL);


    mpTextNotice = new TextWidget(this);
    mpTextNotice->setTextColor(Color::BLUE);
    mpTextNotice->setTextSize(15);
    mpTextNotice->setText("Notice: ");

    mpScrollView = new ScrollWidget(this);
    mpScrollView->addWidget(mpTextNotice);
    mpCtrl->addWidget(mpScrollView);

    mpTextWarning = new TextWidget(this);
    mpTextWarning->setTextColor(Color::RED);
    mpTextWarning->setTextSize(15);
    mpTextWarning->setText("Warning: ");
    mpCtrl->addWidget(mpTextWarning);


    mpNfcAdapter = NfcAdapter::getDefaultAdapterN(*mpContext);


    pScene->attachController(mpCtrl);
    pScene->setupSceneLayout();

    /*
     * checkNFCFunction();
     * initUI()
     * initNFC();
     * */
}

void NfcNdef::onTear() {
	GLOGENTRY();
    safe_delete(mpCtrl);
    safe_delete(mpContext);
    safe_delete(mpTextNotice);
    safe_delete(mpTextWarning);
    safe_delete(mpScrollView);
    safe_delete(mpNfcAdapter);
    mNfcEnabled = false;
    getSceneSingleton()->SceneDestroy();
}

void NfcNdef::onNewDemand(Demand* const demand)
{
	GLOGENTRY();
	setDemand(*demand);

}

void NfcNdef::onResume() {
	GLOGENTRY();
        const Demand demand(getDemand());
    GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand.toString().string());
    //showMessage(demand.toString().string());
    showWarning(demand.toString().string());
    resolveDemand(demand);

}

void NfcNdef::onRestart() {
	GLOGENTRY();
        const Demand demand(getDemand());
    GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand.toString().string());
    showMessage(demand.toString().string());
    resolveDemand(demand);

}
/*
 *     virtual void onRestart();
    virtual void onStart();
    virtual void onPreResume();
    virtual void onResume();
    virtual void onPostResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onTear();
 */

void NfcNdef::onStart() {
	GLOGENTRY();
}

void NfcNdef::onPreResume() {
	GLOGENTRY();
}

void NfcNdef::onPostResume() {
	GLOGENTRY();
}

void NfcNdef::onPause() {
	GLOGENTRY();
}

void NfcNdef::onStop() {
	GLOGENTRY();
}

template class Export<NfcNdef, Page>;

