#include "HMUI/Touchable.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/Application.hpp"
#include "UnityEngine/GUIUtility.hpp"

#include "Assets/BundleLoader.hpp"

#include "include/UI/VotingButton.hpp"

#include "main.hpp"

#include <string>
#include <cmath>

using namespace QuestUI;
using namespace std;

DEFINE_TYPE(BeatLeader, VotingButton);

static int SpinnerValuePropertyId;
static int GradientValuePropertyId;
static int StatePropertyId;
static int TintPropertyId;

void BeatLeader::VotingButton::Init(QuestUI::ClickableImage* imageView) {
    this->imageView = imageView;

    this->materialInstance = UnityEngine::Object::Instantiate(BundleLoader::VotingButtonMaterial);
    imageView->set_material(this->materialInstance);

    imageView->set_defaultColor(UnityEngine::Color(0.0, 0.0, 0.0, 1.0));
    imageView->set_highlightColor(UnityEngine::Color(1.0, 0.0, 0.0, 1.0));

    SpinnerValuePropertyId = UnityEngine::Shader::PropertyToID("_SpinnerValue");
    GradientValuePropertyId = UnityEngine::Shader::PropertyToID("_GradientValue");
    StatePropertyId = UnityEngine::Shader::PropertyToID("_State");
    TintPropertyId = UnityEngine::Shader::PropertyToID("_Tint");
}

void UpdateMaterial(UnityEngine::Material* materialInstance, float spinnerValue, float gradientValue, float state, UnityEngine::Color color) {
    materialInstance->SetFloat(SpinnerValuePropertyId, spinnerValue);
    materialInstance->SetFloat(GradientValuePropertyId, gradientValue);
    materialInstance->SetFloat(StatePropertyId, state);
    materialInstance->SetColor(TintPropertyId, color);
}

void BeatLeader::VotingButton::SetState(int state) {

    switch (state)
    {
    case 0:
        UpdateMaterial(materialInstance, 1, 0, 0, UnityEngine::Color(0.2f, 0.2f, 0.2f, 0.0f));
        break;
    case 1:
        UpdateMaterial(materialInstance, 0, 0, 0, UnityEngine::Color(0.2f, 0.2f, 0.2f, 0.0f));
        break;
    case 2:
        UpdateMaterial(materialInstance, 0, 1, 1, UnityEngine::Color(1.0f, 1.0f, 1.0f, 0.7f));
        break;
    case 3:
        UpdateMaterial(materialInstance, 0, 0, 2, UnityEngine::Color(0.2f, 0.8f, 0.2f, 0.4f));
        break;
    
    default:
        break;
    }
}