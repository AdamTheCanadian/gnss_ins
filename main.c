#include <stdio.h>
#include "cvis/vis.h"

double initial_position[3] = {
    39.750661009097016,
    -104.99540760820614,
    1609.34
};

void DisplayConfigurationWindow() {
  if (igBegin("Configuration", NULL, ImGuiWindowFlags_None)) {

    if (igCollapsingHeader_TreeNodeFlags("Initial Starting Position", ImGuiTreeNodeFlags_None)) {
      igInputDouble("Latitude  (deg)", &initial_position[0], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Longitude (deg)", &initial_position[1], -0.1, 10, "%.9f", ImGuiInputTextFlags_None);
      igInputDouble("Height    (m)", &initial_position[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
    }
  }
}

int main(int argc, char *argv[]) {
  visWindow_Initialize("GNSS INS", 800, 800);

  while (!visWindow_ShouldClose()) {
    visWindow_NewFrame();

    DisplayConfigurationWindow();

    visWindow_EndFrame();
  }
}