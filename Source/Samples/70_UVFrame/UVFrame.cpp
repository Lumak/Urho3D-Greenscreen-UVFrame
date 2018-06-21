//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/Material.h>
#include <stdio.h>
#include <SDL/SDL_log.h>

#include "UVFrame.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void UVFrame::RegisterObject(Context* context)
{
    context->RegisterFactory<UVFrame>();

    // type
    URHO3D_ATTRIBUTE("enabled",         bool,       enabled_,        false,         AM_DEFAULT );
    URHO3D_ATTRIBUTE("repeat",          bool,       repeat_,         false,         AM_DEFAULT );

    // uv frame
    URHO3D_ATTRIBUTE("rows",            int,        rows_,           0,             AM_DEFAULT );
    URHO3D_ATTRIBUTE("cols",            int,        cols_,           0,             AM_DEFAULT );
    URHO3D_ATTRIBUTE("numFrames",       int,        numFrames_,      0,             AM_DEFAULT );
    URHO3D_ATTRIBUTE("timePerFrame",    unsigned,   timePerFrame_,   0,             AM_DEFAULT );
}

UVFrame::UVFrame(Context* context) 
    : LogicComponent(context)
    , rows_(0)
    , cols_(0)      
    , numFrames_(0)
    , timePerFrame_(0)
    , enabled_(false)
    , repeat_(false)
    , curFrameIdx_(0)
{
    SetUpdateEventMask(USE_UPDATE);
}

void UVFrame::DelayedStart()
{
    // currently only looks for billboardset and staticmodel, add more if you need
    // **sadly, the drawable base class doesn't have a virutal GetMaterial() function
    if (node_->GetComponent<BillboardSet>())
    {
        drawableComponent_ = node_->GetComponent<BillboardSet>();
        componentMat_ = node_->GetComponent<BillboardSet>()->GetMaterial();
    }
    else if (node_->GetComponent<StaticModel>())
    {
        componentMat_ = node_->GetComponent<StaticModel>()->GetMaterial();
        drawableComponent_ = node_->GetComponent<StaticModel>();
    }

    // init
    Reset();

    // auto start
    if (!enabled_)
    {
        SetUpdateEventMask(0);
    }
}

bool UVFrame::SetEnabled(bool enable)
{
    if (enable == enabled_)
    {
        return false;
    }

    enabled_ = enable;

    if (enabled_)
    {
        SetUpdateEventMask(USE_UPDATE);
    }
    else
    {
        SetUpdateEventMask(0);
    }

    return true;
}

bool UVFrame::Reset()
{
    // init common
    curFrameIdx_ = 0;
    seqTimer_.Reset();

    UpdateUVFrame();

    return true;
}

void UVFrame::Update(float timeStep)
{
    // skip if not in view
    if (!drawableComponent_->IsInView())
        return;

    // update
    UpdateUVFrame();
}

void UVFrame::UpdateUVFrame()
{
    if (seqTimer_.GetMSec(false) > timePerFrame_)
    {
        if ( ++curFrameIdx_ < numFrames_ )
        {
            UpdateUVFrameShader();
        }
        else
        {
            if (repeat_)
            {
                curFrameIdx_ = 0;

                UpdateUVFrameShader();
            }
            else
            {
                SetEnabled(false);
            }
        }

        seqTimer_.Reset();
    }
}

void UVFrame::UpdateUVFrameShader()
{
    float curRow = (float)(curFrameIdx_ / cols_);
    float curCol = (float)(curFrameIdx_ % cols_);

    componentMat_->SetShaderParameter("CurRowCol", Vector2(curRow, curCol));
}



