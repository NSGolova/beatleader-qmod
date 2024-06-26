#include "include/UI/ScoreDetails/ScoreDetailsUI.hpp"
#include "include/Utils/FormatUtils.hpp"
#include "include/Assets/Sprites.hpp"
#include "include/Assets/BundleLoader.hpp"
#include "include/UI/EmojiSupport.hpp"
#include "include/UI/ScoreDetails/ScoreStatsOverview.hpp"
#include "include/UI/QuestUI.hpp"

#include "UnityEngine/Resources.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/Component.hpp"

#include "bsml/shared/BSML/Components/Backgroundable.hpp"

#include "main.hpp"

#include <sstream>

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace GlobalNamespace;

static UnityEngine::Color LeftColor = UnityEngine::Color(0.8f, 0.2f, 0.2f, 0.1f);
static UnityEngine::Color RightColor = UnityEngine::Color(0.2f, 0.2f, 0.8f, 0.1f);
static int FillPropertyId = 0;

BeatLeader::ScoreStatsOverview::ScoreStatsOverview(HMUI::ModalView *modal) noexcept {

    auto modalTransform = modal->get_transform();

    leftPreScore = CreateText(modalTransform, "", UnityEngine::Vector2(3.0, 7.0));
    leftAccScore = CreateText(modalTransform, "", UnityEngine::Vector2(3.0, 3.0));
    leftPostScore = CreateText(modalTransform, "", UnityEngine::Vector2(3.0, -1.0));

    leftScore = CreateText(modalTransform, "", UnityEngine::Vector2(-9.0, 5.8));
    leftPieImage = ::BSML::Lite::CreateImage(modalTransform, Sprites::get_TransparentPixel(), UnityEngine::Vector2(-9, 6), UnityEngine::Vector2(14, 14));
    leftPieImage->set_color(UnityEngine::Color(0.8f, 0.2f, 0.2f, 0.1f));
    leftPieImage->set_material(UnityEngine::Object::Instantiate(BundleLoader::bundle->handAccIndicatorMaterial));

    rightScore = CreateText(modalTransform, "", UnityEngine::Vector2(9.0, 5.8));
    rightPieImage = ::BSML::Lite::CreateImage(modalTransform, Sprites::get_TransparentPixel(), UnityEngine::Vector2(9, 6), UnityEngine::Vector2(14, 14));
    rightPieImage->set_color(UnityEngine::Color(0.2f, 0.2f, 0.8f, 0.1f));
    rightPieImage->set_material(UnityEngine::Object::Instantiate(BundleLoader::bundle->handAccIndicatorMaterial));

    rightPreScore = CreateText(modalTransform, "", UnityEngine::Vector2(-3.0, 9.0));
    rightAccScore = CreateText(modalTransform, "", UnityEngine::Vector2(-3.0, 5.0));
    rightPostScore = CreateText(modalTransform, "", UnityEngine::Vector2(-3.0, 1.0));

    FillPropertyId = UnityEngine::Shader::PropertyToID("_FillValue");

    tdTitle = CreateText(modalTransform, "TD", UnityEngine::Vector2(0.0, -5.0));
    tdTitle->set_alignment(TMPro::TextAlignmentOptions::Center);
    tdBackground = ::BSML::Lite::CreateImage(modalTransform, Sprites::get_TransparentPixel(), UnityEngine::Vector2(0, -9), UnityEngine::Vector2(50, 5));
    tdBackground->set_material(BundleLoader::bundle->accDetailsRowMaterial);
    preTitle = CreateText(modalTransform, "Pre", UnityEngine::Vector2(0.0, -10.0));
    preTitle->set_alignment(TMPro::TextAlignmentOptions::Center);
    preBackground = ::BSML::Lite::CreateImage(modalTransform, Sprites::get_TransparentPixel(), UnityEngine::Vector2(0, -14), UnityEngine::Vector2(50, 5));
    preBackground->set_material(BundleLoader::bundle->accDetailsRowMaterial);
    postTitle = CreateText(modalTransform, "Post", UnityEngine::Vector2(0.0, -15.0));
    postTitle->set_alignment(TMPro::TextAlignmentOptions::Center);
    postBackground = ::BSML::Lite::CreateImage(modalTransform, Sprites::get_TransparentPixel(), UnityEngine::Vector2(0, -19), UnityEngine::Vector2(50, 5));
    postBackground->set_material(BundleLoader::bundle->accDetailsRowMaterial);

    leftTd = CreateText(modalTransform, "", UnityEngine::Vector2(5.0, -7.0));
    leftPre = CreateText(modalTransform, "", UnityEngine::Vector2(5.0, -12.0));
    leftPost = CreateText(modalTransform, "", UnityEngine::Vector2(5.0, -17.0));

    rightTd = CreateText(modalTransform, "", UnityEngine::Vector2(-5.0, -5.0));
    rightPre = CreateText(modalTransform, "", UnityEngine::Vector2(-5.0, -10.0));
    rightPost = CreateText(modalTransform, "", UnityEngine::Vector2(-5.0, -15.0));
}

static float CalculateFillValue(float score) {
    float a = (score - 65) / 50.0f;
    float ratio = a > 1 ? 1.0f : (a < 0 ? 0 : a);
    return pow(ratio, 0.6f);
}

void BeatLeader::ScoreStatsOverview::setScore(ScoreStats score) const {
    leftPreScore->SetText(to_string_wprecision(score.accuracyTracker.leftAverageCut[0], 2), true);
    leftAccScore->SetText(to_string_wprecision(score.accuracyTracker.leftAverageCut[1], 2), true);
    leftPostScore->SetText(to_string_wprecision(score.accuracyTracker.leftAverageCut[2], 2), true);

    leftScore->SetText(to_string_wprecision(score.accuracyTracker.accLeft, 2), true);
    leftScore->set_alignment(TMPro::TextAlignmentOptions::Center);
    leftPieImage->get_material()->SetFloat(FillPropertyId, CalculateFillValue(score.accuracyTracker.accLeft));
    
    rightScore->SetText(to_string_wprecision(score.accuracyTracker.accRight, 2), true);
    rightScore->set_alignment(TMPro::TextAlignmentOptions::Center);
    rightPieImage->get_material()->SetFloat(FillPropertyId, CalculateFillValue(score.accuracyTracker.accRight));

    rightPreScore->SetText(to_string_wprecision(score.accuracyTracker.rightAverageCut[0], 2), true);
    rightPreScore->set_alignment(TMPro::TextAlignmentOptions::Right);
    rightAccScore->SetText(to_string_wprecision(score.accuracyTracker.rightAverageCut[1], 2), true);
    rightAccScore->set_alignment(TMPro::TextAlignmentOptions::Right);
    rightPostScore->SetText(to_string_wprecision(score.accuracyTracker.rightAverageCut[2], 2), true);
    rightPostScore->set_alignment(TMPro::TextAlignmentOptions::Right);

    leftTd->SetText(to_string_wprecision(score.accuracyTracker.leftTimeDependence, 3), true);
    leftPre->SetText(to_string_wprecision(score.accuracyTracker.leftPreswing * 100.0, 2) + "%", true);
    leftPost->SetText(to_string_wprecision(score.accuracyTracker.leftPostswing * 100.0, 2) + "%", true);

    rightTd->SetText(to_string_wprecision(score.accuracyTracker.rightTimeDependence, 3), true);
    rightTd->set_alignment(TMPro::TextAlignmentOptions::Right);
    rightPre->SetText(to_string_wprecision(score.accuracyTracker.rightPreswing * 100.0, 2) + "%", true);
    rightPre->set_alignment(TMPro::TextAlignmentOptions::Right);
    rightPost->SetText(to_string_wprecision(score.accuracyTracker.rightPostswing * 100.0, 2) + "%", true);
    rightPost->set_alignment(TMPro::TextAlignmentOptions::Right);
}

void BeatLeader::ScoreStatsOverview::setSelected(bool selected) const {
    leftPreScore->get_gameObject()->SetActive(selected);
    leftAccScore->get_gameObject()->SetActive(selected);
    leftPostScore->get_gameObject()->SetActive(selected);
    leftScore->get_gameObject()->SetActive(selected);
    leftPieImage->get_gameObject()->SetActive(selected);
    rightScore->get_gameObject()->SetActive(selected);
    rightPieImage->get_gameObject()->SetActive(selected);
    rightPreScore->get_gameObject()->SetActive(selected);
    rightAccScore->get_gameObject()->SetActive(selected);
    rightPostScore->get_gameObject()->SetActive(selected);
    tdTitle->get_gameObject()->SetActive(selected);
    tdBackground->get_gameObject()->SetActive(selected);
    preTitle->get_gameObject()->SetActive(selected);
    preBackground->get_gameObject()->SetActive(selected);
    postTitle->get_gameObject()->SetActive(selected);
    postBackground->get_gameObject()->SetActive(selected);
    leftTd->get_gameObject()->SetActive(selected);
    leftPre->get_gameObject()->SetActive(selected);
    leftPost->get_gameObject()->SetActive(selected);
    rightTd->get_gameObject()->SetActive(selected);
    rightPre->get_gameObject()->SetActive(selected);
    rightPost->get_gameObject()->SetActive(selected);
}