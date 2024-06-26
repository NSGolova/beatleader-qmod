#include "include/UI/ScoreDetails/ScoreDetailsUI.hpp"
#include "include/Utils/FormatUtils.hpp"
#include "include/Assets/Sprites.hpp"
#include "include/Assets/BundleLoader.hpp"
#include "include/UI/EmojiSupport.hpp"
#include "include/UI/ScoreDetails/ScoreStatsGrid.hpp"
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

static UnityEngine::Color GoodColor = UnityEngine::Color(0.0f, 0.2f, 1.0f, 1.0f);
static UnityEngine::Color BadColor = UnityEngine::Color(0.0f, 0.1f, 0.3f, 0.1f);
static UnityEngine::Color EmptyColor = UnityEngine::Color(0.1f, 0.1f, 0.1f, 0.0f);

BeatLeader::ScoreStatsGridCell::ScoreStatsGridCell() {}

BeatLeader::ScoreStatsGridCell::ScoreStatsGridCell(HMUI::ModalView *modal, int index) noexcept {
    float row = (float)(index / 4);
    float column = (float)(index % 4);

    imageView = QuestUI::CreateImage(modal->get_transform(), Sprites::get_TransparentPixel(), UnityEngine::Vector2((column - 2.0f) * 11.0 + 5, (row - 1.0f) * 11.0 - 3), UnityEngine::Vector2(10, 10));
    imageView->set_color(EmptyColor);
    auto backgroundMaterial = UnityEngine::Object::Instantiate(BundleLoader::bundle->accGridBackgroundMaterial);
    imageView->set_material(backgroundMaterial);

    scoreText = CreateText(modal->get_transform(), "", UnityEngine::Vector2((column - 2.0f) * 11.0 + 32, (row - 1.0f) * 11.0 - 6));
    scoreText->set_fontSize(3);
}

void BeatLeader::ScoreStatsGridCell::setScore(float score, float ratio) const {
    scoreText->SetText(to_string_wprecision(score, 2), true);
    imageView->set_color(UnityEngine::Color::Lerp(BadColor, GoodColor, ratio * ratio));
}

        
void BeatLeader::ScoreStatsGridCell::setSelected(bool selected) const {
    scoreText->get_gameObject()->SetActive(selected);
    imageView->get_gameObject()->SetActive(selected);
}