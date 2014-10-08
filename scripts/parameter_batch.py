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
ptam_over_param_range('Bundle', 'RotationEstimatorBlur', 0, 200, 5, 0)
ptam_over_param_range('Bundle', 'UpdateSquaredConvergenceLimit', 1, 40, 1, -7)
ptam_over_param_range('Map', 'PatchSSDThreshold', 5, 200, 5, 2)
ptam_over_param_range('MapMaker', 'MaxKFDistWiggleMult', 10, 200, 5, -2)
ptam_over_param_range('Tracker', 'CoarseMinVelocity', 0, 40, 1, -3)
ptam_over_param_range('Tracker', 'CoarseRange', 0, 80, 2, 0)
ptam_over_param_range('Tracker', 'CoarseSubPixIts', 0, 40, 1, 0)
ptam_over_param_range('Tracker', 'MaxPatchesPerFrame', 1, 40, 1, 2)
ptam_over_param_range('Tracker', 'RotationEstimatorBlur', 0, 200, 5, -2)
ptam_over_param_range('Tracker', 'ShiTomasiThreshold', 50, 130, 2, 0)
ptam_over_param_range('Tracker', 'TrackingQualityGood', 20, 90, 2, -2)
ptam_over_param_range('Tracker', 'UseRotationEstimator', 0, 1, 1, 0)


# PatchSSDThreshold has a non-standard range
minv = 5
maxv = 200
interval = 5
for PatchSSDThreshold in range(minv, maxv + interval, interval):
  cmd = 'cp settings_template.cfg settings.cfg'
  os.system(cmd)

  myfile = open(r"settings.cfg","a")
  myfile.write("\n" + "Tracker.PatchSSDThreshold=" + str(PatchSSDThreshold \
    * PatchSSDThreshold * 2))

  cmd = './UTPTAM'
  os.system(cmd)

  cmd = 'mv coordinates.log coord_PatchSSDThreshold_' \
    + str(PatchSSDThreshold) + '.txt'
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
    myfile.write("\n" + "Tracker.CoarseMin=" + str(CourseMin))
    myfile.write("\n" + "Tracker.CoarseMax=" + str(CoarseMax))

    cmd = './UTPTAM'
    os.system(cmd)

    cmd = 'mv coordinates.log coord_CoarseMin_' + str(CoarseMin) + \
      '_CourseMax_' + str(CoarseMax) + '.txt'
    os.system(cmd)


# TODO:
# FAST.Barrier0=10 : [6:4:22]
# FAST.Barrier1=15 : [11:4:27]
# FAST.Barrier2=15 : [11:4:27]
# FAST.Barrier3=10 : [6:4:22]

