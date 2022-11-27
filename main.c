#include <stdio.h>
#include "cvis/vis.h"
#include "cmat/vec.h"
#include "cmat/mat3d.h"
#include "cmat/rotations.h"]
#include "cgeodesy/geodetic_position.h"

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

Vec3d initial_orientation = {
    0,
    0,
    0
};

Vec3d initial_position_error = {0, 0, 0};
Vec3d initial_velocity_error = {0, 0, 0};
Vec3d initial_orientation_error = {0, 0, 0};

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
      igInputDouble("Roll  (deg)", &initial_orientation.vec[0], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Pitch (deg)", &initial_orientation.vec[1], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
      igInputDouble("Yaw   (deg)", &initial_orientation.vec[2], -0.1, 10, "%.3f", ImGuiInputTextFlags_None);
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

static void RunSimulation();

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

void RunSimulation() {
  static const double micro_g_to_metres_per_second_squared = 9.80665e-6;
  static const double deg_to_rad = 0.0174532925199433;
  static const double rad_to_deg = 1.0 / deg_to_rad;

  /* Create a copy of the imu model since we need to do some unit conversions before running simulator */
  ImuErrorModel imu_model = imu_error_model;

  Vec3d_Scale(&imu_model.accelerometer_biases, micro_g_to_metres_per_second_squared);
  Vec3d_Scale(&imu_model.gyro_biases, deg_to_rad / 3600.0);

  Mat3d_Scale(&imu_model.accelerometer_m, 1.0e-6);
  Mat3d_Scale(&imu_model.gyro_m, 1.0e-6);
  Mat3d_Scale(&imu_model.gyro_g, (deg_to_rad) / (3600 * 9.80665));

  imu_model.accelerometer_noise *= micro_g_to_metres_per_second_squared;
  imu_model.gyro_noise *= deg_to_rad / 60;

  Vec3d orientation_in_radians = initial_orientation;
  Vec3d_Scale(&orientation_in_radians, deg_to_rad);
  Mat3d R_b_n = Rotations_EulerToR(&orientation_in_radians);

  /* Initialize the nav solution using the true solution and the initialization errors */
  PosLLH starting_position = {
      .latitude_radians = initial_position.x * rad_to_deg,
      .longitude_radians = initial_position.y * rad_to_deg,
      .height_metres = initial_position.z
  };
  GeoVec position_error = {
      .north_metres = initial_position_error.x,
      .east_metres = initial_position_error.y,
      .down_metres = initial_position_error.z
  };

  Geodesy_TranslatePosLLH(&starting_position, &position_error);

  Vec3d starting_velocity = {
      .x = initial_velocity.x + initial_velocity_error.x,
      .y = initial_velocity.y + initial_velocity_error.y,
      .z = initial_velocity.z + initial_velocity_error.z
  };

  Vec3d tmp = initial_orientation_error;
  Vec3d_Scale(&tmp, -1);
  Mat3d starting_orientation = Rotations_EulerToR(&tmp);

}