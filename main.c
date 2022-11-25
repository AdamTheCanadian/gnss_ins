#include <stdio.h>
#include "cvis/vis.h"
#include "cmat/vec.h"
#include "cmat/mat3d.h"

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
  Vec3d accelerometer_biases;
  Vec3d gyro_biases;

  /* Accelerometer scale factor and cross coupling */
  Mat3d accelerometer_m;
  /* Gyroscope scale factor and cross coupling */
  Mat3d gyro_m;
  /* Gyroscope g dependent biases */
  Mat3d gyro_g;

  double accelerometer_noise;
  double gyro_noise;
  double accelerometer_quantization;
  double gyro_quantization;
} ImuErrorModel;

static struct ImuErrorModel imu_error_model;

static void InitializeImuModel();

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

    if (igCollapsingHeader_TreeNodeFlags("IMU Model", ImGuiTreeNodeFlags_None)) {
      /* Imgui really only has support for floats so we will convert our imu model parameters to floats for
       * user inputs and then convert back. Annoying but only option for now */
      static Vec3f tmp;
      tmp = Vec3d_ToVec3f(&imu_error_model.accelerometer_biases);
      if (igInputFloat3("Accelerometer Biases", &tmp.vec[0], "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.accelerometer_biases = Vec3f_ToVec3d(&tmp);
      }

      tmp = Vec3d_ToVec3f(&imu_error_model.gyro_biases);
      if (igInputFloat3("Gyro Biases", &tmp.vec[0], "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.gyro_biases = Vec3f_ToVec3d(&tmp);
      }

      igText("Accelerometer Scale Factor and Cross Coupling");
      for (int i = 0; i < 3; i++) {
        Vec3d tmpd;
        tmpd = Mat3d_GetRow(&imu_error_model.accelerometer_m, i);
        tmp = Vec3d_ToVec3f(&tmpd);
        /* Since the inputFloat is going to be any empty label we need to tell Imgui
         * to treat each loop (i) as a unique ID. If we dont do this the three sliders (loop 3 times)
         * will all be linked together */
        int unique_id = 100;
        igPushID_Int(unique_id + i);
        if (igInputFloat3("##ID", &tmp.vec[0], "%.3f", ImGuiInputTextFlags_None)) {
          tmpd = Vec3f_ToVec3d(&tmp);
          Mat3d_SetRow(&imu_error_model.accelerometer_m, &tmpd, i);
        }
        igPopID();
      }

      igText("Gyro Scale Factor and Cross Coupling");
      for (int i = 0; i < 3; i++) {
        Vec3d tmpd;
        tmpd = Mat3d_GetRow(&imu_error_model.gyro_m, i);
        tmp = Vec3d_ToVec3f(&tmpd);
        /* Since the inputFloat is going to be any empty label we need to tell Imgui
         * to treat each loop (i) as a unique ID. If we dont do this the three sliders (loop 3 times)
         * will all be linked together */
        int unique_id = 200;
        igPushID_Int(unique_id + i);
        if (igInputFloat3("", &tmp.vec[0], "%.3f", ImGuiInputTextFlags_None)) {
          tmpd = Vec3f_ToVec3d(&tmp);
          Mat3d_SetRow(&imu_error_model.gyro_m, &tmpd, i);
        }
        igPopID();
      }

      igText("Gyro G-dependent biases");
      for (int i = 0; i < 3; i++) {
        Vec3d tmpd;
        tmpd = Mat3d_GetRow(&imu_error_model.gyro_g, i);
        tmp = Vec3d_ToVec3f(&tmpd);
        /* Since the inputFloat is going to be any empty label we need to tell Imgui
         * to treat each loop (i) as a unique ID. If we dont do this the three sliders (loop 3 times)
         * will all be linked together */
        int unique_id = 300;
        igPushID_Int(unique_id + i);
        if (igInputFloat3("", &tmp.vec[0], "%.3f", ImGuiInputTextFlags_None)) {
          tmpd = Vec3f_ToVec3d(&tmp);
          Mat3d_SetRow(&imu_error_model.gyro_g, &tmpd, i);
        }
        igPopID();
      }

      float tmpf = 0;
      tmpf = (float)imu_error_model.accelerometer_noise;
      if (igInputFloat("Accelerometer Noise", &tmpf, 1.0f, 1.0f, "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.accelerometer_noise = tmpf;
      }
      tmpf = (float)imu_error_model.gyro_noise;
      if (igInputFloat("Gyro Noise", &tmpf, 1.0f, 1.0f, "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.gyro_noise = tmpf;
      }
      tmpf = (float)imu_error_model.accelerometer_quantization;
      if (igInputFloat("Accelerometer Quantization", &tmpf, 0.01f, 1.0f, "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.accelerometer_quantization = tmpf;
      }
      tmpf = (float)imu_error_model.gyro_quantization;
      if (igInputFloat("Gyro Quantization", &tmpf, 0.01f, 1.0f, "%.3f", ImGuiInputTextFlags_None)) {
        imu_error_model.gyro_quantization = tmpf;
      }
    }
  }
  igEnd();
}

int main(int argc, char *argv[]) {
  visWindow_Initialize("GNSS INS", 800, 800);

  InitializeImuModel();

  while (!visWindow_ShouldClose()) {
    visWindow_NewFrame();

    DisplayConfigurationWindow();

    visWindow_EndFrame();
  }
}

void InitializeImuModel() {
  Vec3d_SetZero(&imu_error_model.accelerometer_biases);
  Vec3d_SetZero(&imu_error_model.gyro_biases);

  Mat3d_SetZero(&imu_error_model.accelerometer_m);
  Mat3d_SetZero(&imu_error_model.gyro_m);
  Mat3d_SetZero(&imu_error_model.gyro_g);

  imu_error_model.accelerometer_noise = 0;
  imu_error_model.gyro_noise = 0;
  imu_error_model.accelerometer_quantization = 0;
  imu_error_model.gyro_quantization = 0;
}