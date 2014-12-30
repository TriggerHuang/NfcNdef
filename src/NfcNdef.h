//============================================================================
// Copyright [2013] <COS>
// Name        : NfcNdef.h
// Author      : TriggerHuang
// Description : Example Application of COS Project
//============================================================================


#pragma once
#include <gaia/core/Page.h>

namespace gaia {
    namespace core {
        class Persistence;
    }
    namespace ui {
        class LinearController;
        class TextWidget;
    }
}

class NfcNdef: public gaia::core::Page {
  public:
    NfcNdef();
    ~NfcNdef();

  protected:
    virtual void onInit(gaia::core::Persistence* const p);
    virtual void onTear();

  private:
    gaia::ui::LinearController* mpCtrl;
    gaia::ui::TextWidget* mpText;
};

