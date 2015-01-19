//============================================================================
// Copyright [2013] <NFC>
// Name        : NfcNdef.h
// Author      : TriggerHuang
// Description : Example Application of NFC Project
//============================================================================


#pragma once
#include <gaia/core/Page.h>

namespace gaia {
    namespace connectivity {
        class NfcManager;
        class NdefMessage;
        class NdefRecord;
        class CreateNdefMessageCallback;
        class Tag;
    }  // connectivity

    namespace core {
        class OnClickListener;
        class Persistence;
        class Scene;
        class widget;
        class Context;
        class Handler;
        class Message;
        class Subscriber;
        class DemandFilter;
        class Demand;
        class PendingDemand;
    }  // core

    namespace ui {
        class Button;
        class EditWidget;
        class LinearController;
        class TextWidget;
        class ListWidget;
        class ScrollWidget;
    }  // ui
    namespace base {
        class String;
        template<typename T> class Vector;
        template <class TYPE> class Vector2D;
        class ByteArray;
    }  // base

    namespace net {
        class Uri;
    }

    namespace locale {
        class Locale;
    }
}  // gaia

template <typename T> class Blob;
class NfcNdef: public gaia::core::Page {
  public:
    NfcNdef();
    ~NfcNdef();

  protected:
    virtual void onInit(gaia::core::Persistence* const p);
    virtual void onRestart();
    virtual void onStart();
    virtual void onPreResume();
    virtual void onResume();
    virtual void onPostResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onTear();
    virtual void onNewDemand(gaia::core::Demand* const demand);;



  public:
    void resolveDemand(const gaia::core::Demand& demand);
    void processNdefMessages(const gaia::base::Array<gaia::connectivity::NdefMessage *> msgs);  // from
    void parseNdefMessage(const gaia::connectivity::NdefMessage* ndefMessage);  // NdefMessageParse

    //bool getEnablerState();
    gaia::base::String NfcNdef::bytesToString(const gaia::base::ByteArray& bs);
    void showMessage(const gaia::base::String& msg);
    void showWarning(const gaia::base::String& msg);
    //gaia::base::String NfcTECH::bytesToString(const gaia::base::ByteArray& bs);


  private:
    gaia::ui::LinearController* mpCtrl;

    gaia::core::Context* mpContext;
    gaia::ui::TextWidget* mpTextNotice;
    gaia::ui::TextWidget* mpTextWarning;
    gaia::ui::ScrollWidget* mpScrollView;
    bool mNfcEnabled;

    gaia::connectivity::NfcAdapter* mpNfcAdapter;


};

