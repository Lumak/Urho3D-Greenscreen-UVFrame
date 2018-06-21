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

#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

namespace Urho3D
{
}
//=============================================================================
//=============================================================================
enum UVSeqType
{
    UVSeq_UScroll,      // 0
    UVSeq_VScroll,      // 1
    UVSeq_UVFrame,      // 2
    UVSeq_SwapImage,    // 3
};

//=============================================================================
//=============================================================================
class UVFrame : public LogicComponent
{
    URHO3D_OBJECT(UVFrame, LogicComponent);

public:
    UVFrame(Context* context);
    virtual ~UVFrame(){}

    static void RegisterObject(Context* context);
    virtual void DelayedStart();
    bool SetEnabled(bool enable);
    bool Reset();
    bool IsEnabled() const { return enabled_; }

protected:
    virtual void Update(float timeStep);
    void UpdateUScroll(float timeStep);
    void UpdateVScroll(float timeStep);
    void UpdateUVFrame();
    void UpdateUVFrameShader();
    void UpdateSwapImage();
    void UpdateSwapImageTexture();

    void InitSwapDecFormat();
    void InitUVFrameSize();
    static const char *GetDecFormat(int idx, bool leadingZero);
    
protected:
    WeakPtr<Drawable> drawableComponent_;
    WeakPtr<Material> componentMat_;

    // type
    bool              enabled_;
    bool              repeat_;

    // uv frame
    int               rows_;
    int               cols_;
    int               numFrames_;
    unsigned          timePerFrame_;

    // status update
    int               curFrameIdx_;
    Timer             seqTimer_;

};

