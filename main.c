#include <stdio.h>
#include "cvis/vis.h"
#include "cmat/vec3d.h"

Vec3d initial_position = {
    39.750661009097016,
    -104.99540760820614,
    1609.34
};

Vec3d initial_velocity = {
  0,
  0,
  0
};

Vec3d iniital_orientation = {
    0,
    0,
    0
};

typedef struct ImuErrorModel {

} ImuErrorModel;


void DisplayConfigurationWindow() {
  if (igBegin("Configuration", NULL, ImGuiWindowFlags_None)) {

    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Position", ImGuiTreeNodeFlags_None)) {
      igInputDouble("Latitude  (deg)", &initial_position.vec[0], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Longitude (deg)", &initial_position.vec[1], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Height    (m)", &initial_position.vec[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Velocity", ImGuiTreeNodeFlags_None)) {
      igInputDouble("North (m/s)", &initial_velocity.vec[0], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("East  (m/s)", &initial_velocity.vec[1], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Down  (m/s)", &initial_velocity.vec[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Orientation", ImGuiTreeNodeFlags_None)) {
      igInputDouble("Roll  (deg)", &iniital_orientation.vec[0], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Pitch (deg)", &iniital_orientation.vec[1], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Yaw   (deg)", &iniital_orientation.vec[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
  }
  igEnd();
}

int main(int argc, char *argv[]) {
  visWindow_Initialize("GNSS INS", 800, 800);

  while (!visWindow_ShouldClose()) {
    visWindow_NewFrame();

    DisplayConfigurationWindow();

    visWindow_EndFrame();
  }
}