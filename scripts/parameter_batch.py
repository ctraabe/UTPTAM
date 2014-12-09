import os

# This function executes PTAM for each value of a parameter according to the
# specified interval and range.
def ptam_over_param_range( group, param, minv, maxv, interval, exponent ):
  for paramv in range(minv, maxv + interval, interval):
    cmd = 'cp settings_template.cfg settings.cfg'
    os.system(cmd)

    myfile = open(r"settings.cfg","a")
    myfile.write("\n" + group + "." + param + "=" + str(paramv * pow(10.0, \
      exponent)))
    myfile.close()

    cmd = './UTPTAM'
    os.system(cmd)

    cmd = 'mv coordinates.log coord_' + param + '_' + str(paramv) + 'e' + \
      str(exponent) + '.txt'
    os.system(cmd)
  return


# Go to the PTAM directory.
os.chdir('..')


# Run PTAM over each of the following paramter ranges:
ptam_over_param_range('Bundle', 'MaxIterations', 2, 80, 2, 0)
ptam_over_param_range('Bundle', 'UpdateSquaredConvergenceLimit', 1, 40, 1, -7)
ptam_over_param_range('Map', 'PatchSSDThreshold', 5, 200, 5, 2)
ptam_over_param_range('MapMaker', 'MaxKFDistWiggleMult', 10, 200, 5, -2)
ptam_over_param_range('Tracker', 'CoarseMinVelocity', 0, 32, 1, -3)
ptam_over_param_range('Tracker', 'CoarseRange', 0, 1000, 25, -1)
ptam_over_param_range('Tracker', 'CoarseSubPixIts', 0, 40, 1, 0)
ptam_over_param_range('Tracker', 'MaxPatchesPerFrame', 1, 40, 1, 2)
ptam_over_param_range('Tracker', 'PatchSSDThreshold', 50, 1000, 25, 2)
ptam_over_param_range('Tracker', 'RotationEstimatorBlur', 0, 80, 2, -1)
ptam_over_param_range('Tracker', 'ShiTomasiThreshold', 0, 200, 5, 0)
ptam_over_param_range('Tracker', 'TrackingQualityGood', 20, 90, 2, -2)
ptam_over_param_range('Tracker', 'UseRotationEstimator', 0, 1, 1, 0)


# Set all barriers to the same value
minv = 150
maxv = 350
interval = 5
exponent = -1
for Barrier in range(minv, maxv + interval, interval):
  cmd = 'cp settings_template.cfg settings.cfg'
  os.system(cmd)

  myfile = open(r"settings.cfg","a")
  myfile.write("\n" + "FAST.Barrier0=" + str(Barrier * pow(10.0, exponent)))
  myfile.write("\n" + "FAST.Barrier1=" + str(Barrier * pow(10.0, exponent)))
  myfile.write("\n" + "FAST.Barrier2=" + str(Barrier * pow(10.0, exponent)))
  myfile.write("\n" + "FAST.Barrier3=" + str(Barrier * pow(10.0, exponent)))
  myfile.close()

  cmd = './UTPTAM'
  os.system(cmd)

  cmd = 'mv coordinates.log coord_Barrier_' + str(Barrier) + 'e' + \
    str(exponent) + '.txt'
  os.system(cmd)


# CourseMin and CourseMax are inter-dependent
minv = 5
maxv = 120
interval = 15
for CoarseMax in range(minv, maxv + interval, interval):
  for CoarseMin in range(minv, CoarseMax, interval):
    cmd = 'cp settings_template.cfg settings.cfg'
    os.system(cmd)

    myfile = open(r"settings.cfg","a")
    myfile.write("\n" + "Tracker.CoarseMin=" + str(CoarseMin))
    myfile.write("\n" + "Tracker.CoarseMax=" + str(CoarseMax))
    myfile.close()

    cmd = './UTPTAM'
    os.system(cmd)

    cmd = 'mv coordinates.log coord_CoarseMin_' + str(CoarseMin) + \
      '_CourseMax_' + str(CoarseMax) + '.txt'
    os.system(cmd)
