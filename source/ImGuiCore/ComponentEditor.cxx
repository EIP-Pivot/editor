#include "ImGuiCore/ComponentEditor.hxx"
#include <misc/cpp/imgui_stdlib.h>

void ComponentEditor::create(Entity entity)
{
    currentEntity = entity;
    ImGui::Begin("Component editor");
    createPopUp();
    ImGui::InputText("Name", &gSceneManager.getCurrentLevel().getEntityName(currentEntity));
    displayComponent<RigidBody>("Rigid body");
    displayComponent<Gravity>("Gravity");
    displayComponent<pivot::graphics::RenderObject>("Render Object");
    if (ImGui::Button("Add Component")) { ImGui::OpenPopup("AddComponent"); }
    ImGui::End();
}

void ComponentEditor::create()
{
    ImGui::Begin("Component editor");
    ImGui::Text("No entity selected.");
    ImGui::End();
}

void ComponentEditor::setVectorObject(LevelId scene) { sceneObject[scene] = ObjectVector(); }

std::unordered_map<LevelId, ObjectVector> &ComponentEditor::getVectorObject() { return sceneObject; }

ObjectVector ComponentEditor::getObject() { return sceneObject[gSceneManager.getCurrentLevelId()]; }

void ComponentEditor::createPopUp()
{
    if (ImGui::BeginPopup("AddComponent")) {
        if (!gSceneManager.getCurrentLevel().hasComponent<RigidBody>(currentEntity)) {
            if (ImGui::MenuItem("Rigid Body")) { addComponent<RigidBody>(RigidBody()); }
        }
        if (!gSceneManager.getCurrentLevel().hasComponent<Gravity>(currentEntity)) {
            if (ImGui::MenuItem("Gravity")) { addComponent<Gravity>(Gravity()); }
        }
        if (!gSceneManager.getCurrentLevel().hasComponent<pivot::graphics::RenderObject>(currentEntity)) {
            if (ImGui::MenuItem("Render object")) {
                addComponent<pivot::graphics::RenderObject>({
                    .meshID = "cube",
                    .objectInformation =
                        {
                            .transform = Transform(glm::vec3(), glm::vec3(), glm::vec3(1.0f)),
                            .textureIndex = "blanc",
                            .materialIndex = "white",
                        },
                });
            }
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Textures")) {
        for (std::string texture: textures) {
            if (ImGui::MenuItem(texture.c_str())) {
                gSceneManager.getCurrentLevel()
                    .GetComponent<pivot::graphics::RenderObject>(currentEntity)
                    .objectInformation.textureIndex = texture;
            }
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Models")) {
        for (std::string model: models) {
            if (ImGui::MenuItem(model.c_str())) {
                gSceneManager.getCurrentLevel().GetComponent<pivot::graphics::RenderObject>(currentEntity).meshID =
                    model;
            }
        }
        ImGui::EndPopup();
    }
}

void ComponentEditor::createComponent(RigidBody &rigidBody)
{
    ImGui::Indent();
    ImGui::InputFloat3("Acceleration", glm::value_ptr(rigidBody.acceleration));
    ImGui::InputFloat3("Velocity", glm::value_ptr(rigidBody.velocity));
    ImGui::Unindent();
}

void ComponentEditor::createComponent(Gravity &gravity)
{
    ImGui::Indent();
    ImGui::InputFloat3("Force", glm::value_ptr(gravity.force));
    ImGui::Unindent();
}

void ComponentEditor::createComponent(pivot::graphics::RenderObject &renderObject)
{
    ImGui::Indent();
    if (ImGui::InputFloat3("Tr", glm::value_ptr(matrixTranslation)))
        renderObject.objectInformation.transform.setPosition(matrixTranslation);
    if (ImGui::InputFloat3("Rt", glm::value_ptr(matrixRotation)))
        renderObject.objectInformation.transform.setRotation(matrixRotation);
    if (ImGui::InputFloat3("Sc", glm::value_ptr(matrixScale)))
        renderObject.objectInformation.transform.setScale(matrixScale);
    if (ImGui::Selectable(gSceneManager.getCurrentLevel()
                              .GetComponent<pivot::graphics::RenderObject>(currentEntity)
                              .objectInformation.textureIndex.c_str(),
                          false, 0, ImVec2(50, 15)))
        ImGui::OpenPopup("Textures");
    if (ImGui::Selectable(
            gSceneManager.getCurrentLevel().GetComponent<pivot::graphics::RenderObject>(currentEntity).meshID.c_str(),
            false, 0, ImVec2(50, 15)))
        ImGui::OpenPopup("Models");
    ImGui::Unindent();
}

template <>
void ComponentEditor::addComponent(pivot::graphics::RenderObject renderObject)
{
    if (!gSceneManager.getCurrentLevel().isRegister<pivot::graphics::RenderObject>())
        gSceneManager.getCurrentLevel().RegisterComponent<pivot::graphics::RenderObject>();
    gSceneManager.getCurrentLevel().AddComponent<pivot::graphics::RenderObject>(currentEntity, renderObject);
    sceneObject[gSceneManager.getCurrentLevelId()].push_back(
        gSceneManager.getCurrentLevel().GetComponent<pivot::graphics::RenderObject>(currentEntity));
}

template <>
void ComponentEditor::addComponent(Entity entity, pivot::graphics::RenderObject renderObject)
{
    if (!gSceneManager.getCurrentLevel().isRegister<pivot::graphics::RenderObject>())
        gSceneManager.getCurrentLevel().RegisterComponent<pivot::graphics::RenderObject>();
    gSceneManager.getCurrentLevel().AddComponent<pivot::graphics::RenderObject>(entity, renderObject);
    sceneObject[gSceneManager.getCurrentLevelId()].push_back(
        gSceneManager.getCurrentLevel().GetComponent<pivot::graphics::RenderObject>(entity));
}