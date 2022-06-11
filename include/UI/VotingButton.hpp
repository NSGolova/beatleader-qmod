#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Material.hpp"

#include "custom-types/shared/macros.hpp"

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

DECLARE_CLASS_CODEGEN(BeatLeader, VotingButton, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(QuestUI::ClickableImage*, imageView);
    DECLARE_INSTANCE_FIELD(UnityEngine::Material*, materialInstance);

    DECLARE_INSTANCE_METHOD(void, Init, QuestUI::ClickableImage* imageView);
    DECLARE_INSTANCE_METHOD(void, SetState, int state);
)