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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "StaticScene.h"
#include "UVFrame.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
URHO3D_DEFINE_APPLICATION_MAIN(StaticScene)

//=============================================================================
//=============================================================================
StaticScene::StaticScene(Context* context) :
    Sample(context)
{
    UVFrame::RegisterObject(context);
}

void StaticScene::Setup()
{
    engineParameters_["WindowTitle"]  = GetTypeName();
    engineParameters_["LogName"]      = GetSubsystem<FileSystem>()->GetProgramDir() + "shooterdemo.log";
    engineParameters_["FullScreen"]   = false;
    engineParameters_["Headless"]     = false;
    engineParameters_["WindowWidth"]  = 1280; 
    engineParameters_["WindowHeight"] = 720;
}

void StaticScene::Start()
{
    // Execute base class startup
    Sample::Start();

    ChangeDebugHudText();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateInstructions();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_RELATIVE);

    CreateSequencers();
}

void StaticScene::ChangeDebugHudText()
{
    // change profiler text
    if (GetSubsystem<DebugHud>())
    {
        Text *dbgText = GetSubsystem<DebugHud>()->GetProfilerText();
        dbgText->SetColor(Color::CYAN);
        dbgText->SetTextEffect(TE_NONE);

        dbgText = GetSubsystem<DebugHud>()->GetStatsText();
        dbgText->SetColor(Color::CYAN);
        dbgText->SetTextEffect(TE_NONE);

        dbgText = GetSubsystem<DebugHud>()->GetMemoryText();
        dbgText->SetColor(Color::CYAN);
        dbgText->SetTextEffect(TE_NONE);

        dbgText = GetSubsystem<DebugHud>()->GetModeText();
        dbgText->SetColor(Color::CYAN);
        dbgText->SetTextEffect(TE_NONE);
    }
}

void StaticScene::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();
    cameraNode_->SetPosition(Vector3(0.0f, 2.0f, -10.0f));

    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(50.0f, 0.5f, 50.0f));
    planeNode->SetPosition(Vector3(-5.0f, 0.0f, 0.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("UVFrame/Materials/PlaneMat.xml"));

    Node* planeNode2 = scene_->CreateChild("Plane");
    planeNode2->SetScale(Vector3(50.0f, 0.5f, 50.0f));
    planeNode2->SetPosition(Vector3(-5.0f, 25.0f, 10.0f));
    planeNode2->SetRotation(Quaternion(90.0f, Vector3(1.0f, 0.0f, 0.0f)));
    StaticModel* planeObject2 = planeNode2->CreateComponent<StaticModel>();
    planeObject2->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    planeObject2->SetMaterial(cache->GetResource<Material>("UVFrame/Materials/PlaneMat.xml"));


    drawDebug_ = false;
}

void StaticScene::CreateSequencers()
{
    // uv frame sequencers
    nodeList_.Push(CreateUVFrame("dirt1_", "UVFrame/Sequences/dirt1/dirt",           Vector2(3.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("dirt2_", "UVFrame/Sequences/dirt2/dirt",           Vector2(3.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("fire1_", "UVFrame/Sequences/bigfire/bigfire",      Vector2(3.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("expl1_", "UVFrame/Sequences/explosion1/explosion", Vector2(3.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("expl2_", "UVFrame/Sequences/explosion2/explosion", Vector2(4.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("expl3_", "UVFrame/Sequences/explosion3/explosion", Vector2(4.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("expl4_", "UVFrame/Sequences/explosion4/explosion", Vector2(3.0f, 3.0f)));
    nodeList_.Push(CreateUVFrame("expl5_", "UVFrame/Sequences/explosion5/explosion", Vector2(3.0f, 3.0f)));

    nodeIdx_ = 0;
    nodeList_[nodeIdx_]->SetEnabled(true);
}

Node* StaticScene::CreateUVFrame(const String &name, const String &path, const Vector2 &bsize)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Node *node = scene_->CreateChild(name);
    node->SetEnabled(false);
    node->SetPosition(Vector3(0,1.0f,0));
    BillboardSet *bbset = node->CreateComponent<BillboardSet>();
    bbset->SetNumBillboards(1);
    bbset->SetMaterial(cache->GetResource<Material>(path + String("MatAlpha.xml")));
    bbset->SetFaceCameraMode(FC_ROTATE_Y);

    bbset->GetBillboard(0)->size_ = bsize;
    bbset->GetBillboard(0)->position_ = Vector3(0, bsize.y_ * 0.5f, 0);
    bbset->GetBillboard(0)->enabled_ = true;
    bbset->Commit();

    UVFrame *uvFrame = node->CreateComponent<UVFrame>();
    XMLFile *xmlDat = cache->GetResource<XMLFile>(path + String("UVFrame.xml"));
    uvFrame->LoadXML(xmlDat->GetRoot());

    return node;
}

void StaticScene::CreateInstructions()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    // Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetColor(Color::GREEN);
    instructionText->SetText("<- Q and E ->");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    //instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, 40);
}

void StaticScene::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    // at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    // use, but now we just use full screen and default render path configured in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void StaticScene::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    // Use the Translate() function (default local space) to move relative to the node's orientation.
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void StaticScene::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(StaticScene, HandleUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(StaticScene, HandlePostRenderUpdate));
}

void StaticScene::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);

    // cycle sequence images
    Input* input = GetSubsystem<Input>();
    if (input->GetKeyPress(KEY_Q))
    {
        if (debounceTimer_.GetMSec(false) > 250)
        {
            nodeList_[nodeIdx_]->SetEnabled(false);
            if (--nodeIdx_ > nodeList_.Size() )
            {
                nodeIdx_ = nodeList_.Size() - 1;
            }
            nodeList_[nodeIdx_]->SetEnabled(true);
            nodeList_[nodeIdx_]->GetComponent<UVFrame>()->Reset();

            debounceTimer_.Reset();
        }
    }
    if (input->GetKeyPress(KEY_E))
    {
        if (debounceTimer_.GetMSec(false) > 250)
        {
            nodeList_[nodeIdx_]->SetEnabled(false);
            nodeIdx_ = ++nodeIdx_ % nodeList_.Size();
            nodeList_[nodeIdx_]->SetEnabled(true);
            nodeList_[nodeIdx_]->GetComponent<UVFrame>()->Reset();

            debounceTimer_.Reset();
        }
    }
}

void StaticScene::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // If draw debug mode is enabled, draw viewport debug geometry. This time use depth test, as otherwise the result becomes
    // hard to interpret due to large object count
    if (drawDebug_)
        GetSubsystem<Renderer>()->DrawDebugGeometry(true);
}

