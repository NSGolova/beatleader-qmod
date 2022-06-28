#include "include/UI/ScoreDetails/AccuracyGraph/AccuracyGraphLine.hpp"
#include "include/UI/ScoreDetails/AccuracyGraph/AccuracyGraph.hpp"

#include "include/Assets/BundleLoader.hpp"

#include "include/UI/ScoreDetails/ScoreStatsGraph.hpp"
#include "include/UI/ScoreDetails/AccuracyGraph/AccuracyGraphUtils.hpp"

#include "include/Utils/FormatUtils.hpp"
#include "include/Utils/Range.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Component.hpp"

#include "HMUI/ImageView.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"

#include "UnityEngine/RectTransformUtility.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Time.hpp"

#include "main.hpp"

DEFINE_TYPE(BeatLeader, AccuracyGraph);

using namespace BeatLeader;

static int ViewRectPropertyId;
static int SongDurationPropertyId;
static int CursorPositionPropertyId;

UnityEngine::Rect viewRect;

::ArrayW<UnityEngine::Vector3> corners = ::ArrayW<UnityEngine::Vector3>(new UnityEngine::Vector3(4));
UnityEngine::Vector3 lastPosition3D;

void BeatLeader::AccuracyGraph::Construct(HMUI::ImageView* backgroundImage, BeatLeader::AccuracyGraphLine* graphLineObject) {
    ViewRectPropertyId = UnityEngine::Shader::PropertyToID("_ViewRect");
    SongDurationPropertyId = UnityEngine::Shader::PropertyToID("_SongDuration");
    CursorPositionPropertyId = UnityEngine::Shader::PropertyToID("_CursorPosition");

    graphLine = graphLineObject;

    backgroundMaterial = UnityEngine::Object::Instantiate(BundleLoader::accuracyGraphMaterial);
    backgroundImage->set_material(backgroundMaterial);

    graphLineObject->set_material(BundleLoader::accuracyGraphLine);
    
    underlineText = QuestUI::BeatSaberUI::CreateText(backgroundImage->get_transform(), "", UnityEngine::Vector2(2, -18));

    vrPointer = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer *>()[0];
    cursorInitialized = vrPointer != NULL;
}

float BeatLeader::AccuracyGraph::GetCanvasRadius() {
    if (curvedCanvasSettingsHelper == NULL) {
       curvedCanvasSettingsHelper = HMUI::CurvedCanvasSettingsHelper::New_ctor();
    }

    auto canvasSettings = curvedCanvasSettingsHelper->GetCurvedCanvasSettings(graphLine->get_canvas());
    return canvasSettings == NULL ? 100 : canvasSettings->radius;
}

void BeatLeader::AccuracyGraph::Setup(
    float* points, 
    int length,
    float songDuration) {

    vector<UnityEngine::Vector2> positions;
    AccuracyGraphUtils::PostProcessPoints(points, length, &positions, &viewRect);
    
    this->graphLine->Setup(&positions[0], positions.size(), viewRect, GetCanvasRadius());

    this->points = points;
    this->pointsLength = length;
    this->songDuration = songDuration;

    auto viewRectVector = UnityEngine::Vector4(viewRect.get_xMin(), viewRect.get_yMin(), viewRect.get_xMax(), viewRect.get_yMax());
    backgroundMaterial->SetVector(ViewRectPropertyId, viewRectVector);
    backgroundMaterial->SetFloat(SongDurationPropertyId, songDuration);
}

static string FormatCursorText(float songTime, float accuracy) {
    int fullMinutes = (int)(songTime / 60.0);
    int remainingSeconds = abs((int)songTime % 60);
    return "<color=#B856FF>time: </color>" + to_string(fullMinutes) + ":" + to_string(remainingSeconds) + "<color=#B856FF>accuracy: </color>" + to_string_wprecision(accuracy * 100.0, 2) + "<size=70%>%";
}

void BeatLeader::AccuracyGraph::Update() {
    if (!graphLine->get_gameObject()->get_activeInHierarchy() || targetViewTime == nanf("")) return;

    currentViewTime = AccuracyGraphUtils::Lerp(currentViewTime, targetViewTime, UnityEngine::Time::get_deltaTime() * 10.0);
    auto songTime = currentViewTime * songDuration;
    auto accuracy = GetAccuracy(currentViewTime);
    backgroundMaterial->SetFloat(CursorPositionPropertyId, currentViewTime);
    underlineText->SetText(FormatCursorText(songTime, accuracy));
}

Vector2 CalculateCursorPosition(Vector3 worldCursor, BeatLeader::AccuracyGraphLine* graphLine, float canvasRadius) {
    UnityEngine::RectTransform* graphContainer = (UnityEngine::RectTransform*)graphLine->get_transform();

    auto nonCurved = AccuracyGraphUtils::TransformPointFrom3DToCanvas(worldCursor, canvasRadius * graphContainer->get_lossyScale().x);

    graphContainer->GetWorldCorners(corners);

    return Vector2(
        Range(corners[0].x, corners[3].x).GetRatio(nonCurved.x),
        Range(corners[0].y, corners[1].y).GetRatio(nonCurved.y)
    );
}

float UpdateCursor(Vector2 normalized) {
    if (normalized.x < 0 || normalized.y < 0 || normalized.x > 1 || normalized.y > 1) return -1;
    auto viewCursor = AccuracyGraphUtils::NormalizedToPoint(viewRect, normalized);
    return AccuracyGraphUtils::Clamp01(viewCursor.x);
}

void BeatLeader::AccuracyGraph::LateUpdate() {
    if (!graphLine->get_gameObject()->get_activeInHierarchy() || !cursorInitialized) return;

    auto cursorPosition3D = vrPointer->get_cursorPosition();
    if (cursorPosition3D.Equals(lastPosition3D)) return;
    lastPosition3D = cursorPosition3D;

    auto normalized = CalculateCursorPosition(cursorPosition3D, graphLine, GetCanvasRadius());
    
    float newTargetTime = UpdateCursor(normalized);
    if (newTargetTime != -1) {
        targetViewTime = newTargetTime;
    }
}

float BeatLeader::AccuracyGraph::GetAccuracy(float viewTime) {
    if (pointsLength == 0) return 1.0;

    auto xStep = 1.0 / (float)pointsLength;
    auto x = xStep;

    for (int i = 1; i < pointsLength; i++, x += xStep) {
        if (x < viewTime) continue;
        auto xRange = Range(x - xStep, x);
        auto yRange = Range(points[i - 1], points[i]);
        auto ratio = xRange.GetRatio(viewTime);
        return yRange.SlideBy(ratio);
    }

    return points[pointsLength - 1];
}