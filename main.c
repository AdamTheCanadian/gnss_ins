#include <stdio.h>
#include "cvis/vis.h"

double initial_position[3] = {
    39.750661009097016,
    -104.99540760820614,
    1609.34
};

double initial_velocity[3] = {
  0,
  0,
  0
};

double iniital_orientation[3] = {
    0,
    0,
    0
};

void DisplayConfigurationWindow() {
  if (igBegin("Configuration", NULL, ImGuiWindowFlags_None)) {

    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Position", ImGuiTreeNodeFlags_None)) {
      igInputDouble("Latitude  (deg)", &initial_position[0], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Longitude (deg)", &initial_position[1], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Height    (m)", &initial_position[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Velocity", ImGuiTreeNodeFlags_None)) {
      igInputDouble("North (m/s)", &initial_velocity[0], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("East  (m/s)", &initial_velocity[1], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Down  (m/s)", &initial_velocity[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Orientation", ImGuiTreeNodeFlags_None)) {
      igInputDouble("Roll  (deg)", &iniital_orientation[0], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Pitch (deg)", &iniital_orientation[1], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Yaw   (deg)", &iniital_orientation[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
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