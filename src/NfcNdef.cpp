//============================================================================
// Copyright [2013] <COS>
// Name        : NfcNdef.cpp
// Author      : TriggerHuang
// Description : Example Application of COS Project
//============================================================================
#define DEBUG_LEVEL 0
#define LOG_TAG "NfcNdefSample"

#include <gaia/core/Context.h>
#include <gaia/core/Persistence.h>
#include <gaia/core/vision/Scene.h>
#include <gaia/ui/LinearController.h>
#include <gaia/ui/TextWidget.h>
#include <gaia/base/String.h>
#include "NfcNdef.h"
#include "nfcndef/R.h"

using namespace gaia::core;
using namespace gaia::ui;
using namespace gaia::base;

  NfcNdef::NfcNdef() :
    mpCtrl(NULL),
    mpText(NULL) {
}

  NfcNdef::~NfcNdef() {
}

void NfcNdef::onInit(Persistence* const p) {
    Scene* pScene = Scene::SceneFactory(this);

    mpText = new TextWidget(this);
    mpText->setText(nfcndef::R::string::app_name);
    mpText->setTextSize(30);

    mpCtrl = new LinearController(this);
    mpCtrl->addWidget(mpText);

    pScene->attachController(mpCtrl);
    pScene->setupSceneLayout();
}

void NfcNdef::onTear() {
    delete mpCtrl;
    delete mpText;
    getSceneSingleton()->SceneDestroy();
}

template class Export<NfcNdef, Page>;

